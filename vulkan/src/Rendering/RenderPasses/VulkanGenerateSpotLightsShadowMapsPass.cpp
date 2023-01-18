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
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/RenderPasses/VulkanGenerateSpotLightsShadowMapsPass.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/perspective.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderableSet.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShadowMap.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/Vertex.hpp"
#include "Rendering/VulkanFramebuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderPass.hpp"
#include "Rendering/VulkanShadowMap.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/ApplyToGeometries.hpp"
#include "Visitors/FetchLights.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

GenerateSpotLightsShadowMaps::GenerateSpotLightsShadowMaps( RenderDevice *renderDevice ) noexcept
    : RenderPassBase( renderDevice )
{
    init();
}

GenerateSpotLightsShadowMaps::~GenerateSpotLightsShadowMaps( void ) noexcept
{
    clear();
}

Event GenerateSpotLightsShadowMaps::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::SCENE_CHANGED:
        case Event::Type::WINDOW_RESIZE: {
            clear();
            init();
            break;
        }

        default:
            break;
    }

    return e;
}

void GenerateSpotLightsShadowMaps::render(
    const SceneRenderState::Lights &lights,
    const SceneRenderState::ShadowCasters &shadowCasters,
    const Camera *camera
) noexcept
{
    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    for ( const auto *light : lights.at( Light::Type::SPOT ) ) {
        if ( light->castShadows() ) {
            if ( auto shadowMap = getRenderDevice()->getShadowMap( light ) ) {
                auto &shadowMapImage = shadowMap->images[ currentFrameIndex ];

                // Transition to transfer so we can write into the image after render.
                shadowMapImage->transitionLayout( commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

                // Use a perspective projection for light space matrix when using
                // spot lights, using the radius as limit.
                shadowMap->lightSpaceMatrices[ 0 ] = perspective( 90, 1, 0.01f, light->getRadius() ) * light->getWorld().invMat;

                renderShadowMap( light, shadowCasters, shadowMap->lightSpaceMatrices[ 0 ], shadowMapImage );

                // Transition back to read after render.
                shadowMapImage->transitionLayout( commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
            }
        }
    }
}

void GenerateSpotLightsShadowMaps::renderShadowMap(
    const Light *light,
    const SceneRenderState::ShadowCasters &shadowCasters,
    const Matrix4f &lightSpaceMatrix,
    SharedPointer< vulkan::Image > const &shadowMapImage
) noexcept
{
    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    m_renderPass->begin( commandBuffer, m_framebuffers[ currentFrameIndex ] );

    // Set the rendering viewport, but keep in mind that it will be reversed
    // after rendering (because of Vulkan's coordinate system). This sounds
    // counter-intuitive at first, but it makes things easier when applying shadows,
    // since we don't need to transform coordinate (see LocalLightingPass).
    const auto viewport = VkViewport {
        .width = float( m_shadowAttachment.extent.width ),
        .height = float( m_shadowAttachment.extent.height ),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport( commandBuffer, 0, 1, &viewport );

    const auto scissor = VkRect2D {
        .offset = { 0, 0 },
        .extent = m_shadowAttachment.extent,
    };
    vkCmdSetScissor( commandBuffer, 0, 1, &scissor );

    // Set depth bias (aka "Polygon offset")
    // Required to avoid shadow mapping artifacts
    vkCmdSetDepthBias(
        commandBuffer,
        // Constant depth bias factor (always applied)
        1.25f,
        0.0f,
        // Slope depth bias factor, applied depending on polygon's slope
        1.75f
    );

    vkCmdBindPipeline(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getHandle()
    );

    for ( auto &[ primitive, renderables ] : shadowCasters ) {
        for ( const auto &renderable : renderables ) {
            const auto mvp = lightSpaceMatrix * renderable.model;
            vkCmdPushConstants(
                commandBuffer,
                m_pipeline->getPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof( Matrix4f ),
                &mvp
            );

            drawPrimitive( commandBuffer, primitive );
        }
    }

    m_renderPass->end( commandBuffer );

    m_shadowAttachment.images[ currentFrameIndex ]->transitionLayout(
        commandBuffer,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
    );

    auto copyRegion = vulkan::initializers::imageCopy();
    copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    copyRegion.extent = shadowMapImage->getExtent();
    shadowMapImage->copy( commandBuffer, m_shadowAttachment.images[ currentFrameIndex ], copyRegion );

    m_shadowAttachment.images[ currentFrameIndex ]->transitionLayout(
        commandBuffer,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );
}

void GenerateSpotLightsShadowMaps::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto extent = VkExtent2D {
        .width = 2048,
        .height = 2048,
    };

    getRenderDevice()->createFramebufferAttachment( "Scene/Shadow", extent, VK_FORMAT_D32_SFLOAT, m_shadowAttachment );

    m_renderPass = crimild::alloc< vulkan::RenderPass >(
        getRenderDevice(),
        std::vector< const FramebufferAttachment * > {
            &m_shadowAttachment,
        },
        true
    );

    m_framebuffers = vulkan::Framebuffer::createInFlightFramebuffers(
        getRenderDevice(),
        m_renderPass,
        {
            &m_shadowAttachment,
        }
    );

    m_pipeline = [ & ] {
        auto program = crimild::alloc< ShaderProgram >();
        program->setShaders(
            Array< SharedPointer< Shader > > {
                crimild::alloc< Shader >(
                    Shader::Stage::VERTEX,
                    R"(
                        layout ( location = 0 ) in vec3 inPosition;

                        layout( push_constant ) uniform Uniforms {
                            mat4 mvp;
                        };

                        void main()
                        {
                            gl_Position = mvp * vec4( inPosition, 1.0 );
                        }
                    )"
                ),
            }
        );

        const auto viewport = ViewportDimensions { .scalingMode = ScalingMode::DYNAMIC };

        const auto vertexLayouts = std::vector< VertexLayout > {
            VertexLayout::P3_N3_TC2,
        };

        auto pipeline = std::make_unique< GraphicsPipeline >(
            getRenderDevice(),
            *m_renderPass,
            GraphicsPipeline::Descriptor {
                .program = program.get(),
                .vertexLayouts = vertexLayouts,
                .depthStencilState = DepthStencilState {
                    .depthCompareOp = CompareOp::LESS_OR_EQUAL,
                },
                .rasterizationState = RasterizationState {
                    .cullMode = CullMode::FRONT,
                    .depthBiasEnable = true,
                },
                .colorBlendState = ColorBlendState {
                    .enable = false,
                },
                .colorAttachmentCount = 0,
                .dynamicStates = std::vector< VkDynamicState > {
                    VK_DYNAMIC_STATE_VIEWPORT,
                    VK_DYNAMIC_STATE_SCISSOR,
                    VK_DYNAMIC_STATE_DEPTH_BIAS,
                },
                .viewport = viewport,
                .scissor = viewport,
                .pushConstantRanges = std::vector< VkPushConstantRange > {
                    VkPushConstantRange {
                        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                        .offset = 0,
                        .size = sizeof( Matrix4f ),
                    },
                },
            }
        );
        getRenderDevice()->setObjectName( pipeline->getHandle(), "GenerateSpotLightsShadowMapsPass" );
        return pipeline;
    }();
}

void GenerateSpotLightsShadowMaps::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    m_pipeline = nullptr;

    m_framebuffers.clear();
    m_renderPass = nullptr;

    getRenderDevice()->destroyFramebufferAttachment( m_shadowAttachment );
}

