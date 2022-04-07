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

#include "Rendering/RenderPasses/VulkanGBufferPass.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/RenderableSet.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/ApplyToGeometries.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

GBufferPass::GBufferPass( RenderDevice *renderDevice ) noexcept
    : RenderPass( renderDevice ),
      m_program(
          [ & ] {
              auto program = std::make_unique< ShaderProgram >();
              program->setShaders(
                  Array< SharedPointer< Shader > > {
                      crimild::alloc< Shader >(
                          Shader::Stage::VERTEX,
                          R"(
                            layout ( location = 0 ) in vec3 inPosition;
                            layout ( location = 1 ) in vec3 inNormal;
                            layout ( location = 2 ) in vec2 inTexCoord;

                            layout ( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                mat4 view;
                                mat4 proj;
                            };

                            layout ( set = 2, binding = 0 ) uniform GeometryUniforms {
                                mat4 model;
                            };

                            layout ( location = 0 ) out vec3 outPosition;
                            layout ( location = 1 ) out vec3 outNormal;
                            layout ( location = 2 ) out vec2 outTexCoord;

                            void main()
                            {
                                gl_Position = proj * view * model * vec4( inPosition, 1.0 );

                                outPosition = ( model * vec4( inPosition, 1.0 ) ).xyz;
                                outNormal = inverse( transpose( mat3( model ) ) ) * inNormal;
                                outTexCoord = inTexCoord;
                            }

                        )" ),
                      crimild::alloc< Shader >(
                          Shader::Stage::FRAGMENT,
                          R"(
                            layout ( location = 0 ) in vec3 inPosition;
                            layout ( location = 1 ) in vec3 inNormal;
                            layout ( location = 2 ) in vec2 inTexCoord;

                            layout( set = 1, binding = 0 ) uniform MaterialUniform
                            {
                                vec3 albedo;
                                float metallic;
                                float roughness;
                                float ambientOcclusion;
                                float transmission;
                                float indexOfRefraction;
                                vec3 emissive;
                            } uMaterial;

                            layout( set = 1, binding = 1 ) uniform sampler2D uAlbedoMap;

                            layout ( location = 0 ) out vec4 outAlbedo;
                            layout ( location = 1 ) out vec4 outPosition;
                            layout ( location = 2 ) out vec4 outNormal;
                            layout ( location = 3 ) out vec4 outMaterial;

                            void main()
                            {
                                vec3 albedo = uMaterial.albedo * pow( texture( uAlbedoMap, inTexCoord ).rgb, vec3( 2.2 ) );
                                float metallic = uMaterial.metallic;// * texture( uMetallicMap, inTexCoord ).r;
                                float roughness = uMaterial.roughness;// * texture( uRoughnessMap, inTexCoord ).r;
                                float ambientOcclusion = uMaterial.ambientOcclusion;// * texture( uAmbientOcclusionMap, inTexCoord ).r;

                                metallic = clamp( metallic, 0.0, 1.0 );
                                roughness = clamp( roughness, 0.05, 0.999 );

                                outAlbedo = vec4( albedo, 1.0 );
                                outPosition = vec4( inPosition, 1.0 );
                                outNormal = vec4( inNormal, 1.0 );
                                outMaterial = vec4( metallic, roughness, ambientOcclusion, 1.0 );
                            }
                        )" ) } );
              return program;
          }() )
{
    init();
}

GBufferPass::~GBufferPass( void ) noexcept
{
    clear();
}

Event GBufferPass::handle( const Event &e ) noexcept
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

void GBufferPass::render( void ) noexcept
{
    auto scene = Simulation::getInstance()->getScene();
    if ( scene == nullptr ) {
        return;
    }

    RenderableSet renderables;

    scene->perform(
        ApplyToGeometries(
            [ & ]( Geometry *geometry ) {
                if ( auto ms = geometry->getComponent< MaterialComponent >() ) {
                    if ( auto m = ms->first() ) {
                        if ( m->getClassName() == materials::PrincipledBSDF::__CLASS_NAME ) {
                            renderables.addGeometry( geometry );
                        }
                    }
                }
            } ) );

    auto camera = Camera::getMainCamera();
    if ( m_renderPassObjects.uniforms != nullptr ) {
        m_renderPassObjects.uniforms->setValue(
            RenderPassObjects::Uniforms {
                .view = camera->getViewMatrix(),
                .proj = camera->getProjectionMatrix() } );
        getRenderDevice()->update( m_renderPassObjects.uniforms.get() );
    }

    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    const auto clearColorValue = VkClearValue {
        .color = {
            .float32 = {
                0.0f,
                0.0f,
                0.0f,
                0.0f,
            },
        },
    };

    const auto clearValues = std::array< VkClearValue, 5 > {
        clearColorValue,
        clearColorValue,
        clearColorValue,
        clearColorValue,
        VkClearValue {
            .depthStencil = {
                1,
                0,
            },
        },
    };

    auto renderPassInfo = VkRenderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_renderPass,
        .framebuffer = m_framebuffers[ currentFrameIndex ],
        .renderArea = m_renderArea,
        .clearValueCount = clearValues.size(),
        .pClearValues = clearValues.data(),
    };

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    renderables.eachGeometry(
        [ & ]( Geometry *geometry ) {
            if ( auto ms = geometry->getComponent< MaterialComponent >() ) {
                if ( auto material = ms->first() ) {
                    vkCmdBindPipeline(
                        commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        m_pipeline->getHandle() );

                    vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getPipelineLayout(), 0, 1, &m_renderPassObjects.descriptorSets[ currentFrameIndex ], 0, nullptr );

                    bindMaterialDescriptors( commandBuffer, currentFrameIndex, static_cast< materials::PrincipledBSDF * >( material ) );
                    bindGeometryDescriptors( commandBuffer, currentFrameIndex, geometry );
                    drawPrimitive( commandBuffer, currentFrameIndex, geometry->anyPrimitive() );
                }
            }
        } );

    vkCmdEndRenderPass( commandBuffer );
}

