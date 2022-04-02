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

#include "Rendering/RenderPasses/VulkanScenePass.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/Primitive.hpp"
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
#include "Visitors/ApplyToGeometries.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

ScenePass::ScenePass( RenderDevice *renderDevice ) noexcept
    : m_renderDevice( renderDevice ),
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
                                // outColor = color;
                                // outColor = vec4( 0, 1, 1, 1 );
                                if ( outColor.a <= 0.01 ) {
                                    discard;
                                }
                            }
                        )" ) } );
              return program;
          }() )
{
    m_simulation = Simulation::create();
    m_simulation->start();

    init();
}

ScenePass::~ScenePass( void ) noexcept
{
    m_simulation->stop();

    clear();
}

Event ScenePass::handle( const Event &e ) noexcept
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

    return m_simulation->handle( e );
}

void ScenePass::render( void ) noexcept
{
    auto scene = Simulation::getInstance()->getScene();
    if ( scene == nullptr ) {
        return;
    }

    RenderableSet renderables;

    scene->perform(
        ApplyToGeometries(
            [ & ]( Geometry *geometry ) {
                renderables.addGeometry( geometry );
            } ) );

    auto camera = Camera::getMainCamera();
    if ( m_renderPassObjects.uniforms != nullptr ) {
        m_renderPassObjects.uniforms->setValue(
            RenderPassObjects::Uniforms {
                .view = camera->getViewMatrix(),
                .proj = camera->getProjectionMatrix() } );
        m_renderDevice->update( m_renderPassObjects.uniforms.get() );
    }

    const auto currentFrameIndex = m_renderDevice->getCurrentFrameIndex();
    auto commandBuffer = m_renderDevice->getCurrentCommandBuffer();

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

    renderables.eachGeometry(
        [ & ]( Geometry *geometry ) {
            if ( auto ms = geometry->getComponent< MaterialComponent >() ) {
                if ( auto material = ms->first() ) {
                    vkCmdBindPipeline(
                        commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        m_pipeline->getHandle() );

                    vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getPipelineLayout(), 0, 1, &m_renderPassObjects.descriptorSets[ currentFrameIndex ], 0, nullptr );

                    bindMaterialDescriptors( commandBuffer, currentFrameIndex, material );
                    bindGeometryDescriptors( commandBuffer, currentFrameIndex, geometry );
                    drawPrimitive( commandBuffer, currentFrameIndex, geometry->anyPrimitive() );
                }
            }
        } );

    vkCmdEndRenderPass( commandBuffer );
}

