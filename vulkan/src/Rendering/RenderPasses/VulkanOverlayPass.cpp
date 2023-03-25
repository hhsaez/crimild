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

#include "Rendering/RenderPasses/VulkanOverlayPass.hpp"

#include "Rendering/ShaderProgram.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Simulation/Event.hpp"
#include "Simulation/Settings.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

OverlayPass::OverlayPass( RenderDevice *renderDevice, std::string name, const std::vector< const FramebufferAttachment * > &inputs ) noexcept
    : RenderPassBase( renderDevice ),
      NamedObject( name ),
      m_inputs( inputs )
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

    init();
}

OverlayPass::~OverlayPass( void ) noexcept
{
    clear();
}

Event OverlayPass::handle( const Event &e ) noexcept
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

void OverlayPass::render( void ) noexcept
{
    // const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    // auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    // const auto clearValues = std::array< VkClearValue, 1 > {
    //     VkClearValue {
    //         .color = {
    //             .float32 = {
    //                 0.0f,
    //                 1.0f,
    //                 0.0f,
    //                 // Set alpha to 1 since the resulting image should not be transparent.
    //                 1.0f,
    //             },
    //         },
    //     },
    // };

    // auto renderPassInfo = VkRenderPassBeginInfo {
    //     .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    //     .renderPass = m_renderPass,
    //     .framebuffer = m_framebuffers[ currentFrameIndex ],
    //     .renderArea = m_renderArea,
    //     .clearValueCount = clearValues.size(),
    //     .pClearValues = clearValues.data(),
    // };

    // vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    // vkCmdBindPipeline(
    //     commandBuffer,
    //     VK_PIPELINE_BIND_POINT_GRAPHICS,
    //     m_pipeline->getHandle()
    // );

    // for ( const auto &att : m_inputs ) {
    //     vkCmdBindDescriptorSets(
    //         commandBuffer,
    //         VK_PIPELINE_BIND_POINT_GRAPHICS,
    //         m_pipeline->getPipelineLayout(),
    //         0,
    //         1,
    //         &att->descriptorSets[ currentFrameIndex ],
    //         0,
    //         nullptr
    //     );
    //     vkCmdDraw( commandBuffer, 6, 1, 0, 0 );
    // }

    // vkCmdEndRenderPass( commandBuffer );
}