void GBufferPass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto extent = getRenderDevice()->getSwapchainExtent();

    m_renderArea = VkRect2D {
        .offset = {
            0,
            0,
        },
        .extent = extent,
    };

    createFramebufferAttachment( "Scene/Albedo", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_albedoAttachment );
    createFramebufferAttachment( "Scene/Position", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_positionAttachment );
    createFramebufferAttachment( "Scene/Normal", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_normalAttachment );
    createFramebufferAttachment( "Scene/Material", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_materialAttachment );
    createFramebufferAttachment( "Scene/Depth", extent, getRenderDevice()->getDepthStencilFormat(), m_depthStencilAttachment );

    auto getAttachmentDescription = [ & ]( const auto &att ) {
        return VkAttachmentDescription {
            .format = att.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = getRenderDevice()->formatIsColor( att.format )
                               ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                               : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };
    };

    auto attachments = std::array< VkAttachmentDescription, 5 > {
        getAttachmentDescription( m_albedoAttachment ),
        getAttachmentDescription( m_positionAttachment ),
        getAttachmentDescription( m_normalAttachment ),
        getAttachmentDescription( m_materialAttachment ),
        getAttachmentDescription( m_depthStencilAttachment ),
    };

    auto getAttachmentReference = [ & ]( const auto &att, uint32_t idx ) {
        return VkAttachmentReference {
            .attachment = idx,
            .layout = getRenderDevice()->formatIsColor( att.format )
                          ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                          : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };
    };

    auto colorReferences = std::array< VkAttachmentReference, 4 > {
        getAttachmentReference( m_albedoAttachment, 0 ),
        getAttachmentReference( m_positionAttachment, 1 ),
        getAttachmentReference( m_normalAttachment, 2 ),
        getAttachmentReference( m_materialAttachment, 3 ),
    };

    auto depthStencilReferences = std::array< VkAttachmentReference, 1 > {
        getAttachmentReference( m_depthStencilAttachment, 4 ),
    };

    auto subpass = VkSubpassDescription {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = crimild::UInt32( colorReferences.size() ),
        .pColorAttachments = colorReferences.data(),
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = depthStencilReferences.data(),
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    auto dependencies = std::array< VkSubpassDependency, 2 > {
        VkSubpassDependency {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        },
        VkSubpassDependency {
            .srcSubpass = 0,
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        }
    };

    auto createInfo = VkRenderPassCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = static_cast< crimild::UInt32 >( attachments.size() ),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = crimild::UInt32( dependencies.size() ),
        .pDependencies = dependencies.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateRenderPass(
            getRenderDevice()->getHandle(),
            &createInfo,
            nullptr,
            &m_renderPass ) );

    m_framebuffers.resize( getRenderDevice()->getSwapchainImageViews().size() );
    for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
        auto attachments = std::array< VkImageView, 5 > {
            m_albedoAttachment.imageView,
            m_positionAttachment.imageView,
            m_normalAttachment.imageView,
            m_materialAttachment.imageView,
            m_depthStencilAttachment.imageView,
        };

        auto createInfo = VkFramebufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .renderPass = m_renderPass,
            .attachmentCount = uint32_t( attachments.size() ),
            .pAttachments = attachments.data(),
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        };

        CRIMILD_VULKAN_CHECK(
            vkCreateFramebuffer(
                getRenderDevice()->getHandle(),
                &createInfo,
                nullptr,
                &m_framebuffers[ i ] ) );
    }

    createRenderPassObjects();
    createMaterialObjects();
    createGeometryObjects();

    m_pipeline = std::make_unique< GraphicsPipeline >(
        getRenderDevice(),
        m_renderPass,
        std::vector< VkDescriptorSetLayout > {
            m_renderPassObjects.layout,
            m_materialObjects.descriptorSetLayout,
            m_geometryObjects.descriptorSetLayout,
        },
        m_program.get(),
        std::vector< VertexLayout > { VertexLayout::P3_N3_TC2 },
        DepthStencilState {},
        RasterizationState {},
        ColorBlendState {},
        colorReferences.size() );
}

