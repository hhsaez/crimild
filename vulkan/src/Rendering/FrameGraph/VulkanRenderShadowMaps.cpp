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

#include "Rendering/FrameGraph/VulkanRenderShadowMaps.hpp"

#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_lookAt.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "Mathematics/Vector3Ops.hpp"
#include "Mathematics/Vector3_constants.hpp"
#include "Mathematics/Vector4Ops.hpp"
#include "Mathematics/ceil.hpp"
#include "Mathematics/length.hpp"
#include "Mathematics/max.hpp"
#include "Mathematics/ortho.hpp"
#include "Mathematics/perspective.hpp"
#include "Mathematics/swizzle.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanDescriptor.hpp"
#include "Rendering/VulkanDescriptorPool.hpp"
#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanDescriptorSetLayout.hpp"
#include "Rendering/VulkanFramebuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderDeviceCache.hpp"
#include "Rendering/VulkanRenderPass.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "Rendering/VulkanShadowMap.hpp"
#include "SceneGraph/Camera.hpp"

namespace crimild::vulkan::framegraph {

    class RenderDirectionalLightsShadowMaps : public RenderSceneBase {
    public:
        RenderDirectionalLightsShadowMaps( RenderDevice *device )
            : RenderSceneBase(
                device,
                "RenderDirectionalLightShadowMaps",
                VkExtent2D { 2048, 2048 }
            ),
              m_renderTarget(
                  crimild::alloc< RenderTarget >(
                      device,
                      "Scene/Shadows/DirectionalAux",
                      VK_FORMAT_D32_SFLOAT,
                      getExtent()
                  )
              ),
              m_commandBuffer(
                  crimild::alloc< CommandBuffer >(
                      device,
                      getName() + "/CommandBuffer",
                      VK_COMMAND_BUFFER_LEVEL_PRIMARY
                  )
              ),
              m_fallbackShadowMap( crimild::alloc< ShadowMap >( getRenderDevice(), "DirectionalShadowMap", Light::Type::DIRECTIONAL ) )
        {
            auto renderTargets = std::vector< std::shared_ptr< RenderTarget > > { m_renderTarget };

            m_resources.renderPass = crimild::alloc< RenderPass >(
                getRenderDevice(),
                getName() + "/RenderPass",
                renderTargets,
                VK_ATTACHMENT_LOAD_OP_CLEAR
            );

            m_resources.framebuffer = crimild::alloc< Framebuffer >(
                getRenderDevice(),
                getName() + "/Framebuffer",
                getExtent(),
                m_resources.renderPass,
                renderTargets
            );

            m_resources.pipeline = [ & ] {
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
                    m_resources.renderPass->getHandle(),
                    pipelineDescriptor
                );
                getRenderDevice()->setObjectName( pipeline->getHandle(), getName() + "/Pipeline" );
                return pipeline;
            }();
        }

        virtual ~RenderDirectionalLightsShadowMaps( void ) noexcept
        {
            m_resources = {};
        }