void ScenePass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    const auto depthFormat = m_renderDevice->getDepthStencilFormat();
    const auto extent = m_renderDevice->getSwapchainExtent();

    m_colorAttachment.format = colorFormat;
    m_colorAttachment.extent = extent;
    m_depthAttachment.format = depthFormat;
    m_depthAttachment.extent = extent;

    m_renderArea = VkRect2D {
        .offset = {
            0,
            0,
        },
        .extent = extent,
    };

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
            m_renderDevice->getHandle(),
            &createInfo,
            nullptr,
            &m_renderPass ) );

    // Color attachment
    {
        m_renderDevice->createImage(
            extent.width,
            extent.height,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            1,
            0,
            m_colorAttachment.image,
            m_colorAttachment.memory );
        m_renderDevice->createImageView(
            m_colorAttachment.image,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_ASPECT_COLOR_BIT,
            m_colorAttachment.imageView );
        auto samplerInfo = VkSamplerCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .mipLodBias = 0,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = 1,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0,
            .maxLod = 1,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
            .unnormalizedCoordinates = VK_FALSE,
        };
        CRIMILD_VULKAN_CHECK(
            vkCreateSampler(
                m_renderDevice->getHandle(),
                &samplerInfo,
                nullptr,
                &m_colorAttachment.sampler ) );
    }

    // Depth attachment
    {
        m_renderDevice->createImage(
            extent.width,
            extent.height,
            m_renderDevice->getDepthStencilFormat(),
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            1,
            0,
            m_depthAttachment.image,
            m_depthAttachment.memory );
        m_renderDevice->createImageView(
            m_depthAttachment.image,
            m_renderDevice->getDepthStencilFormat(),
            VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
            m_depthAttachment.imageView );
        auto samplerInfo = VkSamplerCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .mipLodBias = 0,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = 1,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0,
            .maxLod = 1,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
            .unnormalizedCoordinates = VK_FALSE,
        };
        CRIMILD_VULKAN_CHECK(
            vkCreateSampler(
                m_renderDevice->getHandle(),
                &samplerInfo,
                nullptr,
                &m_depthAttachment.sampler ) );
    }

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
                m_renderDevice->getHandle(),
                &createInfo,
                nullptr,
                &m_framebuffers[ i ] ) );
    }

    createRenderPassObjects();
    createMaterialObjects();
    createGeometryObjects();

    m_pipeline = std::make_unique< GraphicsPipeline >(
        m_renderDevice,
        m_renderPass,
        std::vector< VkDescriptorSetLayout > {
            m_renderPassObjects.layout,
            m_materialObjects.descriptorSetLayout,
            m_geometryObjects.descriptorSetLayout,
        },
        m_program.get(),
        std::vector< VertexLayout > { VertexLayout::P3_N3_TC2 } );
}

void ScenePass::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( m_renderDevice->getHandle() );

    m_pipeline = nullptr;

    destroyGeometryObjects();
    destroyMaterialObjects();
    destroyRenderPassObjects();

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( m_renderDevice->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    vkDestroySampler( m_renderDevice->getHandle(), m_colorAttachment.sampler, nullptr );
    m_colorAttachment.sampler = VK_NULL_HANDLE;
    vkDestroyImageView( m_renderDevice->getHandle(), m_colorAttachment.imageView, nullptr );
    m_colorAttachment.imageView = VK_NULL_HANDLE;
    vkDestroyImage( m_renderDevice->getHandle(), m_colorAttachment.image, nullptr );
    m_colorAttachment.image = VK_NULL_HANDLE;
    vkFreeMemory( m_renderDevice->getHandle(), m_colorAttachment.memory, nullptr );
    m_colorAttachment.memory = VK_NULL_HANDLE;

    vkDestroySampler( m_renderDevice->getHandle(), m_depthAttachment.sampler, nullptr );
    m_depthAttachment.sampler = VK_NULL_HANDLE;
    vkDestroyImageView( m_renderDevice->getHandle(), m_depthAttachment.imageView, nullptr );
    m_depthAttachment.imageView = VK_NULL_HANDLE;
    vkDestroyImage( m_renderDevice->getHandle(), m_depthAttachment.image, nullptr );
    m_depthAttachment.image = VK_NULL_HANDLE;
    vkFreeMemory( m_renderDevice->getHandle(), m_depthAttachment.memory, nullptr );
    m_depthAttachment.memory = VK_NULL_HANDLE;

    vkDestroyRenderPass( m_renderDevice->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void ScenePass::createRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderPassObjects.uniforms = [ & ] {
        auto ubo = std::make_unique< UniformBuffer >( RenderPassObjects::Uniforms {} );
        ubo->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
        m_renderDevice->bind( ubo.get() );
        return ubo;
    }();

    VkDescriptorPoolSize poolSize {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = uint32_t( m_renderDevice->getSwapchainImageCount() ),
    };

    auto poolCreateInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
        .maxSets = uint32_t( m_renderDevice->getSwapchainImageCount() ),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( m_renderDevice->getHandle(), &poolCreateInfo, nullptr, &m_renderPassObjects.pool ) );

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

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( m_renderDevice->getHandle(), &layoutCreateInfo, nullptr, &m_renderPassObjects.layout ) );

    std::vector< VkDescriptorSetLayout > layouts( m_renderDevice->getSwapchainImageCount(), m_renderPassObjects.layout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_renderPassObjects.pool,
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    m_renderPassObjects.descriptorSets.resize( m_renderDevice->getSwapchainImageCount() );
    CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( m_renderDevice->getHandle(), &allocInfo, m_renderPassObjects.descriptorSets.data() ) );

    for ( size_t i = 0; i < m_renderPassObjects.descriptorSets.size(); ++i ) {
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = m_renderDevice->getHandle( m_renderPassObjects.uniforms.get(), i ),
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

        vkUpdateDescriptorSets( m_renderDevice->getHandle(), 1, &descriptorWrite, 0, nullptr );
    }
}

