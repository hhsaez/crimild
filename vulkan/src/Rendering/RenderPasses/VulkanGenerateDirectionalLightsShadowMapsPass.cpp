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

#include "Rendering/RenderPasses/VulkanGenerateDirectionalLightsShadowMapsPass.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Transformation_lookAt.hpp"
#include "Mathematics/Vector3Ops.hpp"
#include "Mathematics/Vector3_constants.hpp"
#include "Mathematics/ceil.hpp"
#include "Mathematics/length.hpp"
#include "Mathematics/max.hpp"
#include "Mathematics/ortho.hpp"
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

GenerateDirectionalLightsShadowMaps::GenerateDirectionalLightsShadowMaps( RenderDevice *renderDevice ) noexcept
    : RenderPassBase( renderDevice )
{
    init();
}

GenerateDirectionalLightsShadowMaps::~GenerateDirectionalLightsShadowMaps( void ) noexcept
{
    clear();
}

Event GenerateDirectionalLightsShadowMaps::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
            //        case Event::Type::SCENE_CHANGED:
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

static void computeLightSpaceMatrix( const Camera *camera, const Light *light, vulkan::ShadowMap *shadowMap, uint32_t cascadeId )
{
    Array< Real > cascadeSplits( 4 );

    // TODO: get clipping values from camera
    auto nearClip = 0.1f;   // frustum.getDMin();
    auto farClip = 1000.0f; // frustum.getDMax();
    auto clipRange = farClip - nearClip;
    auto minZ = nearClip;
    auto maxZ = nearClip + clipRange;
    auto range = maxZ - minZ;
    auto ratio = maxZ / minZ;

    const auto CASCADE_SPLIT_LAMBDA = 0.95f;

    // Calculate cascade split depths based on view camera frustum
    // This is based on these presentations:
    // https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
    // https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
    // TODO (hernan): This might break if the camera frustum changes...
    for ( auto i = 0; i < 4; ++i ) {
        auto p = float( i + 1 ) / float( 4 );
        auto log = minZ * std::pow( ratio, p );
        auto uniform = minZ + range * p;
        auto d = CASCADE_SPLIT_LAMBDA * ( log - uniform ) + uniform;
        cascadeSplits[ i ] = ( d - nearClip ) / clipRange;
    }

    // auto shadowMap = light->getShadowMap();
    const auto pMatrix = camera->getProjectionMatrix();
    const auto vMatrix = camera->getViewMatrix();
    const auto invViewProj = inverse( pMatrix * vMatrix );

    // Calculate orthographic projections matrices for each cascade
    auto lastSplitDistance = cascadeId > 0 ? cascadeSplits[ cascadeId - 1 ] : 0.0f;
    auto splitDistance = cascadeSplits[ cascadeId ];

    auto frustumCorners = Array< Vector3f > {
        Vector3f { -1.0f, +1.0f, -1.0f },
        Vector3f { +1.0f, +1.0f, -1.0f },
        Vector3f { +1.0f, -1.0f, -1.0f },
        Vector3f { -1.0f, -1.0f, -1.0f },
        Vector3f { -1.0f, +1.0f, +1.0f },
        Vector3f { +1.0f, +1.0f, +1.0f },
        Vector3f { +1.0f, -1.0f, +1.0f },
        Vector3f { -1.0f, -1.0f, +1.0f },
    };

    for ( auto i = 0l; i < 8; ++i ) {
        const auto inversePoint = invViewProj * vector4( frustumCorners[ i ], 1 );
        frustumCorners[ i ] = vector3( inversePoint / inversePoint.w );
    }

    for ( auto i = 0l; i < 4; ++i ) {
        const auto cornerRay = frustumCorners[ i + 4 ] - frustumCorners[ i ];
        const auto nearCornerRay = cornerRay * lastSplitDistance;
        const auto farCornerRay = cornerRay * splitDistance;
        frustumCorners[ i + 4 ] = frustumCorners[ i ] + farCornerRay;
        frustumCorners[ i ] = frustumCorners[ i ] + nearCornerRay;
    }

    auto frustumCenter = Vector3::Constants::ZERO;
    for ( auto i = 0l; i < 8; ++i ) {
        frustumCenter = frustumCenter + frustumCorners[ i ];
    }
    frustumCenter = frustumCenter / 8.0;

    auto far = numbers::NEGATIVE_INFINITY;
    auto near = numbers::POSITIVE_INFINITY;
    auto radius = Real( 0 );

    for ( auto i = 0l; i < 8; ++i ) {
        const auto distance = length( frustumCorners[ i ] - frustumCenter );
        radius = crimild::max( radius, distance );
    }
    radius = crimild::ceil( radius * 16.0f ) / 16.0f;

    const auto maxExtents = Vector3 { radius, radius, radius };
    const auto minExtents = -maxExtents;

    const auto lightDirection = light->getDirection();
    const auto lightViewMatrix = lookAt( Point3 { frustumCenter + lightDirection * minExtents.z }, point3( frustumCenter ), Vector3f::Constants::UP ).invMat;

    // Swap Y-coordinate min/max because of Vulkan's inverted coordinate system...
    const auto lightProjectionMatrix = ortho( minExtents.x, maxExtents.x, maxExtents.y, minExtents.y, 0.0f, maxExtents.z - minExtents.z );

    // store split distances and matrices
    shadowMap->splits[ cascadeId ] = -1.0f * ( nearClip + splitDistance * clipRange );
    shadowMap->lightSpaceMatrices[ cascadeId ] = lightProjectionMatrix * lightViewMatrix;
}