        virtual void render(
            const SceneRenderState &renderState,
            const Camera *camera,
            SyncOptions const &options = {}
        ) noexcept override
        {
            m_commandBuffer->reset();
            m_commandBuffer->begin();

            auto cache = getRenderDevice()->getCache();
            const auto &lights = renderState.lights.at( Light::Type::DIRECTIONAL );
            const auto &shadowCasters = renderState.shadowCasters;
            for ( const auto &light : lights ) {
                if ( light->castShadows() ) {
                    if ( !cache->hasShadowMap( light ) ) {
                        std::string name = !light->getName().empty() ? light->getName() + "/ShadowMap" : "ShadowMap";
                        auto shadowMap = crimild::alloc< vulkan::ShadowMap >( getRenderDevice(), name, light->getType() );
                        cache->setShadowMap( light, shadowMap );
                    }

                    if ( auto shadowMap = cache->getShadowMap( light ) ) {
                        auto &shadowMapImage = shadowMap->getImage();

                        // Transition to transfer so we can write into the image after render.
                        m_commandBuffer->transitionImageLayout( shadowMap->getImage().get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

                        for ( uint32_t layerIndex = 0; layerIndex < shadowMap->getLayerCount(); ++layerIndex ) {
                            computeLightSpaceMatrix( camera, light.get(), shadowMap.get(), layerIndex );
                            renderShadowMapImage(
                                light.get(),
                                shadowCasters,
                                shadowMap->getLightSpaceMatrix( layerIndex ),
                                shadowMap->getImage(),
                                layerIndex
                            );
                        }

                        // Transition back to read after render.
                        m_commandBuffer->transitionImageLayout( shadowMap->getImage().get(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL );
                    }
                } else if ( !cache->hasShadowMap( light ) ) {
                    // Set default shadow map for this light
                    // This makes things easier when computing lighting later
                    getRenderDevice()->getCache()->setShadowMap( light, m_fallbackShadowMap );
                }
            }

            m_commandBuffer->end();
            getRenderDevice()->submitGraphicsCommands( m_commandBuffer );
        }

    private:
        void renderShadowMapImage(
            const Light *light,
            const SceneRenderState::ShadowCasters &shadowCasters,
            const Matrix4f &lightSpaceMatrix,
            std::shared_ptr< vulkan::Image > const &shadowMapImage,
            uint32_t layerIndex
        ) noexcept
        {
            m_commandBuffer->beginRenderPass( m_resources.renderPass, m_resources.framebuffer );

            // Set the rendering viewport, but keep in mind that it will be reversed
            // after rendering (because of Vulkan's coordinate system). This sounds
            // counter-intuitive at first, but it makes things easier when applying shadows,
            // since we don't need to transform coordinate (see LocalLightingPass).
            m_commandBuffer->setViewport(
                VkViewport {
                    .width = float( getExtent().width ),
                    .height = float( getExtent().height ),
                    .minDepth = 0.0f,
                    .maxDepth = 1.0f,
                }
            );

            m_commandBuffer->setScissor(
                VkRect2D {
                    .offset = { 0, 0 },
                    .extent = getExtent(),
                }
            );

            // Set depth bias (aka "Polygon offset")
            // Required to avoid shadow mapping artifacts
            m_commandBuffer->setDepthBias(
                // Constant depth bias factor (always applied)
                1.25f,
                0.0f,
                // Slope depth bias factor, applied depending on polygon's slope
                1.75f
            );

            m_commandBuffer->bindPipeline( m_resources.pipeline );

            for ( auto &[ primitive, renderables ] : shadowCasters ) {
                for ( const auto &renderable : renderables ) {
                    const auto mvp = lightSpaceMatrix * renderable.model;
                    m_commandBuffer->pushConstants( VK_SHADER_STAGE_VERTEX_BIT, 0, mvp );
                    m_commandBuffer->drawPrimitive( primitive );
                }
            }

            m_commandBuffer->endRenderPass();

            m_commandBuffer->transitionImageLayout(
                m_renderTarget->getImage().get(),
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
            );
            m_commandBuffer->copy(
                m_renderTarget->getImage().get(),
                shadowMapImage.get(),
                layerIndex
            );
            m_commandBuffer->transitionImageLayout(
                m_renderTarget->getImage().get(),
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            );
        }

        void computeLightSpaceMatrix( const Camera *camera, const Light *light, vulkan::ShadowMap *shadowMap, uint32_t cascadeId )
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
            shadowMap->setSplit( cascadeId, -1.0f * ( nearClip + splitDistance * clipRange ) );
            shadowMap->setLightSpaceMatrix( cascadeId, lightProjectionMatrix * lightViewMatrix );
        }

    private:
        std::shared_ptr< RenderTarget > m_renderTarget;
        std::shared_ptr< CommandBuffer > m_commandBuffer;

        struct Resources {
            std::shared_ptr< RenderPass > renderPass;
            std::shared_ptr< Framebuffer > framebuffer;
            std::shared_ptr< GraphicsPipeline > pipeline;
        } m_resources;

        std::shared_ptr< ShadowMap > m_fallbackShadowMap;
    };

