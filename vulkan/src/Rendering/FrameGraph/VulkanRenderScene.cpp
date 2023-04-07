/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/FrameGraph/VulkanRenderScene.hpp"

#include "Rendering/FrameGraph/VulkanRenderSceneGBuffer.hpp"
#include "Rendering/FrameGraph/VulkanRenderSceneLighting.hpp"
#include "Rendering/FrameGraph/VulkanRenderSceneUnlit.hpp"
#include "Rendering/FrameGraph/VulkanRenderShadowMaps.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Node.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/VulkanFetchSceneRenderState.hpp"

using namespace crimild::vulkan;
using namespace crimild::vulkan::framegraph;

RenderScene::RenderScene(
    RenderDevice *device,
    std::string name,
    std::shared_ptr< Camera > const &camera,
    std::shared_ptr< ImageView > const &output,
    SyncOptions const &options
) noexcept
    : RenderBase( device, name, output ),
      m_camera( camera ),
      m_output( output ),
      m_syncOptions( options )
{
    const auto extent = VkExtent2D {
        m_output->getExtent().width,
        m_output->getExtent().height,
    };

    m_renderTargets[ getName() + "/Targets/Color" ] = crimild::alloc< RenderTarget >( device, getName() + "/Targets/Color", output );

    // Set scene color to opaque
    m_renderTargets[ getName() + "/Targets/Color" ]->setClearValue( VkClearValue { .color = { .float32 = { 0, 0, 0, 1 } } } );

    m_renderTargets[ getName() + "/Targets/Depth" ] = crimild::alloc< RenderTarget >( device, getName() + "/Targets/Depth", VK_FORMAT_D32_SFLOAT, extent );
    m_renderTargets[ getName() + "/Targets/Albedo" ] = crimild::alloc< RenderTarget >( device, getName() + "/Targets/Albedo", VK_FORMAT_R32G32B32A32_SFLOAT, extent );
    m_renderTargets[ getName() + "/Targets/Position" ] = crimild::alloc< RenderTarget >( device, getName() + "/Targets/Position", VK_FORMAT_R32G32B32A32_SFLOAT, extent );
    m_renderTargets[ getName() + "/Targets/Normal" ] = crimild::alloc< RenderTarget >( device, getName() + "/Targets/Normal", VK_FORMAT_R32G32B32A32_SFLOAT, extent );
    m_renderTargets[ getName() + "/Targets/Material" ] = crimild::alloc< RenderTarget >( device, getName() + "/Targets/Material", VK_FORMAT_R32G32B32A32_SFLOAT, extent );

    m_shadows = crimild::alloc< RenderShadowMaps >( device );

    m_gBuffer = crimild::alloc< RenderSceneGBuffer >(
        device,
        extent,
        std::vector< std::shared_ptr< RenderTarget > > {
            m_renderTargets[ getName() + "/Targets/Depth" ],
            // m_renderTargets[ getName() + "/Targets/Color" ],
            m_renderTargets[ getName() + "/Targets/Albedo" ],
            m_renderTargets[ getName() + "/Targets/Position" ],
            m_renderTargets[ getName() + "/Targets/Normal" ],
            m_renderTargets[ getName() + "/Targets/Material" ],
        }
    );

    m_lighting = crimild::alloc< RenderSceneLighting >(
        device,
        extent,
        std::vector< std::shared_ptr< RenderTarget > > {
            m_renderTargets[ getName() + "/Targets/Albedo" ],
            m_renderTargets[ getName() + "/Targets/Position" ],
            m_renderTargets[ getName() + "/Targets/Normal" ],
            m_renderTargets[ getName() + "/Targets/Material" ],
        },
        m_renderTargets[ getName() + "/Targets/Color" ]
    );

    m_unlit = crimild::alloc< RenderSceneUnlit >(
        device,
        extent,
        m_renderTargets[ getName() + "/Targets/Depth" ],
        m_renderTargets[ getName() + "/Targets/Color" ]
    );

    m_environment = crimild::alloc< RenderSceneUnlit >(
        device,
        extent,
        m_renderTargets[ getName() + "/Targets/Depth" ],
        m_renderTargets[ getName() + "/Targets/Color" ]
    );
}

void RenderScene::onResize( void ) noexcept
{
    // TODO
}

void RenderScene::execute( void ) noexcept
{
    auto scene = Simulation::getInstance()->getScene();
    const auto renderState =
        scene != nullptr
            ? scene->perform< FetchSceneRenderState >()
            : FetchSceneRenderState::Result {};

    if ( m_camera == nullptr ) {
        m_camera = retain( Simulation::getInstance()->getMainCamera() );
    }

    if ( m_camera != nullptr ) {
        // Set correct aspect ratio for camera before rendering
        m_camera->setAspectRatio(
            float( getOutputImage()->getExtent().width ) / float( getOutputImage()->getExtent().height )
        );
    }

    // Execute shadow pass. No need to add sync objects since render pass will
    // leave all attachments in the correct layout
    m_shadows->render( 
        renderState, 
        m_camera.get(),
        { 
            .signal = { m_shadows->getSemaphore() },
        }
    );

    m_gBuffer->render(
        renderState.litRenderables,
        m_camera.get(),
        {
            .pre = {
                // No need to add barriers here since render pass dependencies will deal with
                // any layout transition.
                .imageMemoryBarriers = {
                    // Since this is the last pass, make sure the color target is
                    // in the correct layout and can be read later on.
                    ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .dstAccessMask = 0,
                        .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .imageView = m_renderTargets[ getName() + "/Targets/Color" ]->getImageView(),
                    },
                    ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .dstAccessMask = 0,
                        .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .imageView = m_renderTargets[ getName() + "/Targets/Albedo" ]->getImageView(),
                    },
                    ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .dstAccessMask = 0,
                        .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .imageView = m_renderTargets[ getName() + "/Targets/Position" ]->getImageView(),
                    },
                    ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .dstAccessMask = 0,
                        .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .imageView = m_renderTargets[ getName() + "/Targets/Normal" ]->getImageView(),
                    },
                    ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .dstAccessMask = 0,
                        .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .imageView = m_renderTargets[ getName() + "/Targets/Material" ]->getImageView(),
                    },
                },

            },
            .post = {
                // Barriers for layout transitions will be added later (see below) in order to
                // wait for them only when they're going to be used.
            },
        }
    );

    m_lighting->render(
        renderState,
        m_camera.get(),
        SyncOptions {
            .pre = {
                // Make sure all G-Buffer targets are in the correct layout
                // Do it here to avoid waiting before they are actually needed
                .imageMemoryBarriers = {
                    ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_renderTargets[ getName() + "/Targets/Albedo" ]->getImageView(),
                    },
                    ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_renderTargets[ getName() + "/Targets/Position" ]->getImageView(),
                    },
                    ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_renderTargets[ getName() + "/Targets/Normal" ]->getImageView(),
                    },
                    ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_renderTargets[ getName() + "/Targets/Material" ]->getImageView(),
                    },
                },
            },
            .wait = { m_shadows->getSemaphore() },
        }
    );

    m_unlit->render(
        renderState.unlitRenderables,
        m_camera.get()
    );

    m_environment->render(
        renderState.envRenderables,
        m_camera.get(),
        {
            .post = {
                .imageMemoryBarriers = {
                    // Since this is the last pass, make sure the color target is
                    // in the correct layout and can be read later on.
                    ImageMemoryBarrier {
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_renderTargets[ getName() + "/Targets/Color" ]->getImageView(),
                    },
                },
            },

            // This is the last pass, so signal semaphores if needed.
            .signal = m_syncOptions.signal,
        }
    );
}