void GenerateSpotLightsShadowMaps::drawPrimitive( VkCommandBuffer cmds, const Primitive *primitive ) noexcept
{
    primitive->getVertexData().each(
        [ &, i = 0 ]( auto &vertices ) mutable {
            if ( vertices != nullptr ) {
                VkBuffer buffers[] = { getRenderDevice()->bind( vertices.get() ) };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers( cmds, i, 1, buffers, offsets );
            }
        }
    );

    UInt32 instanceCount = 1;

    if ( instanceCount == 0 ) {
        CRIMILD_LOG_WARNING( "Instance count must be greater than zero. Primitive will not be rendered" );
        return;
    }

    auto indices = primitive->getIndices();
    if ( indices != nullptr ) {
        vkCmdBindIndexBuffer(
            cmds,
            getRenderDevice()->bind( indices ),
            0,
            utils::getIndexType( crimild::get_ptr( indices ) )
        );
        vkCmdDrawIndexed( cmds, indices->getIndexCount(), instanceCount, 0, 0, 0 );
    } else if ( primitive->getVertexData().size() > 0 ) {
        auto vertices = primitive->getVertexData()[ 0 ];
        if ( vertices != nullptr && vertices->getVertexCount() > 0 ) {
            vkCmdDraw( cmds, vertices->getVertexCount(), 1, 0, 0 );
        }
    }
}