void OverlayPass::init( void ) noexcept
{
    // CRIMILD_LOG_TRACE();

    // const auto extent = m_renderArea.extent;

    // getRenderDevice()->createFramebufferAttachment( getName(), extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_colorAttachment );

    // auto attachments = std::array< VkAttachmentDescription, 1 > {
    //     VkAttachmentDescription {
    //         .format = m_colorAttachment.format,
    //         .samples = VK_SAMPLE_COUNT_1_BIT,
    //         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    //         .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
    //         .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    //         .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    //         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    //         .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    //     }
    // };

    // auto colorReferences = std::array< VkAttachmentReference, 1 > {
    //     VkAttachmentReference {
    //         .attachment = 0,
    //         .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    //     },
    // };

    // auto subpass = VkSubpassDescription {
    //     .flags = 0,
    //     .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    //     .inputAttachmentCount = 0,
    //     .pInputAttachments = nullptr,
    //     .colorAttachmentCount = crimild::UInt32( colorReferences.size() ),
    //     .pColorAttachments = colorReferences.data(),
    //     .pResolveAttachments = nullptr,
    //     .pDepthStencilAttachment = nullptr,
    //     .preserveAttachmentCount = 0,
    //     .pPreserveAttachments = nullptr,
    // };

    // auto dependencies = std::array< VkSubpassDependency, 2 > {
    //     VkSubpassDependency {
    //         .srcSubpass = VK_SUBPASS_EXTERNAL,
    //         .dstSubpass = 0,
    //         .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    //         .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    //         .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
    //         .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    //         .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
    //     },
    //     VkSubpassDependency {
    //         .srcSubpass = 0,
    //         .dstSubpass = VK_SUBPASS_EXTERNAL,
    //         .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    //         .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    //         .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    //         .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
    //         .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
    //     }
    // };

    // auto createInfo = VkRenderPassCreateInfo {
    //     .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    //     .attachmentCount = static_cast< crimild::UInt32 >( attachments.size() ),
    //     .pAttachments = attachments.data(),
    //     .subpassCount = 1,
    //     .pSubpasses = &subpass,
    //     .dependencyCount = crimild::UInt32( dependencies.size() ),
    //     .pDependencies = dependencies.data(),
    // };

    // CRIMILD_VULKAN_CHECK(
    //     vkCreateRenderPass(
    //         getRenderDevice()->getHandle(),
    //         &createInfo,
    //         nullptr,
    //         &m_renderPass
    //     )
    // );

    // m_framebuffers.resize( getRenderDevice()->getSwapchainImageCount() );
    // for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
    //     auto attachments = std::array< VkImageView, 1 > {
    //         m_colorAttachment.imageViews[ i ]->getHandle(),
    //     };

    //     auto createInfo = VkFramebufferCreateInfo {
    //         .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    //         .pNext = nullptr,
    //         .renderPass = m_renderPass,
    //         .attachmentCount = uint32_t( attachments.size() ),
    //         .pAttachments = attachments.data(),
    //         .width = extent.width,
    //         .height = extent.height,
    //         .layers = 1,
    //     };

    //     CRIMILD_VULKAN_CHECK(
    //         vkCreateFramebuffer(
    //             getRenderDevice()->getHandle(),
    //             &createInfo,
    //             nullptr,
    //             &m_framebuffers[ i ]
    //         )
    //     );
    // }

    // m_pipeline = [ & ] {
    //     auto program = crimild::alloc< ShaderProgram >();
    //     program->setShaders(
    //         Array< SharedPointer< Shader > > {
    //             crimild::alloc< Shader >(
    //                 Shader::Stage::VERTEX,
    //                 R"(
    //                     layout ( location = 0 ) out vec2 outUV;

    //                     void main()
    //                     {
    //                         outUV = vec2( ( gl_VertexIndex << 1 ) & 2, gl_VertexIndex & 2 );
    //                         gl_Position = vec4( outUV * 2.0f - 1.0f, 0.0f, 1.0f );
    //                         outUV.y = 1 - outUV.y;
    //                     }
    //                 )"
    //             ),
    //             crimild::alloc< Shader >(
    //                 Shader::Stage::FRAGMENT,
    //                 R"(
    //                     layout ( location = 0 ) in vec2 inUV;

    //                     layout ( set = 0, binding = 0 ) uniform sampler2D samplerColor;

    //                     layout ( location = 0 ) out vec4 outFragColor;

    //                     void main()
    //                     {
    //                         vec4 color = texture( samplerColor, inUV );
    //                         if (color.a < 0.01 ) {
    //                             discard;
    //                         }
    //                         outFragColor = color;
    //                     }
    //                 )"
    //             ),
    //         }
    //     );

    //     const auto viewport = ViewportDimensions::fromExtent( m_renderArea.extent.width, m_renderArea.extent.height );

    //     auto pipeline = std::make_unique< GraphicsPipeline >(
    //         getRenderDevice(),
    //         m_renderPass,
    //         GraphicsPipeline::Descriptor {
    //             .descriptorSetLayouts = std::vector< VkDescriptorSetLayout > {
    //                 m_inputs[ 0 ]->descriptorSetLayout,
    //             },
    //             .program = program.get(),
    //             .colorAttachmentCount = 1,
    //             .viewport = viewport,
    //             .scissor = viewport,
    //         }
    //     );
    //     getRenderDevice()->setObjectName( pipeline->getHandle(), "OverlayPass" );
    //     return pipeline;
    // }();
}

void OverlayPass::clear( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    m_pipeline = nullptr;

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( getRenderDevice()->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    getRenderDevice()->destroyFramebufferAttachment( m_colorAttachment );

    vkDestroyRenderPass( getRenderDevice()->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;
}
