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

#include "Rendering/RenderPasses/VulkanGeneratePointLightsShadowMapsPass.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_lookAt.hpp"
#include "Mathematics/Transformation_rotation.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/Vector4_constants.hpp"
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

GeneratePointLightsShadowMaps::GeneratePointLightsShadowMaps( RenderDevice *renderDevice ) noexcept
    : RenderPassBase( renderDevice )
{
    init();
}

GeneratePointLightsShadowMaps::~GeneratePointLightsShadowMaps( void ) noexcept
{
    clear();
}

Event GeneratePointLightsShadowMaps::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
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

static Matrix4f computeLightSpaceMatrix( const Light *light, uint32_t layerIndex ) noexcept
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

void GeneratePointLightsShadowMaps::render(
    const SceneRenderState::Lights &lights,
    const SceneRenderState::ShadowCasters &shadowCasters,
    const Camera *
) noexcept
{
    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    for ( const auto &light : lights.at( Light::Type::POINT ) ) {
        if ( light->castShadows() ) {
            if ( auto shadowMap = getRenderDevice()->getShadowMap( light.get() ) ) {
                auto &shadowMapImage = shadowMap->images[ currentFrameIndex ];

                // Transition to transfer so we can write into the image after render.
                shadowMapImage->transitionLayout( commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

                for ( uint32_t layerIndex = 0; layerIndex < shadowMap->imageLayerCount; ++layerIndex ) {
                    shadowMap->lightSpaceMatrices[ layerIndex ] = computeLightSpaceMatrix( light.get(), layerIndex );
                    renderShadowMapImage( light.get(), shadowCasters, shadowMap->lightSpaceMatrices[ layerIndex ], shadowMapImage, layerIndex );
                }

                // Transition back to read after render.
                shadowMapImage->transitionLayout( commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
            }
        }
    }
}

void GeneratePointLightsShadowMaps::renderShadowMapImage(
    const Light *light,
    const SceneRenderState::ShadowCasters &shadowCasters,
    const Matrix4f &lightSpaceMatrix,
    SharedPointer< vulkan::Image > &shadowMapImage,
    uint32_t layerIndex
) noexcept
{
    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    m_renderPass->begin( commandBuffer, m_framebuffers[ currentFrameIndex ] );

    // Bind light, creating objects if needed.
    bindLight( light );

    // Update light uniforms for this layer.
    if ( auto uniforms = m_lightObjects.lights[ light ][ layerIndex ].uniforms.get() ) {
        const auto lightPos = location( light->getWorld() );
        uniforms->setValue(
            LightObjects::PerLayer::Uniforms {
                .lightSpaceMatrix = lightSpaceMatrix,
                .lightPosition = lightPos,
            }
        );
        getRenderDevice()->update( uniforms );
    }

    vkCmdBindPipeline(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getHandle()
    );

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getPipelineLayout(),
        0,
        1,
        &m_lightObjects.lights[ light ][ layerIndex ].descriptorSets[ currentFrameIndex ],
        0,
        nullptr
    );

    for ( auto &[ primitive, renderables ] : shadowCasters ) {
        for ( const auto &renderable : renderables ) {
            const auto uniforms = PerGeometryUniforms {
                .model = renderable.model,
            };

            vkCmdPushConstants(
                commandBuffer,
                m_pipeline->getPipelineLayout(),
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof( PerGeometryUniforms ),
                &uniforms
            );

            drawPrimitive( commandBuffer, primitive.get() );
        }
    }

    m_renderPass->end( commandBuffer );

    m_colorAttachment.images[ currentFrameIndex ]->transitionLayout(
        commandBuffer,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
    );

    auto copyRegion = vulkan::initializers::imageCopy();
    copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.dstSubresource.baseArrayLayer = layerIndex;
    copyRegion.extent = shadowMapImage->getExtent();
    shadowMapImage->copy( commandBuffer, m_colorAttachment.images[ currentFrameIndex ], copyRegion );

    m_colorAttachment.images[ currentFrameIndex ]->transitionLayout(
        commandBuffer,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );
}

void GeneratePointLightsShadowMaps::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto extent = VkExtent2D {
        .width = 1024,
        .height = 1024,
    };

    getRenderDevice()->createFramebufferAttachment( "Scene/Shadows/Point/Color", extent, VK_FORMAT_R32_SFLOAT, m_colorAttachment );
    getRenderDevice()->createFramebufferAttachment( "Scene/Shadows/Point/Depth", extent, VK_FORMAT_D32_SFLOAT, m_depthAttachment );

    m_renderPass = crimild::alloc< vulkan::RenderPass >(
        getRenderDevice(),
        std::vector< const FramebufferAttachment * > {
            &m_colorAttachment,
            &m_depthAttachment,
        },
        true
    );

    m_framebuffers = vulkan::Framebuffer::createInFlightFramebuffers(
        getRenderDevice(),
        m_renderPass,
        {
            &m_colorAttachment,
            &m_depthAttachment,
        }
    );

    createLightObjects();

    m_pipeline = [ & ] {
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

        const auto viewport = ViewportDimensions::fromExtent( extent.width, extent.height );

        const auto vertexLayouts = std::vector< VertexLayout > {
            VertexLayout::P3_N3_TC2
        };

        auto pipeline = std::make_unique< GraphicsPipeline >(
            getRenderDevice(),
            *m_renderPass,
            GraphicsPipeline::Descriptor {
                .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
                    m_lightObjects.layout,
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
                        .size = sizeof( PerGeometryUniforms ),
                    },
                },
            }
        );
        getRenderDevice()->setObjectName( pipeline->getHandle(), "GeneratePointLightsShadowMapsPass" );
        return pipeline;
    }();
}

