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

#include "Rendering/RenderPasses/VulkanClearPass.hpp"

#include "Rendering/VulkanRenderDevice.hpp"

#include <array>

using namespace crimild;
using namespace crimild::vulkan;

ClearPass::ClearPass( vulkan::RenderDevice *renderDevice, const std::vector< const FramebufferAttachment * > &attachments ) noexcept
    : RenderPassBase( renderDevice ),
      m_attachments( attachments )
{
    init();
}

ClearPass::~ClearPass( void ) noexcept
{
    deinit();
}

Event ClearPass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            deinit();
            init();
            break;
        }

        default:
            break;
    }

    return e;
}

void ClearPass::render( void ) noexcept
{
    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();
    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    auto renderPassInfo = VkRenderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_renderPass,
        .framebuffer = m_framebuffers[ currentFrameIndex ],
        .renderArea = m_renderArea,
        .clearValueCount = static_cast< uint32_t >( m_clearValues.size() ),
        .pClearValues = m_clearValues.data(),
    };

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
    vkCmdEndRenderPass( commandBuffer );
}

void ClearPass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    if ( m_attachments.empty() ) {
        CRIMILD_LOG_WARNING( "No attachments provided for clearing" );
        return;
    }

    m_renderArea = VkRect2D {
        .extent = m_attachments.front()->extent,
        .offset = { 0, 0 },
    };

    std::vector< VkAttachmentDescription > attachmentDescriptions;
    std::vector< VkAttachmentReference > colorReferences;
    std::vector< VkAttachmentReference > depthStencilReferences;

    attachmentDescriptions.reserve( m_attachments.size() );
    colorReferences.reserve( m_attachments.size() );
    depthStencilReferences.reserve( m_attachments.size() );
    m_clearValues.reserve( m_attachments.size() );

    for ( uint32_t i = 0; i < m_attachments.size(); ++i ) {
        const auto &att = m_attachments[ i ];
        const auto isColorAttachment = getRenderDevice()->formatIsColor( att->format );
        const auto isDepthStencilAttachment = getRenderDevice()->formatIsDepthStencil( att->format );
        attachmentDescriptions.push_back(
            VkAttachmentDescription {
                .format = att->format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                // Use STORE here since we could access the contents of these attachments outside
                // of the current render pass.
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = isDepthStencilAttachment ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = isDepthStencilAttachment ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                // Final layout is ready for use in another pass (not presentation, though)
                .finalLayout = isColorAttachment ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            }
        );

        if ( isColorAttachment ) {
            colorReferences.push_back(
                VkAttachmentReference {
                    .attachment = i,
                    .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                }
            );
            m_clearValues.push_back(
                VkClearValue {
                    .color = {
                        .float32 = {
                            att->clearColor.r,
                            att->clearColor.g,
                            att->clearColor.b,
                            att->clearColor.a,
                        },
                    },
                }
            );
        } else {
            depthStencilReferences.push_back(
                VkAttachmentReference {
                    .attachment = i,
                    .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                }
            );
            m_clearValues.push_back(
                VkClearValue {
                    .depthStencil = {
                        att->clearDepthStencil[ 0 ],
                        uint32_t( att->clearDepthStencil[ 1 ] ),
                    },
                }
            );
        }
    }

    auto subpass = VkSubpassDescription {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = crimild::UInt32( colorReferences.size() ),
        .pColorAttachments = colorReferences.data(),
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = !depthStencilReferences.empty() ? depthStencilReferences.data() : nullptr,
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
        .attachmentCount = uint32_t( attachmentDescriptions.size() ),
        .pAttachments = attachmentDescriptions.data(),
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
            &m_renderPass
        )
    );

    m_framebuffers.resize( getRenderDevice()->getSwapchainImageCount() );
    for ( uint8_t i = 0; i < m_framebuffers.size(); ++i ) {
        std::vector< VkImageView > imageViews;
        imageViews.reserve( m_attachments.size() );
        for ( auto att : m_attachments ) {
            imageViews.push_back( att->imageViews[ i ] );
        }

        auto createInfo = VkFramebufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_renderPass,
            .attachmentCount = uint32_t( imageViews.size() ),
            .pAttachments = imageViews.data(),
            .width = m_renderArea.extent.width,
            .height = m_renderArea.extent.height,
            .layers = 1,
        };

        CRIMILD_VULKAN_CHECK(
            vkCreateFramebuffer(
                getRenderDevice()->getHandle(),
                &createInfo,
                nullptr,
                &m_framebuffers[ i ]
            )
        );
    }
}

void ClearPass::deinit( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    vkDeviceWaitIdle( getRenderDevice()->getHandle() );

    for ( auto &fb : m_framebuffers ) {
        vkDestroyFramebuffer( getRenderDevice()->getHandle(), fb, nullptr );
    }
    m_framebuffers.clear();

    vkDestroyRenderPass( getRenderDevice()->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;

    m_clearValues.clear();
}
