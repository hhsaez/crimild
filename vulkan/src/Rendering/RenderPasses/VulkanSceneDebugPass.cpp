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

#include "Rendering/RenderPasses/VulkanSceneDebugPass.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Materials/UnlitMaterial.hpp"
#include "Rendering/RenderableSet.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/Apply.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

SceneDebugPass::SceneDebugPass( RenderDevice *renderDevice ) noexcept
    : RenderPassBase( renderDevice ),
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

                            layout ( set = 2, binding = 0 ) uniform NodeUniforms {
                                mat4 model;
                            };

                            layout ( location = 0 ) out vec2 outTexCoord;

                            void main()
                            {
                                gl_Position = proj * view * model * vec4( inPosition, 1.0 );
                                outTexCoord = inTexCoord;
                            }

                        )" ),
                      crimild::alloc< Shader >(
                          Shader::Stage::FRAGMENT,
                          R"(
                            layout( location = 0 ) in vec2 inTexCoord;

                            layout( set = 1, binding = 0 ) uniform MaterialUniform
                            {
                                vec4 color;
                            };

                            layout( set = 1, binding = 1 ) uniform sampler2D uSampler;

                            layout( location = 0 ) out vec4 outColor;

                            void main()
                            {
                                outColor = color * texture( uSampler, inTexCoord );
                                if ( outColor.a <= 0.01 ) {
                                    discard;
                                }
                            }
                        )" ) } );
              return program;
          }() )
{
    m_renderArea = VkRect2D {
        .offset = {
            0,
            0,
        },
        .extent = {
            .width = 1024,
            .height = 1024,
        },
    };

    m_material = [] {
        auto material = std::make_unique< UnlitMaterial >();
        material->setColor( ColorRGBA { 1, 0, 1, 1 } );
        return material;
    }();

    m_pointLightPrimitive = crimild::alloc< SpherePrimitive >(
        SpherePrimitive::Params {
            .type = Primitive::Type::LINES,
        } );

    init();
}

SceneDebugPass::~SceneDebugPass( void ) noexcept
{
    clear();
}

Event SceneDebugPass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            m_renderArea.extent = {
                .width = uint32_t( e.extent.width ),
                .height = uint32_t( e.extent.height ),
            };
            clear();
            init();
            break;
        }

        default:
            break;
    }

    return e;
}

void SceneDebugPass::render( Node *scene, Camera *camera ) noexcept
{
    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    const auto clearValues = std::array< VkClearValue, 2 > {
        VkClearValue {
            .color = {
                .float32 = {
                    0.0f,
                    0.0f,
                    0.0f,
                    0.0f,
                },
            },
        },
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
        .framebuffer = m_framebuffers[ 0 ],
        .renderArea = m_renderArea,
        .clearValueCount = static_cast< crimild::UInt32 >( clearValues.size() ),
        .pClearValues = clearValues.data(),
    };

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    if ( scene != nullptr && camera != nullptr ) {
        std::vector< Node * > lights;
        scene->perform(
            Apply(
                [ & ]( auto node ) {
                    if ( node->getClassName() == Light::__CLASS_NAME ) {
                        lights.push_back( node );
                    }
                } ) );

        // Set correct aspect ratio for camera before rendering
        camera->setAspectRatio( m_renderArea.extent.width / m_renderArea.extent.height );

        if ( m_renderPassObjects.uniforms != nullptr ) {
            m_renderPassObjects.uniforms->setValue(
                RenderPassObjects::Uniforms {
                    .view = camera->getViewMatrix(),
                    .proj = camera->getProjectionMatrix() } );
            getRenderDevice()->update( m_renderPassObjects.uniforms.get() );
        }

        for ( auto node : lights ) {
            vkCmdBindPipeline(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_pipeline->getHandle() );

            vkCmdBindDescriptorSets(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_pipeline->getPipelineLayout(),
                0,
                1,
                &m_renderPassObjects.descriptorSets[ currentFrameIndex ],
                0,
                nullptr );

            bindMaterialDescriptors( commandBuffer, currentFrameIndex, m_material.get() );
            bindNodeDescriptors( commandBuffer, currentFrameIndex, node );
            drawPrimitive( commandBuffer, currentFrameIndex, m_pointLightPrimitive.get() );
        }
    }

    vkCmdEndRenderPass( commandBuffer );
}