void GBufferPass::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    m_pipeline = nullptr;

    destroyGeometryObjects();
    destroyMaterialObjects();
    destroyRenderPassObjects();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( getRenderDevice()->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    destroyFramebufferAttachment( m_albedoAttachment );
    destroyFramebufferAttachment( m_positionAttachment );
    destroyFramebufferAttachment( m_normalAttachment );
    destroyFramebufferAttachment( m_materialAttachment );
    destroyFramebufferAttachment( m_depthStencilAttachment );

    vkDestroyRenderPass( getRenderDevice()->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void GBufferPass::createRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderPassObjects.uniforms = [ & ] {
        auto ubo = std::make_unique< UniformBuffer >( RenderPassObjects::Uniforms {} );
        ubo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        getRenderDevice()->bind( ubo.get() );
        return ubo;
    }();

    VkDescriptorPoolSize poolSize {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
    };

    auto poolCreateInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
        .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_renderPassObjects.pool ) );

    const auto layoutBinding = VkDescriptorSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr,
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &layoutBinding,
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( getRenderDevice()->getHandle(), &layoutCreateInfo, nullptr, &m_renderPassObjects.layout ) );

    std::vector< VkDescriptorSetLayout > layouts( getRenderDevice()->getSwapchainImageCount(), m_renderPassObjects.layout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_renderPassObjects.pool,
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    m_renderPassObjects.descriptorSets.resize( getRenderDevice()->getSwapchainImageCount() );
    CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( getRenderDevice()->getHandle(), &allocInfo, m_renderPassObjects.descriptorSets.data() ) );

    for ( size_t i = 0; i < m_renderPassObjects.descriptorSets.size(); ++i ) {
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = getRenderDevice()->getHandle( m_renderPassObjects.uniforms.get(), i ),
            .offset = 0,
            .range = m_renderPassObjects.uniforms->getBufferView()->getLength(),
        };

        const auto descriptorWrite = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = m_renderPassObjects.descriptorSets[ i ],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .pBufferInfo = &bufferInfo,
            .pImageInfo = nullptr,
            .pTexelBufferView = nullptr,
        };

        vkUpdateDescriptorSets( getRenderDevice()->getHandle(), 1, &descriptorWrite, 0, nullptr );
    }
}

void GBufferPass::destroyRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_renderPassObjects.layout, nullptr );
    m_renderPassObjects.layout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( getRenderDevice()->getHandle(), m_renderPassObjects.pool, nullptr );
    m_renderPassObjects.pool = VK_NULL_HANDLE;

    getRenderDevice()->unbind( m_renderPassObjects.uniforms.get() );
    m_renderPassObjects.uniforms = nullptr;
}

void GBufferPass::createMaterialObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto bindings = std::array< VkDescriptorSetLayoutBinding, 2 > {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
        VkDescriptorSetLayoutBinding {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = uint32_t( bindings.size() ),
        .pBindings = bindings.data(),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( getRenderDevice()->getHandle(), &layoutCreateInfo, nullptr, &m_materialObjects.descriptorSetLayout ) );
}

