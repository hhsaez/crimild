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

#include "Rendering/FrameGraph/VulkanComputeSceneLighting.hpp"
#include "Rendering/FrameGraph/VulkanRenderSceneGBuffer.hpp"
#include "Rendering/FrameGraph/VulkanRenderSceneLighting.hpp"
#include "Rendering/FrameGraph/VulkanRenderSceneUnlit.hpp"
#include "Rendering/FrameGraph/VulkanRenderShadowMaps.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/VulkanFetchSceneRenderState.hpp"

using namespace crimild::vulkan;
using namespace crimild::vulkan::framegraph;

RenderScene::RenderScene( RenderDevice *device, const VkExtent2D &extent )
    : RenderBase( device, "RenderScene", extent )
{
    m_renderTargets[ "Scene/Depth" ] = crimild::alloc< RenderTarget >( device, "Scene/Depth", VK_FORMAT_D32_SFLOAT, extent );
    m_renderTargets[ "Scene/Color" ] = crimild::alloc< RenderTarget >( device, "Scene/Color", VK_FORMAT_R32G32B32A32_SFLOAT, extent );
    m_renderTargets[ "Scene/Albedo" ] = crimild::alloc< RenderTarget >( device, "Scene/Albedo", VK_FORMAT_R32G32B32A32_SFLOAT, extent );
    m_renderTargets[ "Scene/Position" ] = crimild::alloc< RenderTarget >( device, "Scene/Position", VK_FORMAT_R32G32B32A32_SFLOAT, extent );
    m_renderTargets[ "Scene/Normal" ] = crimild::alloc< RenderTarget >( device, "Scene/Normal", VK_FORMAT_R32G32B32A32_SFLOAT, extent );
    m_renderTargets[ "Scene/Material" ] = crimild::alloc< RenderTarget >( device, "Scene/Material", VK_FORMAT_R32G32B32A32_SFLOAT, extent );

    // Set scene color to opaque
    m_renderTargets[ "Scene/Color" ]->setClearValue( VkClearValue { .color = { .float32 = { 0, 0, 0, 1 } } } );

    m_shadows = crimild::alloc< RenderShadowMaps >( device );

    m_gBuffer = crimild::alloc< RenderSceneGBuffer >(
        device,
        extent,
        std::vector< std::shared_ptr< RenderTarget > > {
            m_renderTargets[ "Scene/Depth" ],
            m_renderTargets[ "Scene/Albedo" ],
            m_renderTargets[ "Scene/Position" ],
            m_renderTargets[ "Scene/Normal" ],
            m_renderTargets[ "Scene/Material" ],
        }
    );

    m_lighting = crimild::alloc< RenderSceneLighting >(
        device,
        extent,
        std::vector< std::shared_ptr< RenderTarget > > {
            m_renderTargets[ "Scene/Albedo" ],
            m_renderTargets[ "Scene/Position" ],
            m_renderTargets[ "Scene/Normal" ],
            m_renderTargets[ "Scene/Material" ],
        },
        m_renderTargets[ "Scene/Color" ]
    );

    m_unlit = crimild::alloc< RenderSceneUnlit >(
        device,
        extent,
        m_renderTargets[ "Scene/Depth" ],
        m_renderTargets[ "Scene/Color" ]
    );

    m_environment = crimild::alloc< RenderSceneUnlit >(
        device,
        extent,
        m_renderTargets[ "Scene/Depth" ],
        m_renderTargets[ "Scene/Color" ]
    );
}

void RenderScene::onResize( void ) noexcept
{
    // TODO
}

void RenderScene::render( Node *scene, Camera *camera ) noexcept
{
    const auto renderState =
        scene != nullptr
            ? scene->perform< FetchSceneRenderState >()
            : FetchSceneRenderState::Result {};

    if ( camera != nullptr ) {
        // Set correct aspect ratio for camera before rendering
        camera->setAspectRatio( float( getExtent().width ) / float( getExtent().height ) );
    }

    // Execute shadow pass. No need to add sync objects since render pass will
    // leave all attachments in the correct layout
    m_shadows->render( renderState, camera );

    m_gBuffer->render(
        renderState.litRenderables,
        camera,
        {
            .pre = {
                // No need to add barriers here since render pass dependencies will deal with
                // any layout transition.
            },
            .post = {
                // Barriers for layout transitions will be added later (see below) in order to
                // wait for them only when they're going to be used.
            },
        }
    );

    m_lighting->render(
        renderState,
        camera,
        {
            .pre = {
                // Make sure all G-Buffer targets are in the correct layout
                // Do it here to avoid waiting before they are actually needed
                .imageMemoryBarriers = {
                    ImageMemoryBarrier {
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_renderTargets[ "Scene/Albedo" ]->getImageView(),
                    },
                    ImageMemoryBarrier {
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_renderTargets[ "Scene/Position" ]->getImageView(),
                    },
                    ImageMemoryBarrier {
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_renderTargets[ "Scene/Normal" ]->getImageView(),
                    },
                    ImageMemoryBarrier {
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_renderTargets[ "Scene/Material" ]->getImageView(),
                    },
                },
            },
        }
    );

    m_unlit->render(
        renderState.unlitRenderables,
        camera
    );

    m_environment->render(
        renderState.envRenderables,
        camera,
        {
            .post = {
                .imageMemoryBarriers = {
                    // Since this is the last pass, make sure the color target is
                    // in the correct layout and can be read later on.
                    ImageMemoryBarrier {
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = m_renderTargets[ "Scene/Color" ]->getImageView(),
                    },
                },
            },
            .signal = {
                getSemaphore(),
            },
        }
    );
}