void SceneDebugPass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    const auto depthFormat = VK_FORMAT_D32_SFLOAT;

    const auto extent = m_renderArea.extent;

    auto attachments = std::array< VkAttachmentDescription, 2 > {
        VkAttachmentDescription {
            .format = colorFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        },
        VkAttachmentDescription {
            .format = depthFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        }
    };

    auto colorReferences = std::array< VkAttachmentReference, 1 > {
        VkAttachmentReference {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        },
    };

    auto depthStencilReferences = std::array< VkAttachmentReference, 1 > {
        VkAttachmentReference {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        },
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

    createFramebufferAttachment( "Scene/Debug", extent, colorFormat, m_colorAttachment );
    createFramebufferAttachment( "Scene/Debug/Depth", extent, depthFormat, m_depthAttachment );

    // We won't be swapping framebuffers, so create only one
    m_framebuffers.resize( 1 );
    for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
        auto attachments = std::array< VkImageView, 2 > {
            m_colorAttachment.imageView,
            m_depthAttachment.imageView,
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
    createNodeObjects();

    const auto viewport = ViewportDimensions::fromExtent( m_renderArea.extent.width, m_renderArea.extent.height );

    m_pipeline = std::make_unique< GraphicsPipeline >(
        getRenderDevice(),
        m_renderPass,
        GraphicsPipeline::Descriptor {
            .primitiveType = Primitive::Type::LINES,
            .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
                m_renderPassObjects.layout,
                m_materialObjects.descriptorSetLayout,
                m_nodeObjects.descriptorSetLayout,
            },
            .program = m_program.get(),
            .vertexLayouts = std::vector< VertexLayout > { VertexLayout::P3_N3_TC2 },
            .viewport = viewport,
            .scissor = viewport,
        } );
}

void SceneDebugPass::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    m_pipeline = nullptr;

    destroyNodeObjects();
    destroyMaterialObjects();
    destroyRenderPassObjects();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( getRenderDevice()->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    destroyFramebufferAttachment( m_colorAttachment );
    destroyFramebufferAttachment( m_depthAttachment );

    vkDestroyRenderPass( getRenderDevice()->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void SceneDebugPass::createRenderPassObjects( void ) noexcept
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

void SceneDebugPass::destroyRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_renderPassObjects.layout, nullptr );
    m_renderPassObjects.layout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( getRenderDevice()->getHandle(), m_renderPassObjects.pool, nullptr );
    m_renderPassObjects.pool = VK_NULL_HANDLE;

    getRenderDevice()->unbind( m_renderPassObjects.uniforms.get() );
    m_renderPassObjects.uniforms = nullptr;
}

void SceneDebugPass::createMaterialObjects( void ) noexcept
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

void SceneDebugPass::bindMaterialDescriptors( VkCommandBuffer cmds, Index currentFrameIndex, Material *material ) noexcept
{
    auto [ color, texture ] = [ & ]() {
        if ( material == nullptr ) {
            CRIMILD_LOG_WARNING( "Material is null" );
            return std::make_tuple( ColorRGBA { 1, 0, 1, 1 }, crimild::get_ptr( Texture::ONE ) );
        }

        if ( material->getClassName() == materials::PrincipledBSDF::__CLASS_NAME ) {
            const auto bsdf = static_cast< materials::PrincipledBSDF * >( material );
            return std::make_tuple( rgba( bsdf->getAlbedo() ), bsdf->getAlbedoMap() );
        } else {
            const auto unlit = static_cast< UnlitMaterial * >( material );
            return std::make_tuple( unlit->getColor(), unlit->getTexture() );
        }
    }();

    struct MaterialUniforms {
        alignas( 16 ) ColorRGBA color = ColorRGBA { 1, 1, 1, 1 };
    };

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

        m_materialObjects.uniforms[ material ] = std::make_unique< UniformBuffer >(
            MaterialUniforms {
                .color = color,
            } );
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

        auto imageView = getRenderDevice()->bind( texture->imageView.get() );
        auto sampler = getRenderDevice()->bind( texture->sampler.get() );

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
    m_materialObjects.uniforms[ material ]->setValue( color );
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

void SceneDebugPass::destroyMaterialObjects( void ) noexcept
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

void SceneDebugPass::createNodeObjects( void ) noexcept
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

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( getRenderDevice()->getHandle(), &layoutCreateInfo, nullptr, &m_nodeObjects.descriptorSetLayout ) );
}