void GenerateDirectionalLightsShadowMaps::render(
    const SceneRenderState::Lights &lights,
    const SceneRenderState::ShadowCasters &shadowCasters,
    const Camera *camera
) noexcept
{
    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    for ( const auto &light : lights.at( Light::Type::DIRECTIONAL ) ) {
        if ( light->castShadows() ) {
            if ( auto shadowMap = getRenderDevice()->getShadowMap( light.get() ) ) {
                auto &shadowMapImage = shadowMap->images[ currentFrameIndex ];

                // Transition to transfer so we can write into the image after render.
                shadowMapImage->transitionLayout( commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

                for ( uint32_t layerIndex = 0; layerIndex < shadowMap->imageLayerCount; ++layerIndex ) {
                    computeLightSpaceMatrix( camera, light.get(), shadowMap, layerIndex );
                    renderShadowMapImage( light.get(), shadowCasters, shadowMap->lightSpaceMatrices[ layerIndex ], shadowMapImage, layerIndex );
                }

                // Transition back to read after render.
                shadowMapImage->transitionLayout( commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
            }
        }
    }
}

void GenerateDirectionalLightsShadowMaps::renderShadowMapImage(
    const Light *light,
    const SceneRenderState::ShadowCasters &shadowCasters,
    const Matrix4f &lightSpaceMatrix,
    SharedPointer< vulkan::Image > const &shadowMapImage,
    uint32_t layerIndex
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

            drawPrimitive( commandBuffer, primitive.get() );
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
    copyRegion.dstSubresource.baseArrayLayer = layerIndex;
    copyRegion.extent = shadowMapImage->getExtent();
    shadowMapImage->copy( commandBuffer, m_shadowAttachment.images[ currentFrameIndex ], copyRegion );

    m_shadowAttachment.images[ currentFrameIndex ]->transitionLayout(
        commandBuffer,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );
}

void GenerateDirectionalLightsShadowMaps::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto extent = VkExtent2D {
        .width = 2048,
        .height = 2048,
    };

    getRenderDevice()->createFramebufferAttachment( "Scene/Shadow", extent, VK_FORMAT_D32_SFLOAT, m_shadowAttachment );

    m_renderPass = crimild::alloc< vulkan::RenderPass >(
        getRenderDevice(),
        std::vector< const FramebufferAttachment * > { &m_shadowAttachment },
        true
    );
    m_renderPass->setName( "GenerateDirectionalLightShadowMaps" );

    m_framebuffers = vulkan::Framebuffer::createInFlightFramebuffers( getRenderDevice(), m_renderPass, { &m_shadowAttachment } );

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
            VertexLayout::P3_N3_TC2
        };

        const auto pipelineDescriptor = GraphicsPipeline::Descriptor {
            .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {},
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
            .pushConstantRanges = {
                VkPushConstantRange {
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof( Matrix4f ),
                },
            },
        };
        auto pipeline = std::make_unique< GraphicsPipeline >(
            getRenderDevice(),
            *m_renderPass,
            pipelineDescriptor
        );
        getRenderDevice()->setObjectName( pipeline->getHandle(), "GenerateDirectionalLightsShadowMapsPass" );
        return pipeline;
    }();
}

void GenerateDirectionalLightsShadowMaps::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    m_pipeline = nullptr;
    m_framebuffers.clear();
    m_renderPass = nullptr;

    getRenderDevice()->destroyFramebufferAttachment( m_shadowAttachment );
}

void GenerateDirectionalLightsShadowMaps::drawPrimitive( VkCommandBuffer cmds, const Primitive *primitive ) noexcept
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