void GeneratePointLightsShadowMaps::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    m_pipeline = nullptr;

    destroyLightObjects();

    m_framebuffers.clear();
    m_renderPass = nullptr;

    getRenderDevice()->destroyFramebufferAttachment( m_colorAttachment );
    getRenderDevice()->destroyFramebufferAttachment( m_depthAttachment );
}

void GeneratePointLightsShadowMaps::createLightObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    auto bindings = std::vector< VkDescriptorSetLayoutBinding > {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = uint32_t( bindings.size() ),
        .pBindings = bindings.data(),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( getRenderDevice()->getHandle(), &layoutCreateInfo, nullptr, &m_lightObjects.layout ) );
}

void GeneratePointLightsShadowMaps::bindLight( const Light *light ) noexcept
{
    if ( m_lightObjects.lights.contains( light ) ) {
        return;
    }

    for ( uint32_t layerIndex = 0; layerIndex < 6; ++layerIndex ) {
        m_lightObjects.lights[ light ][ layerIndex ].uniforms = [ & ] {
            auto ubo = std::make_unique< UniformBuffer >( LightObjects::PerLayer::Uniforms {} );
            ubo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
            getRenderDevice()->bind( ubo.get() );
            return ubo;
        }();

        auto poolSizes = std::vector< VkDescriptorPoolSize > {
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
            },
        };

        auto poolCreateInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
            .poolSizeCount = uint32_t( poolSizes.size() ),
            .pPoolSizes = poolSizes.data(),
        };

        CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_lightObjects.lights[ light ][ layerIndex ].pool ) );

        std::vector< VkDescriptorSetLayout > layouts( getRenderDevice()->getSwapchainImageCount(), m_lightObjects.layout );

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_lightObjects.lights[ light ][ layerIndex ].pool,
            .descriptorSetCount = uint32_t( layouts.size() ),
            .pSetLayouts = layouts.data(),
        };

        m_lightObjects.lights[ light ][ layerIndex ].descriptorSets.resize( getRenderDevice()->getSwapchainImageCount() );
        CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( getRenderDevice()->getHandle(), &allocInfo, m_lightObjects.lights[ light ][ layerIndex ].descriptorSets.data() ) );

        for ( size_t i = 0; i < m_lightObjects.lights[ light ][ layerIndex ].descriptorSets.size(); ++i ) {
            const auto bufferInfo = VkDescriptorBufferInfo {
                .buffer = getRenderDevice()->getHandle( m_lightObjects.lights[ light ][ layerIndex ].uniforms.get(), i ),
                .offset = 0,
                .range = m_lightObjects.lights[ light ][ layerIndex ].uniforms->getBufferView()->getLength(),
            };

            auto writes = std::vector< VkWriteDescriptorSet > {
                VkWriteDescriptorSet {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = m_lightObjects.lights[ light ][ layerIndex ].descriptorSets[ i ],
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .pImageInfo = nullptr,
                    .pBufferInfo = &bufferInfo,
                    .pTexelBufferView = nullptr,
                },
            };

            vkUpdateDescriptorSets(
                getRenderDevice()->getHandle(),
                writes.size(),
                writes.data(),
                0,
                nullptr
            );
        }
    }
}

void GeneratePointLightsShadowMaps::destroyLightObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_lightObjects.layout, nullptr );
    m_lightObjects.layout = VK_NULL_HANDLE;

    for ( auto &it : m_lightObjects.lights ) {
        for ( auto &layer : it.second ) {
            vkDestroyDescriptorPool( getRenderDevice()->getHandle(), layer.pool, nullptr );
            layer.pool = VK_NULL_HANDLE;

            getRenderDevice()->unbind( layer.uniforms.get() );
            layer.uniforms = nullptr;

            layer.descriptorSets.clear();
        }
    }
    m_lightObjects.lights.clear();
}

void GeneratePointLightsShadowMaps::drawPrimitive( VkCommandBuffer cmds, const Primitive *primitive ) noexcept
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