void SceneDebugPass::bindNodeDescriptors( VkCommandBuffer cmds, Index currentFrameIndex, Node *node ) noexcept
{
    if ( !m_nodeObjects.descriptorSets.contains( node ) ) {
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

        CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( getRenderDevice()->getHandle(), &poolCreateInfo, nullptr, &m_nodeObjects.descriptorPools[ node ] ) );

        m_nodeObjects.uniforms[ node ] = std::make_unique< UniformBuffer >( Matrix4 {} );
        getRenderDevice()->bind( m_nodeObjects.uniforms[ node ].get() );

        std::vector< VkDescriptorSetLayout > layouts( getRenderDevice()->getSwapchainImageCount(), m_nodeObjects.descriptorSetLayout );

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_nodeObjects.descriptorPools[ node ],
            .descriptorSetCount = uint32_t( layouts.size() ),
            .pSetLayouts = layouts.data(),
        };

        m_nodeObjects.descriptorSets[ node ].resize( getRenderDevice()->getSwapchainImageCount() );
        CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( getRenderDevice()->getHandle(), &allocInfo, m_nodeObjects.descriptorSets[ node ].data() ) );

        for ( size_t i = 0; i < m_nodeObjects.descriptorSets[ node ].size(); ++i ) {
            const auto bufferInfo = VkDescriptorBufferInfo {
                .buffer = getRenderDevice()->getHandle( m_nodeObjects.uniforms[ node ].get(), i ),
                .offset = 0,
                .range = m_nodeObjects.uniforms[ node ]->getBufferView()->getLength(),
            };

            const auto descriptorWrite = VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_nodeObjects.descriptorSets[ node ][ i ],
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

    m_nodeObjects.uniforms[ node ]->setValue( node->getWorld().mat );
    getRenderDevice()->update( m_nodeObjects.uniforms[ node ].get() );

    vkCmdBindDescriptorSets(
        cmds,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getPipelineLayout(),
        2,
        1,
        &m_nodeObjects.descriptorSets[ node ][ currentFrameIndex ],
        0,
        nullptr );
}

void SceneDebugPass::destroyNodeObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    // no need to destroy sets
    m_nodeObjects.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), m_nodeObjects.descriptorSetLayout, nullptr );
    m_nodeObjects.descriptorSetLayout = VK_NULL_HANDLE;

    for ( auto &it : m_nodeObjects.descriptorPools ) {
        vkDestroyDescriptorPool( getRenderDevice()->getHandle(), it.second, nullptr );
    }
    m_nodeObjects.descriptorPools.clear();

    for ( auto &it : m_nodeObjects.uniforms ) {
        getRenderDevice()->unbind( it.second.get() );
    }
    m_nodeObjects.uniforms.clear();
}

void SceneDebugPass::drawPrimitive( VkCommandBuffer cmds, Index currentFrameIndex, Primitive *primitive ) noexcept
{
    primitive->getVertexData().each(
        [ &, i = 0 ]( auto &vertices ) mutable {
            if ( vertices != nullptr ) {
                VkBuffer buffers[] = { getRenderDevice()->bind( vertices.get() ) };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers( cmds, i, 1, buffers, offsets );
            }
        } );

    auto indices = primitive->getIndices();
    if ( indices != nullptr ) {
        vkCmdBindIndexBuffer(
            cmds,
            getRenderDevice()->bind( indices ),
            0,
            utils::getIndexType( crimild::get_ptr( indices ) ) );
        vkCmdDrawIndexed( cmds, indices->getIndexCount(), 1, 0, 0, 0 );
    } else if ( primitive->getVertexData().size() > 0 ) {
        auto vertices = primitive->getVertexData()[ 0 ];
        if ( vertices != nullptr && vertices->getVertexCount() > 0 ) {
            vkCmdDraw( cmds, vertices->getVertexCount(), 1, 0, 0 );
        }
    }
}