void GBufferPass::bindMaterialDescriptors( VkCommandBuffer cmds, Index currentFrameIndex, materials::PrincipledBSDF *material ) noexcept
{
    if ( material == nullptr ) {
        CRIMILD_LOG_WARNING( "Material is null" );
        return;
    }

    if ( !m_materialObjects.descriptorSets.contains( material ) ) {
        const auto poolSizes = std::array< VkDescriptorPoolSize, 2 > {
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
            },
        };

        auto poolCreateInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .poolSizeCount = uint32_t( poolSizes.size() ),
            .pPoolSizes = poolSizes.data(),
            .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        };

        CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_materialObjects.descriptorPools[ material ] ) );

        m_materialObjects.uniforms[ material ] = std::make_unique< UniformBuffer >( materials::PrincipledBSDF::Props {} );
        getRenderDevice()->bind( m_materialObjects.uniforms[ material ].get() );

        std::vector< VkDescriptorSetLayout > layouts( getRenderDevice()->getSwapchainImageCount(), m_materialObjects.descriptorSetLayout );

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_materialObjects.descriptorPools[ material ],
            .descriptorSetCount = uint32_t( layouts.size() ),
            .pSetLayouts = layouts.data(),
        };

        m_materialObjects.descriptorSets[ material ].resize( getRenderDevice()->getSwapchainImageCount() );
        CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( getRenderDevice()->getHandle(), &allocInfo, m_materialObjects.descriptorSets[ material ].data() ) );

        auto imageView = getRenderDevice()->bind( material->getAlbedoMap()->imageView.get() );
        auto sampler = getRenderDevice()->bind( material->getAlbedoMap()->sampler.get() );

        for ( size_t i = 0; i < m_materialObjects.descriptorSets[ material ].size(); ++i ) {
            const auto bufferInfo = VkDescriptorBufferInfo {
                .buffer = getRenderDevice()->getHandle( m_materialObjects.uniforms[ material ].get(), i ),
                .offset = 0,
                .range = m_materialObjects.uniforms[ material ]->getBufferView()->getLength(),
            };

            const auto imageInfo = VkDescriptorImageInfo {
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .imageView = imageView,
                .sampler = sampler,
            };

            const auto writes = std::array< VkWriteDescriptorSet, 2 > {
                VkWriteDescriptorSet {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = m_materialObjects.descriptorSets[ material ][ i ],
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = 1,
                    .pBufferInfo = &bufferInfo,
                    .pImageInfo = nullptr,
                    .pTexelBufferView = nullptr,
                },
                VkWriteDescriptorSet {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = m_materialObjects.descriptorSets[ material ][ i ],
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = 1,
                    .pBufferInfo = nullptr,
                    .pImageInfo = &imageInfo,
                    .pTexelBufferView = nullptr,
                },
            };
            vkUpdateDescriptorSets( getRenderDevice()->getHandle(), writes.size(), writes.data(), 0, nullptr );
        }
    }

    // TODO: This should be handled in a different way. What if texture changes?
    // Also, update only when material changes.
    m_materialObjects.uniforms[ material ]->setValue( material->getProps() );
    getRenderDevice()->update( m_materialObjects.uniforms[ material ].get() );

    vkCmdBindDescriptorSets(
        cmds,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getPipelineLayout(),
        1,
        1,
        &m_materialObjects.descriptorSets[ material ][ currentFrameIndex ],
        0,
        nullptr );
}

void GBufferPass::destroyMaterialObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    // no need to destroy sets
    m_materialObjects.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_materialObjects.descriptorSetLayout, nullptr );
    m_materialObjects.descriptorSetLayout = VK_NULL_HANDLE;

    for ( auto &it : m_materialObjects.descriptorPools ) {
        vkDestroyDescriptorPool( getRenderDevice()->getHandle(), it.second, nullptr );
    }
    m_materialObjects.descriptorPools.clear();

    for ( auto &it : m_materialObjects.uniforms ) {
        getRenderDevice()->unbind( it.second.get() );
    }
    m_materialObjects.uniforms.clear();
}

void GBufferPass::createGeometryObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto layoutBinding = VkDescriptorSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr,
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &layoutBinding,
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( getRenderDevice()->getHandle(), &layoutCreateInfo, nullptr, &m_geometryObjects.descriptorSetLayout ) );
}

