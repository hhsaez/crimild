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

#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Simulation/Event.hpp"
#include "Simulation/Settings.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

ScenePass::ScenePass( RenderDevice *renderDevice ) noexcept
    : m_renderDevice( renderDevice ),
      m_uniforms( std::make_unique< UniformBuffer >( Vector2 { 1024, 768 } ) )
{
    // m_uniforms->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );

    init();
}

ScenePass::~ScenePass( void ) noexcept
{
    clear();
}

void ScenePass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            clear();

            auto settings = Settings::getInstance();
            auto width = settings->get< float >( "video.width", 1024 );
            auto height = settings->get< float >( "video.height", 768 );
            m_uniforms->setValue( Vector2 { width, height } );

            init();
            break;
        }

        default:
            break;
    }
}

void ScenePass::render( void ) noexcept
{
    const auto currentFrameIndex = m_renderDevice->getCurrentFrameIndex();
    auto commandBuffer = m_renderDevice->getCurrentCommandBuffer();

    beginRenderPass( commandBuffer, currentFrameIndex );

    // m_renderDevice->update( m_uniforms.get() );

    vkCmdBindPipeline(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->getHandle() );

    vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getPipelineLayout(), 0, 1, &m_descriptorSets[ currentFrameIndex ], 0, nullptr );

    vkCmdDraw( commandBuffer, 6, 1, 0, 0 );

    endRenderPass( commandBuffer );
}

void ScenePass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderArea = VkRect2D {
        .offset = {
            0,
            0,
        },
        .extent = m_renderDevice->getSwapchainExtent(),
    };

    auto attachments = std::array< VkAttachmentDescription, 1 > {
        VkAttachmentDescription {
            .format = m_renderDevice->getSwapchainFormat(),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            // Don't clear input. Just load it as it is
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        }
    };

    auto colorReferences = std::array< VkAttachmentReference, 1 > {
        VkAttachmentReference {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
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
        .pDepthStencilAttachment = nullptr,
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

    m_framebuffers.resize( m_renderDevice->getSwapchainImageViews().size() );
    for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
        const auto &imageView = m_renderDevice->getSwapchainImageViews()[ i ];

        auto attachments = std::array< VkImageView, 1 > {
            imageView,
            // TODO: add depth image view if available
        };

        auto createInfo = VkFramebufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .renderPass = m_renderPass,
            .attachmentCount = uint32_t( attachments.size() ),
            .pAttachments = attachments.data(),
            .width = m_renderDevice->getSwapchainExtent().width,
            .height = m_renderDevice->getSwapchainExtent().height,
            .layers = 1,
        };

        CRIMILD_VULKAN_CHECK(
            vkCreateFramebuffer(
                m_renderDevice->getHandle(),
                &createInfo,
                nullptr,
                &m_framebuffers[ i ] ) );
    }

    m_renderDevice->bind( m_uniforms.get() );

    createDescriptorPool();
    createDescriptorSetLayout();
    createDescriptorSets();

    // m_pipeline = std::make_unique< GraphicsPipeline >(
    //     m_renderDevice,
    //     m_renderPass,
    //     std::vector< VkDescriptorSetLayout > { m_descriptorSetLayout },
    //     [ & ] {
    //         auto program = crimild::alloc< ShaderProgram >();
    //         program->setShaders(
    //             Array< SharedPointer< Shader > > {
    //                 crimild::alloc< Shader >(
    //                     Shader::Stage::VERTEX,
    //                     R"(
    //                         vec2 positions[6] = vec2[](
    //                             vec2( -1.0, 1.0 ),
    //                             vec2( -1.0, -1.0 ),
    //                             vec2( 1.0, -1.0 ),

    //                             vec2( -1.0, 1.0 ),
    //                             vec2( 1.0, -1.0 ),
    //                             vec2( 1.0, 1.0 )
    //                         );

    //                         vec2 texCoords[6] = vec2[](
    //                             vec2( 0.0, 1.0 ),
    //                             vec2( 0.0, 0.0 ),
    //                             vec2( 1.0, 0.0 ),

    //                             vec2( 0.0, 1.0 ),
    //                             vec2( 1.0, 0.0 ),
    //                             vec2( 1.0, 1.0 )
    //                         );

    //                         layout ( location = 0 ) out vec2 outTexCoord;

    //                         void main()
    //                         {
    //                             gl_Position = vec4( positions[ gl_VertexIndex ], 0.0, 1.0 );
    //                             outTexCoord = texCoords[ gl_VertexIndex ];
    //                         }
    //                     )" ),
    //                 crimild::alloc< Shader >(
    //                     Shader::Stage::FRAGMENT,
    //                     R"(
    //                         layout( location = 0 ) in vec2 inTexCoord;

    //                         layout ( set = 0, binding = 0 ) uniform Context {
    //                             vec2 dimensions;
    //                         } context;

    //                         layout( location = 0 ) out vec4 outColor;

    //                         float circleMask( vec2 uv, vec2 p, float r, float blur )
    //                         {
    //                             float d = length( uv - p );
    //                             float c = smoothstep( r, r - blur, d );
    //                             return c;
    //                         }

    //                         void main()
    //                         {
    //                             vec2 uv = inTexCoord;
    //                             uv -= 0.5;
    //                             uv.x *= context.dimensions.x / context.dimensions.y;

    //                             float blur = 0.00625;

    //                             float mask = circleMask( uv, vec2( 0.0 ), 0.4, blur );
    //                             mask -= circleMask( uv, vec2( -0.15, 0.1 ), 0.075, blur );
    //                             mask -= circleMask( uv, vec2( 0.15, 0.1 ), 0.075, blur );
    //                             vec3 faceColor = vec3( 1.0, 1.0, 0.0 ) * mask;

    //                             mask = circleMask( uv, vec2( 0.0 ), 0.25, blur );
    //                             mask -= circleMask( uv, vec2( 0.0, 0.05 ), 0.25, blur );
    //                             mask *= uv.y <= 0.0 ? 1.0 : 0.0;
    //                             vec3 mouthColor = vec3( 1.0 ) * mask;

    //                             vec3 color = faceColor - mouthColor;

    //                             outColor = vec4( color, 1.0 );
    //                         }
    //                     )" ) } );
    //         return program;
    //     }() );
}

