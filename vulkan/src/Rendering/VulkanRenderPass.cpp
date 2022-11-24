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

#include "Rendering/VulkanRenderPass.hpp"

#include "Rendering/VulkanFramebuffer.hpp"
#include "Rendering/VulkanFramebufferAttachment.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;

vulkan::RenderPass::RenderPass(
    const RenderDevice *rd,
    const std::vector< const FramebufferAttachment * > &attachments,
    bool clearAttachments
) noexcept
    : WithConstRenderDevice( rd )
{
    m_renderArea = VkRect2D {
        .extent = attachments.front()->extent,
        .offset = { 0, 0 },
    };

    std::vector< VkAttachmentDescription > attachmentDescriptions;
    std::vector< VkAttachmentReference > colorReferences;
    std::vector< VkAttachmentReference > depthStencilReferences;

    const auto loadOp = clearAttachments ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    const auto storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    uint32_t attachmentIndex = 0;
    for ( const auto *att : attachments ) {
        const bool isColor = rd->formatIsColor( att->format );
        const bool isDepthStencil = rd->formatIsDepthStencil( att->format );

        attachmentDescriptions.push_back(
            VkAttachmentDescription {
                .format = att->format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = loadOp,
                .storeOp = storeOp,
                .stencilLoadOp = isDepthStencil ? loadOp : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = isDepthStencil ? storeOp : VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout =
                    clearAttachments
                        ? VK_IMAGE_LAYOUT_UNDEFINED
                        : ( isColor ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                                    : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ),
                // Final layout is ready for use in another pass (not presentation, though)
                .finalLayout = isColor ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            }
        );

        if ( isColor ) {
            colorReferences.push_back(
                VkAttachmentReference {
                    .attachment = attachmentIndex++,
                    .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                }
            );
            if ( clearAttachments ) {
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
            }
        } else {
            depthStencilReferences.push_back(
                VkAttachmentReference {
                    .attachment = attachmentIndex++,
                    .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                }
            );
            if ( clearAttachments ) {
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
    }

    std::vector< VkSubpassDescription > subpasses = {
        VkSubpassDescription {
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
        }
    };

    std::vector< VkSubpassDependency > dependencies = {
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
        .subpassCount = uint32_t( subpasses.size() ),
        .pSubpasses = subpasses.data(),
        .dependencyCount = uint32_t( dependencies.size() ),
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
}

vulkan::RenderPass::RenderPass(
    const vulkan::RenderDevice *rd,
    const VkRenderPassCreateInfo &createInfo
) noexcept
    : WithConstRenderDevice( rd )
{
    CRIMILD_VULKAN_CHECK(
        vkCreateRenderPass(
            getRenderDevice()->getHandle(),
            &createInfo,
            nullptr,
            &m_renderPass
        )
    );
}

vulkan::RenderPass::~RenderPass( void ) noexcept
{
    vkDestroyRenderPass( getRenderDevice()->getHandle(), m_renderPass, nullptr );
    m_renderPass = VK_NULL_HANDLE;

    m_clearValues.clear();
}

void vulkan::RenderPass::setName( std::string_view name ) const noexcept
{
    getRenderDevice()->setObjectName( uint64_t( m_renderPass ), VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT, name );
}

void vulkan::RenderPass::begin( VkCommandBuffer commandBuffer, const SharedPointer< vulkan::Framebuffer > &framebuffer ) const noexcept
{
    auto beginInfo = vulkan::initializers::renderPassBeginInfo();
    beginInfo.renderPass = m_renderPass;
    beginInfo.framebuffer = *framebuffer;
    beginInfo.renderArea = m_renderArea;
    beginInfo.clearValueCount = uint32_t( m_clearValues.size() );
    beginInfo.pClearValues = m_clearValues.data();

    begin( commandBuffer, beginInfo );
}

void vulkan::RenderPass::begin( VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo &beginInfo ) const noexcept
{
    vkCmdBeginRenderPass( commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE );
}

void vulkan::RenderPass::end( VkCommandBuffer commandBuffer ) const noexcept
{
    vkCmdEndRenderPass( commandBuffer );
}