void ScenePass::destroyRenderPassObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorSetLayout( m_renderDevice->getHandle(), m_renderPassObjects.layout, nullptr );
    m_renderPassObjects.layout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( m_renderDevice->getHandle(), m_renderPassObjects.pool, nullptr );
    m_renderPassObjects.pool = VK_NULL_HANDLE;

    m_renderDevice->unbind( m_renderPassObjects.uniforms.get() );
    m_renderPassObjects.uniforms = nullptr;
}

void ScenePass::createMaterialObjects( void ) noexcept
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

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( m_renderDevice->getHandle(), &layoutCreateInfo, nullptr, &m_materialObjects.descriptorSetLayout ) );
}

void ScenePass::bindMaterialDescriptors( VkCommandBuffer cmds, Index currentFrameIndex, Material *material ) noexcept
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
                .descriptorCount = uint32_t( m_renderDevice->getSwapchainImageCount() ),
            },
            VkDescriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = uint32_t( m_renderDevice->getSwapchainImageCount() ),
            },
        };

        auto poolCreateInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .poolSizeCount = uint32_t( poolSizes.size() ),
            .pPoolSizes = poolSizes.data(),
            .maxSets = uint32_t( m_renderDevice->getSwapchainImageCount() ),
        };

        CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( m_renderDevice->getHandle(), &poolCreateInfo, nullptr, &m_materialObjects.descriptorPools[ material ] ) );

        m_materialObjects.uniforms[ material ] = std::make_unique< UniformBuffer >(
            MaterialUniforms {
                .color = color,
            } );
        m_renderDevice->bind( m_materialObjects.uniforms[ material ].get() );

        std::vector< VkDescriptorSetLayout > layouts( m_renderDevice->getSwapchainImageCount(), m_materialObjects.descriptorSetLayout );

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_materialObjects.descriptorPools[ material ],
            .descriptorSetCount = uint32_t( layouts.size() ),
            .pSetLayouts = layouts.data(),
        };

        m_materialObjects.descriptorSets[ material ].resize( m_renderDevice->getSwapchainImageCount() );
        CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( m_renderDevice->getHandle(), &allocInfo, m_materialObjects.descriptorSets[ material ].data() ) );

        auto imageView = m_renderDevice->bind( texture->imageView.get() );
        auto sampler = m_renderDevice->bind( texture->sampler.get() );

        for ( size_t i = 0; i < m_materialObjects.descriptorSets[ material ].size(); ++i ) {
            const auto bufferInfo = VkDescriptorBufferInfo {
                .buffer = m_renderDevice->getHandle( m_materialObjects.uniforms[ material ].get(), i ),
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
            vkUpdateDescriptorSets( m_renderDevice->getHandle(), writes.size(), writes.data(), 0, nullptr );
        }
    }

    // TODO: This should be handled in a different way. What if texture changes?
    // Also, update only when material changes.
    m_materialObjects.uniforms[ material ]->setValue( color );
    m_renderDevice->update( m_materialObjects.uniforms[ material ].get() );

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