    class RenderPointLightsShadowMaps : public RenderSceneBase {
    public:
        RenderPointLightsShadowMaps( RenderDevice *device )
            : RenderSceneBase(
                device,
                "RenderPointLightShadowMaps",
                VkExtent2D { 1024, 1024 }
            ),
              m_renderTargets(
                  {
                      crimild::alloc< RenderTarget >(
                          device,
                          "Scene/Shadows/Point/ColorAux",
                          VK_FORMAT_R32_SFLOAT,
                          getExtent()
                      ),
                      crimild::alloc< RenderTarget >(
                          device,
                          "Scene/Shadows/Point/Depth/Aux",
                          VK_FORMAT_D32_SFLOAT,
                          getExtent()
                      ),
                  }
              ),
              m_commandBuffer(
                  crimild::alloc< CommandBuffer >(
                      device,
                      getName() + "/CommandBuffer",
                      VK_COMMAND_BUFFER_LEVEL_PRIMARY
                  )
              ),
              m_fallbackShadowMap( crimild::alloc< ShadowMap >( getRenderDevice(), "PointShadowMap", Light::Type::POINT ) )
        {
            m_resources.renderPass = crimild::alloc< RenderPass >(
                getRenderDevice(),
                getName() + "/RenderPass",
                m_renderTargets,
                VK_ATTACHMENT_LOAD_OP_CLEAR
            );

            m_resources.framebuffer = crimild::alloc< Framebuffer >(
                getRenderDevice(),
                getName() + "/Framebuffer",
                getExtent(),
                m_resources.renderPass,
                m_renderTargets
            );

            m_resources.descriptorSetLayout = crimild::alloc< DescriptorSetLayout >(
                getRenderDevice(),
                getName() + "/DescriptorSetLayout",
                std::vector< VkDescriptorSetLayoutBinding > {
                    VkDescriptorSetLayoutBinding {
                        .binding = 0,
                        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .descriptorCount = 1,
                        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                        .pImmutableSamplers = nullptr,
                    },
                }
            );

            m_resources.pipeline = [ & ] {
                auto program = crimild::alloc< ShaderProgram >();
                program->setShaders(
                    Array< SharedPointer< Shader > > {
                        crimild::alloc< Shader >(
                            Shader::Stage::VERTEX,
                            R"(
                                layout ( location = 0 ) in vec3 inPosition;
                            
                                layout ( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                    mat4 lightSpaceMatrix;
                                    vec3 lightPosition;
                                };

                                layout( push_constant ) uniform Uniforms {
                                    mat4 model;
                                };
                            
                                layout ( location = 0 ) out vec3 outLightPosition;
                                layout ( location = 1 ) out vec3 outWorldPosition;

                                void main()
                                {
                                    vec4 worldPosition = model * vec4( inPosition, 1.0 );
                                    gl_Position = lightSpaceMatrix * worldPosition;
                            
                                    outLightPosition = lightPosition;
                                    outWorldPosition = worldPosition.xyz;
                                }
                            )"
                        ),
                        crimild::alloc< Shader >(
                            Shader::Stage::FRAGMENT,
                            R"(
                                layout ( location = 0 ) in vec3 inLightPosition;
                                layout ( location = 1 ) in vec3 inWorldPosition;
                            
                                layout ( location = 0 ) out float outColor;

                                void main()
                                {
                                    outColor = length( inLightPosition - inWorldPosition );
                                }
                            )"
                        ),
                    }
                );

                const auto viewport = ViewportDimensions::fromExtent( getExtent().width, getExtent().height );

                const auto vertexLayouts = std::vector< VertexLayout > {
                    VertexLayout::P3_N3_TC2
                };