void GBufferPass::bindGeometryDescriptors( VkCommandBuffer cmds, Index currentFrameIndex, Geometry *geometry ) noexcept
{
    if ( !m_geometryObjects.descriptorSets.contains( geometry ) ) {
        VkDescriptorPoolSize poolSize {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        };

        auto poolCreateInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
            .maxSets = uint32_t( getRenderDevice()->getSwapchainImageCount() ),
        };

        CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_geometryObjects.descriptorPools[ geometry ] ) );

        m_geometryObjects.uniforms[ geometry ] = std::make_unique< UniformBuffer >( Matrix4 {} );
        getRenderDevice()->bind( m_geometryObjects.uniforms[ geometry ].get() );

        std::vector< VkDescriptorSetLayout > layouts( getRenderDevice()->getSwapchainImageCount(), m_geometryObjects.descriptorSetLayout );

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_geometryObjects.descriptorPools[ geometry ],
            .descriptorSetCount = uint32_t( layouts.size() ),
            .pSetLayouts = layouts.data(),
        };

        m_geometryObjects.descriptorSets[ geometry ].resize( getRenderDevice()->getSwapchainImageCount() );
        CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( getRenderDevice()->getHandle(), &allocInfo, m_geometryObjects.descriptorSets[ geometry ].data() ) );

        for ( size_t i = 0; i < m_geometryObjects.descriptorSets[ geometry ].size(); ++i ) {
            const auto bufferInfo = VkDescriptorBufferInfo {
                .buffer = getRenderDevice()->getHandle( m_geometryObjects.uniforms[ geometry ].get(), i ),
                .offset = 0,
                .range = m_geometryObjects.uniforms[ geometry ]->getBufferView()->getLength(),
            };

            const auto descriptorWrite = VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_geometryObjects.descriptorSets[ geometry ][ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &bufferInfo,
                .pImageInfo = nullptr,
                .pTexelBufferView = nullptr,
            };

            vkUpdateDescriptorSets( getRenderDevice()->getHandle(), 1, &descriptorWrite, 0, nullptr );
        }
    }

    m_geometryObjects.uniforms[ geometry ]->setValue( geometry->getWorld().mat );
    getRenderDevice()->update( m_geometryObjects.uniforms[ geometry ].get() );

    vkCmdBindDescriptorSets(
        cmds,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getPipelineLayout(),
        2,
        1,
        &m_geometryObjects.descriptorSets[ geometry ][ currentFrameIndex ],
        0,
        nullptr );
}

void GBufferPass::destroyGeometryObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    // no need to destroy sets
    m_geometryObjects.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_geometryObjects.descriptorSetLayout, nullptr );
    m_geometryObjects.descriptorSetLayout = VK_NULL_HANDLE;

    for ( auto &it : m_geometryObjects.descriptorPools ) {
        vkDestroyDescriptorPool( getRenderDevice()->getHandle(), it.second, nullptr );
    }
    m_geometryObjects.descriptorPools.clear();

    for ( auto &it : m_geometryObjects.uniforms ) {
        getRenderDevice()->unbind( it.second.get() );
    }
    m_geometryObjects.uniforms.clear();
}

void GBufferPass::drawPrimitive( VkCommandBuffer cmds, Index currentFrameIndex, Primitive *primitive ) noexcept
{
    primitive->getVertexData().each(
        [ &, i = 0 ]( auto &vertices ) mutable {
            if ( vertices != nullptr ) {
                VkBuffer buffers[] = { getRenderDevice()->bind( vertices.get() ) };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers( cmds, i, 1, buffers, offsets );
            }
        } );

    UInt32 instanceCount = 1;
    // if ( instanceData != nullptr ) {
    //     instanceCount = UInt32( instanceData->getVertexCount() );
    //     bindVertexBuffer( get_ptr( instanceData ), vboIndex++ );
    // }

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
            utils::getIndexType( crimild::get_ptr( indices ) ) );
        vkCmdDrawIndexed( cmds, indices->getIndexCount(), instanceCount, 0, 0, 0 );
    } else if ( primitive->getVertexData().size() > 0 ) {
        auto vertices = primitive->getVertexData()[ 0 ];
        if ( vertices != nullptr && vertices->getVertexCount() > 0 ) {
            vkCmdDraw( cmds, vertices->getVertexCount(), 1, 0, 0 );
        }
    }
}