void ScenePass::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( m_renderDevice->getHandle() );

    m_pipeline = nullptr;

    destroyDescriptorSets();
    destroyDescriptorSetLayout();
    destroyDescriptorPool();

    m_renderDevice->unbind( m_uniforms.get() );

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( m_renderDevice->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    vkDestroyRenderPass( m_renderDevice->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}

void ScenePass::beginRenderPass( VkCommandBuffer commandBuffer, uint8_t currentFrameIndex ) noexcept
{
    auto renderPassInfo = VkRenderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_renderPass,
        .framebuffer = m_framebuffers[ currentFrameIndex ],
        .renderArea = m_renderArea,
        .clearValueCount = 0,
        .pClearValues = nullptr,
    };

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
}

void ScenePass::endRenderPass( VkCommandBuffer commandBuffer ) noexcept
{
    vkCmdEndRenderPass( commandBuffer );
}

void ScenePass::createDescriptorPool( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    VkDescriptorPoolSize poolSize {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = uint32_t( m_renderDevice->getSwapchainImageCount() ),
    };

    auto createInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
        .maxSets = uint32_t( m_renderDevice->getSwapchainImageCount() ),
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorPool( m_renderDevice->getHandle(), &createInfo, nullptr, &m_descriptorPool ) );
}

void ScenePass::destroyDescriptorPool( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorPool( m_renderDevice->getHandle(), m_descriptorPool, nullptr );
    m_descriptorPool = VK_NULL_HANDLE;
}

void ScenePass::createDescriptorSetLayout( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto layoutBinding = VkDescriptorSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = nullptr,
    };

    auto createInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &layoutBinding,
    };

    CRIMILD_VULKAN_CHECK( vkCreateDescriptorSetLayout( m_renderDevice->getHandle(), &createInfo, nullptr, &m_descriptorSetLayout ) );
}

void ScenePass::destroyDescriptorSetLayout( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDestroyDescriptorSetLayout( m_renderDevice->getHandle(), m_descriptorSetLayout, nullptr );
    m_descriptorSetLayout = VK_NULL_HANDLE;
}

void ScenePass::createDescriptorSets( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    std::vector< VkDescriptorSetLayout > layouts( m_renderDevice->getSwapchainImageCount(), m_descriptorSetLayout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_descriptorPool,
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    m_descriptorSets.resize( m_renderDevice->getSwapchainImageCount() );
    CRIMILD_VULKAN_CHECK( vkAllocateDescriptorSets( m_renderDevice->getHandle(), &allocInfo, m_descriptorSets.data() ) );

    for ( size_t i = 0; i < m_descriptorSets.size(); ++i ) {
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = m_renderDevice->getHandle( m_uniforms.get(), i ),
            .offset = 0,
            .range = m_uniforms->getBufferView()->getLength(),
        };

        const auto descriptorWrite = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = m_descriptorSets[ i ],
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

void ScenePass::destroyDescriptorSets( void ) noexcept
{
    CRIMILD_LOG_TRACE();
}