void ScenePass::destroyMaterialObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    // no need to destroy sets
    m_materialObjects.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( m_renderDevice->getHandle(), m_materialObjects.descriptorSetLayout, nullptr );
    m_materialObjects.descriptorSetLayout = VK_NULL_HANDLE;

    for ( auto &it : m_materialObjects.descriptorPools ) {
        vkDestroyDescriptorPool( m_renderDevice->getHandle(), it.second, nullptr );
    }
    m_materialObjects.descriptorPools.clear();

    for ( auto &it : m_materialObjects.uniforms ) {
        m_renderDevice->unbind( it.second.get() );
    }
    m_materialObjects.uniforms.clear();
}

void ScenePass::createGeometryObjects( void ) noexcept
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

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( m_renderDevice->getHandle(), &layoutCreateInfo, nullptr, &m_geometryObjects.descriptorSetLayout ) );
}

void ScenePass::bindGeometryDescriptors( VkCommandBuffer cmds, Index currentFrameIndex, Geometry *geometry ) noexcept
{
    if ( !m_geometryObjects.descriptorSets.contains( geometry ) ) {
        VkDescriptorPoolSize poolSize {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = uint32_t( m_renderDevice->getSwapchainImageCount() ),
        };

        auto poolCreateInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
            .maxSets = uint32_t( m_renderDevice->getSwapchainImageCount() ),
        };

        CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( m_renderDevice->getHandle(), &poolCreateInfo, nullptr, &m_geometryObjects.descriptorPools[ geometry ] ) );

        m_geometryObjects.uniforms[ geometry ] = std::make_unique< UniformBuffer >( Matrix4 {} );
        m_renderDevice->bind( m_geometryObjects.uniforms[ geometry ].get() );

        std::vector< VkDescriptorSetLayout > layouts( m_renderDevice->getSwapchainImageCount(), m_geometryObjects.descriptorSetLayout );

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_geometryObjects.descriptorPools[ geometry ],
            .descriptorSetCount = uint32_t( layouts.size() ),
            .pSetLayouts = layouts.data(),
        };

        m_geometryObjects.descriptorSets[ geometry ].resize( m_renderDevice->getSwapchainImageCount() );
        CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( m_renderDevice->getHandle(), &allocInfo, m_geometryObjects.descriptorSets[ geometry ].data() ) );

        for ( size_t i = 0; i < m_geometryObjects.descriptorSets[ geometry ].size(); ++i ) {
            const auto bufferInfo = VkDescriptorBufferInfo {
                .buffer = m_renderDevice->getHandle( m_geometryObjects.uniforms[ geometry ].get(), i ),
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

            vkUpdateDescriptorSets( m_renderDevice->getHandle(), 1, &descriptorWrite, 0, nullptr );
        }
    }

    m_geometryObjects.uniforms[ geometry ]->setValue( geometry->getWorld().mat );
    m_renderDevice->update( m_geometryObjects.uniforms[ geometry ].get() );

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

void ScenePass::destroyGeometryObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    // no need to destroy sets
    m_geometryObjects.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( m_renderDevice->getHandle(), m_geometryObjects.descriptorSetLayout, nullptr );
    m_geometryObjects.descriptorSetLayout = VK_NULL_HANDLE;

    for ( auto &it : m_geometryObjects.descriptorPools ) {
        vkDestroyDescriptorPool( m_renderDevice->getHandle(), it.second, nullptr );
    }
    m_geometryObjects.descriptorPools.clear();

    for ( auto &it : m_geometryObjects.uniforms ) {
        m_renderDevice->unbind( it.second.get() );
    }
    m_geometryObjects.uniforms.clear();
}

void ScenePass::drawPrimitive( VkCommandBuffer cmds, Index currentFrameIndex, Primitive *primitive ) noexcept
{
    primitive->getVertexData().each(
        [ &, i = 0 ]( auto &vertices ) mutable {
            if ( vertices != nullptr ) {
                VkBuffer buffers[] = { m_renderDevice->bind( vertices.get() ) };
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
            m_renderDevice->bind( indices ),
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
