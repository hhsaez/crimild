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

#include "Rendering/VulkanFramebuffer.hpp"

#include "Rendering/VulkanFramebufferAttachment.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderPass.hpp"

using namespace crimild;

std::vector< std::shared_ptr< vulkan::Framebuffer > > vulkan::Framebuffer::createInFlightFramebuffers(
    const vulkan::RenderDevice *rd,
    const std::shared_ptr< vulkan::RenderPass > &renderPass,
    const std::vector< const vulkan::FramebufferAttachment * > &attachments
) noexcept
{
    // Assume all attachments have the same size.
    const auto extent = attachments.front()->extent;

    const auto N = rd->getInFlightFrameCount();
    std::vector< std::shared_ptr< vulkan::Framebuffer > > framebuffers( N );
    for ( uint32_t i = 0; i < N; ++i ) {
        std::vector< VkImageView > imageViews;
        for ( const auto *att : attachments ) {
            imageViews.push_back( *att->imageViews[ i ] );
        }

        auto createInfo = vulkan::initializers::framebufferCreateInfo();
        createInfo.renderPass = *renderPass;
        createInfo.attachmentCount = uint32_t( imageViews.size() );
        createInfo.pAttachments = imageViews.data();
        createInfo.width = extent.width;
        createInfo.height = extent.height;

        framebuffers[ i ] = crimild::alloc< vulkan::Framebuffer >( rd, createInfo );
    }

    return framebuffers;
}

vulkan::Framebuffer::Framebuffer( const vulkan::RenderDevice *rd, const VkFramebufferCreateInfo &createInfo ) noexcept
    : WithConstRenderDevice( rd )
{
    CRIMILD_VULKAN_CHECK(
        vkCreateFramebuffer(
            getRenderDevice()->getHandle(),
            &createInfo,
            nullptr,
            &m_framebuffer
        )
    );
}

vulkan::Framebuffer::~Framebuffer( void ) noexcept
{
    vkDestroyFramebuffer( getRenderDevice()->getHandle(), m_framebuffer, nullptr );
    m_framebuffer = VK_NULL_HANDLE;
}