                auto pipeline = std::make_unique< GraphicsPipeline >(
                    getRenderDevice(),
                    m_resources.renderPass->getHandle(),
                    GraphicsPipeline::Descriptor {
                        .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
                            m_resources.descriptorSetLayout->getHandle(),
                        },
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
                        .colorAttachmentCount = 1,
                        .viewport = viewport,
                        .scissor = viewport,
                        .pushConstantRanges = {
                            VkPushConstantRange {
                                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                .offset = 0,
                                .size = sizeof( Resources::GeometryData ),
                            },
                        },
                    }
                );
                getRenderDevice()->setObjectName( pipeline->getHandle(), getName() + "/Pipeline" );
                return pipeline;
            }();
        }

        virtual ~RenderPointLightsShadowMaps( void ) noexcept
        {
            m_resources = {};
        }

        virtual void render(
            const SceneRenderState &renderState,
            const Camera *,
            SyncOptions const &options = {}
        ) noexcept override
        {
            m_commandBuffer->reset();
            m_commandBuffer->begin();

            auto cache = getRenderDevice()->getCache();
            const auto &lights = renderState.lights.at( Light::Type::POINT );
            const auto &shadowCasters = renderState.shadowCasters;
            for ( const auto &light : lights ) {
                if ( light->castShadows() ) {
                    if ( !cache->hasShadowMap( light ) ) {
                        std::string name = !light->getName().empty() ? light->getName() + "/ShadowMap" : "ShadowMap";
                        auto shadowMap = crimild::alloc< vulkan::ShadowMap >( getRenderDevice(), name, light->getType() );
                        cache->setShadowMap( light, shadowMap );
                    }

                    if ( auto shadowMap = cache->getShadowMap( light ) ) {
                        auto &shadowMapImage = shadowMap->getImage();

                        // Transition to transfer so we can write into the image after render.
                        m_commandBuffer->transitionImageLayout( shadowMap->getImage().get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

                        for ( uint32_t layerIndex = 0; layerIndex < shadowMap->getLayerCount(); ++layerIndex ) {
                            shadowMap->setLightSpaceMatrix(
                                layerIndex,
                                computeLightSpaceMatrix( light.get(), layerIndex )
                            );
                            renderShadowMapImage(
                                light.get(),
                                shadowCasters,
                                shadowMap->getLightSpaceMatrix( layerIndex ),
                                shadowMap->getImage(),
                                layerIndex
                            );
                        }

                        // Transition back to read after render.
                        m_commandBuffer->transitionImageLayout( shadowMap->getImage().get(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
                    }
                } else if ( !cache->hasShadowMap( light ) ) {
                    // Set default shadow map for this light
                    // This makes things easier when computing lighting later
                    getRenderDevice()->getCache()->setShadowMap( light, m_fallbackShadowMap );
                }
            }

            m_commandBuffer->end();
            getRenderDevice()->submitGraphicsCommands( m_commandBuffer );
        }

    private:
        void renderShadowMapImage(
            const Light *light,
            const SceneRenderState::ShadowCasters &shadowCasters,
            const Matrix4f &lightSpaceMatrix,
            std::shared_ptr< vulkan::Image > const &shadowMapImage,
            uint32_t layerIndex
        ) noexcept
        {
            m_commandBuffer->beginRenderPass( m_resources.renderPass, m_resources.framebuffer );

            // Bind light, creating objects if needed.
            bindLight( light );

            // Update light uniforms for this layer.
            if ( auto uniforms = m_resources.lights[ light ][ layerIndex ].uniforms.get() ) {
                const auto lightPos = location( light->getWorld() );
                uniforms->setValue(
                    Resources::LightData::UniformData {
                        .lightSpaceMatrix = lightSpaceMatrix,
                        .lightPosition = lightPos,
                    }
                );
            }

            m_commandBuffer->bindPipeline( m_resources.pipeline );
            m_commandBuffer->bindDescriptorSet( 0, m_resources.lights[ light ][ layerIndex ].descriptorSet );

            for ( auto &[ primitive, renderables ] : shadowCasters ) {
                for ( const auto &renderable : renderables ) {
                    const auto mvp = lightSpaceMatrix * renderable.model;
                    m_commandBuffer->pushConstants( VK_SHADER_STAGE_VERTEX_BIT, 0, Resources::GeometryData { renderable.model } );
                    m_commandBuffer->drawPrimitive( primitive );
                }
            }

            m_commandBuffer->endRenderPass();

            m_commandBuffer->transitionImageLayout(
                m_renderTargets[ 0 ]->getImage().get(),
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
            );
            m_commandBuffer->copy(
                m_renderTargets[ 0 ]->getImage().get(),
                shadowMapImage.get(),
                layerIndex
            );
            m_commandBuffer->transitionImageLayout(
                m_renderTargets[ 0 ]->getImage().get(),
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            );
        }

        void bindLight( const Light *light ) noexcept
        {
            if ( m_resources.lights.contains( light ) ) {
                return;
            }

            for ( uint32_t layerIndex = 0; layerIndex < 6; ++layerIndex ) {
                m_resources.lights[ light ][ layerIndex ].uniforms = [ & ] {
                    auto ubo = crimild::alloc< UniformBuffer >( Resources::LightData::UniformData {} );
                    ubo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
                    getRenderDevice()->getCache()->bind( ubo );
                    return ubo;
                }();

                auto descriptors = std::vector< Descriptor > {
                    Descriptor {
                        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .buffer = getRenderDevice()->getCache()->bind( m_resources.lights[ light ][ layerIndex ].uniforms ),
                        .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    },
                };
                m_resources.lights[ light ][ layerIndex ].descriptorSet = crimild::alloc< DescriptorSet >(
                    getRenderDevice(),
                    getName() + "/DescriptorSet",
                    crimild::alloc< DescriptorPool >( getRenderDevice(), getName() + "/DescriptorPool", descriptors ),
                    m_resources.descriptorSetLayout,
                    descriptors
                );
            }
        }

        Matrix4f computeLightSpaceMatrix( const Light *light, uint32_t layerIndex ) noexcept
        {
            // Use a perspective projection for light space matrix when using
            // point lights, using the radius as limit.
            const auto pMatrix = perspective( 90, 1, 0.01f, light->getRadius() );

            const auto S = [ layerIndex ]() {
                switch ( layerIndex ) {
                    case 2: // positive y
                        return scale( 1, -1, 1 ).mat;
                    default:
                        return scale( -1, 1, 1 ).mat;
                }
            }();

            const auto lightPos = location( light->getWorld() );
            const auto t = [ lightPos, layerIndex ]() {
                switch ( layerIndex ) {
                    case 0: // positive x
                        return lookAt(
                            lightPos,
                            lightPos + Vector3::Constants::UNIT_X,
                            Vector3::Constants::UP
                        );

                    case 1: // negative x
                        return lookAt(
                            lightPos,
                            lightPos - Vector3::Constants::UNIT_X,
                            Vector3::Constants::UP
                        );

                    case 2: // positive y
                        return lookAt(
                            lightPos,
                            lightPos + Vector3::Constants::UNIT_Y,
                            Vector3::Constants::UNIT_Z
                        );

                    case 3: // negative y
                        return lookAt(
                            lightPos,
                            lightPos - Vector3::Constants::UNIT_Y,
                            Vector3::Constants::UNIT_Z
                        );

                    case 4: // positive z
                        return lookAt(
                            lightPos,
                            lightPos + Vector3::Constants::UNIT_Z,
                            Vector3::Constants::UP
                        );

                    case 5: // negative z
                    default:
                        return lookAt(
                            lightPos,
                            lightPos - Vector3::Constants::UNIT_Z,
                            Vector3::Constants::UP
                        );
                }
            }();

            const auto vMatrix = t.invMat;

            return S * pMatrix * vMatrix;
        }

    private:
        std::vector< std::shared_ptr< RenderTarget > > m_renderTargets;
        std::shared_ptr< CommandBuffer > m_commandBuffer;

        struct Resources {
            std::shared_ptr< RenderPass > renderPass;
            std::shared_ptr< Framebuffer > framebuffer;

            std::shared_ptr< DescriptorSetLayout > descriptorSetLayout;

            struct LightData {
                struct UniformData {
                    alignas( 16 ) Matrix4f lightSpaceMatrix = Matrix4::Constants::IDENTITY;
                    alignas( 16 ) Vector3f lightPosition;
                };
                std::shared_ptr< UniformBuffer > uniforms;
                std::shared_ptr< DescriptorSet > descriptorSet;
            };
            std::unordered_map< const Light *, std::array< LightData, 6 > > lights;

            struct GeometryData {
                alignas( 16 ) Matrix4f model;
            };

            std::shared_ptr< GraphicsPipeline > pipeline;
        } m_resources;

        std::shared_ptr< ShadowMap > m_fallbackShadowMap;
    };

    class RenderSpotLightsShadowMaps : public RenderSceneBase {
    public:
        RenderSpotLightsShadowMaps( RenderDevice *device )
            : RenderSceneBase(
                device,
                "RenderSpotLightsShadowMaps",
                VkExtent2D { 2048, 2048 }
            ),
              m_renderTarget(
                  crimild::alloc< RenderTarget >(
                      device,
                      "Scene/Shadows/Spot",
                      VK_FORMAT_D32_SFLOAT,
                      getExtent()
                  )
              ),
              m_commandBuffer(
                  crimild::alloc< CommandBuffer >(
                      device,
                      getName() + "/CommandBuffer",
                      VK_COMMAND_BUFFER_LEVEL_PRIMARY
                  )
              ),
              m_fallbackShadowMap( crimild::alloc< ShadowMap >( getRenderDevice(), "SpotShadowMaps", Light::Type::SPOT ) )
        {
            auto renderTargets = std::vector< std::shared_ptr< RenderTarget > > { m_renderTarget };

            m_resources.renderPass = crimild::alloc< RenderPass >(
                getRenderDevice(),
                getName() + "/RenderPass",
                renderTargets,
                VK_ATTACHMENT_LOAD_OP_CLEAR
            );

            m_resources.framebuffer = crimild::alloc< Framebuffer >(
                getRenderDevice(),
                getName() + "/Framebuffer",
                getExtent(),
                m_resources.renderPass,
                renderTargets
            );

            m_resources.pipeline = [ & ] {
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
                };
                auto pipeline = std::make_unique< GraphicsPipeline >(
                    getRenderDevice(),
                    m_resources.renderPass->getHandle(),
                    pipelineDescriptor
                );
                getRenderDevice()->setObjectName( pipeline->getHandle(), getName() + "/Pipeline" );
                return pipeline;
            }();
        }

        virtual ~RenderSpotLightsShadowMaps( void ) noexcept
        {
            m_resources = {};
        }

        virtual void render(
            const SceneRenderState &renderState,
            const Camera *camera,
            SyncOptions const &options = {}
        ) noexcept override
        {
            m_commandBuffer->reset();
            m_commandBuffer->begin();

            auto cache = getRenderDevice()->getCache();
            const auto &lights = renderState.lights.at( Light::Type::SPOT );
            const auto &shadowCasters = renderState.shadowCasters;
            for ( const auto &light : lights ) {
                if ( light->castShadows() ) {
                    if ( !cache->hasShadowMap( light ) ) {
                        std::string name = !light->getName().empty() ? light->getName() + "/ShadowMap" : "ShadowMap";
                        auto shadowMap = crimild::alloc< vulkan::ShadowMap >( getRenderDevice(), name, light->getType() );
                        cache->setShadowMap( light, shadowMap );
                    }

                    if ( auto shadowMap = cache->getShadowMap( light ) ) {
                        auto &shadowMapImage = shadowMap->getImage();

                        // Transition to transfer so we can write into the image after render.
                        m_commandBuffer->transitionImageLayout( shadowMap->getImage().get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

                        // Use a perspective projection for light space matrix when using
                        // spot lights, using the radius as limit.
                        shadowMap->setLightSpaceMatrix( 0, perspective( 90, 1, 0.01f, light->getRadius() ) * light->getWorld().invMat );
                        renderShadowMapImage(
                            light.get(),
                            shadowCasters,
                            shadowMap->getLightSpaceMatrix( 0 ),
                            shadowMap->getImage()
                        );

                        // Transition back to read after render.
                        m_commandBuffer->transitionImageLayout( shadowMap->getImage().get(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL );
                    }
                } else if ( !cache->hasShadowMap( light ) ) {
                    // Set default shadow map for this light
                    // This makes things easier when computing lighting later
                    getRenderDevice()->getCache()->setShadowMap( light, m_fallbackShadowMap );
                }
            }

            m_commandBuffer->end();
            getRenderDevice()->submitGraphicsCommands( m_commandBuffer );
        }

    private:
        void renderShadowMapImage(
            const Light *light,
            const SceneRenderState::ShadowCasters &shadowCasters,
            const Matrix4f &lightSpaceMatrix,
            std::shared_ptr< vulkan::Image > const &shadowMapImage
        ) noexcept
        {
            m_commandBuffer->beginRenderPass( m_resources.renderPass, m_resources.framebuffer );

            // Set the rendering viewport, but keep in mind that it will be reversed
            // after rendering (because of Vulkan's coordinate system). This sounds
            // counter-intuitive at first, but it makes things easier when applying shadows,
            // since we don't need to transform coordinate (see LocalLightingPass).
            m_commandBuffer->setViewport(
                VkViewport {
                    .width = float( getExtent().width ),
                    .height = float( getExtent().height ),
                    .minDepth = 0.0f,
                    .maxDepth = 1.0f,
                }
            );

            m_commandBuffer->setScissor(
                VkRect2D {
                    .offset = { 0, 0 },
                    .extent = getExtent(),
                }
            );

            // Set depth bias (aka "Polygon offset")
            // Required to avoid shadow mapping artifacts
            m_commandBuffer->setDepthBias(
                // Constant depth bias factor (always applied)
                1.25f,
                0.0f,
                // Slope depth bias factor, applied depending on polygon's slope
                1.75f
            );

            m_commandBuffer->bindPipeline( m_resources.pipeline );

            for ( auto &[ primitive, renderables ] : shadowCasters ) {
                for ( const auto &renderable : renderables ) {
                    const auto mvp = lightSpaceMatrix * renderable.model;
                    m_commandBuffer->pushConstants( VK_SHADER_STAGE_VERTEX_BIT, 0, mvp );
                    m_commandBuffer->drawPrimitive( primitive );
                }
            }

            m_commandBuffer->endRenderPass();

            m_commandBuffer->transitionImageLayout(
                m_renderTarget->getImage().get(),
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
            );
            m_commandBuffer->copy(
                m_renderTarget->getImage().get(),
                shadowMapImage.get()
            );
            m_commandBuffer->transitionImageLayout(
                m_renderTarget->getImage().get(),
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            );
        }

    private:
        std::shared_ptr< RenderTarget > m_renderTarget;
        std::shared_ptr< CommandBuffer > m_commandBuffer;

        struct Resources {
            std::shared_ptr< RenderPass > renderPass;
            std::shared_ptr< Framebuffer > framebuffer;
            std::shared_ptr< GraphicsPipeline > pipeline;
        } m_resources;

        std::shared_ptr< ShadowMap > m_fallbackShadowMap;
    };

}

using namespace crimild;
using namespace crimild::vulkan;
using namespace crimild::vulkan::framegraph;

RenderShadowMaps::RenderShadowMaps( RenderDevice *device ) noexcept
    : RenderSceneBase( device, "RenderShadowMaps", VkExtent2D {} ),
      m_renderers(
          {
              crimild::alloc< RenderDirectionalLightsShadowMaps >( device ),
              crimild::alloc< RenderPointLightsShadowMaps >( device ),
              crimild::alloc< RenderSpotLightsShadowMaps >( device ),
          }
      )
{
    // no-op
}

void RenderShadowMaps::render(
    const SceneRenderState &renderState,
    const Camera *camera,
    SyncOptions const &options
) noexcept
{
    // Call individually so we can control sync options easier
    // This will make this passes to execute synchronously, which
    // might slow things down. Otherwise, we'll end up with visual artifacts
    // since lighting might be computed before copying shadow maps to
    // their corresponding images.
    // TODO: Improve synchronization so these passes run concurrently.

    m_renderers[ 0 ]->render( 
        renderState, 
        camera, 
        { 
            .pre = options.pre, 
            .wait = options.wait,
            .signal = { m_renderers[ 0 ]->getSemaphore() },
        } 
    );

    m_renderers[ 1 ]->render( 
        renderState, 
        camera,
        {
            .wait = { m_renderers[ 0 ]->getSemaphore() },
            .signal = { m_renderers[ 1 ]->getSemaphore() },
        }
    );

    m_renderers[ 0 ]->render(
        renderState,
        camera,
        { 
            .post = options.post,
            .wait = { m_renderers[ 1 ]->getSemaphore() },
            .signal = options.signal,
        }
    );
}
