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
 *     * Neither the name of the copyright holders nor the
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

#include "Rendering/RenderPass.hpp"

#include "VulkanFramebuffer.hpp"
#include "VulkanRenderDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanImageView.hpp"

using namespace crimild;
using namespace crimild::vulkan;

#if 0

Framebuffer::~Framebuffer( void ) noexcept
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

SharedPointer< Framebuffer > FramebufferManager::create( Framebuffer::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating framebuffer" );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    std::vector< VkImageView > attachments;
    for ( const auto &att : descriptor.attachments ) {
        attachments.push_back( att->handler );
    }

    auto createInfo = VkFramebufferCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = descriptor.renderPass->handler,
        .attachmentCount = static_cast< uint32_t >( attachments.size() ),
        .pAttachments = attachments.data(),
        .width = descriptor.extent.width,
        .height = descriptor.extent.height,
        .layers = 1,
    };

    VkFramebuffer framebufferHandler;
    CRIMILD_VULKAN_CHECK(
     	vkCreateFramebuffer(
    		renderDevice->handler,
            &createInfo,
        	nullptr,
            &framebufferHandler
    	)
 	);

    auto framebuffer = crimild::alloc< Framebuffer >();
    framebuffer->handler = framebufferHandler;
    framebuffer->manager = this;
    framebuffer->renderDevice = renderDevice;
    framebuffer->extent = descriptor.extent;
    insert( crimild::get_ptr( framebuffer ) );
    m_framebuffers.add( crimild::get_ptr( framebuffer ) );
    return framebuffer;
}

void FramebufferManager::destroy( Framebuffer *framebuffer ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying framebuffer" );

    if ( framebuffer->renderDevice != nullptr && framebuffer->handler != VK_NULL_HANDLE ) {
        vkDestroyFramebuffer(
            framebuffer->renderDevice->handler,
            framebuffer->handler,
            nullptr
        );
    }

    m_framebuffers.remove( framebuffer );

    framebuffer->handler = VK_NULL_HANDLE;
    framebuffer->manager = nullptr;
    framebuffer->renderDevice = nullptr;
    erase( framebuffer );
}

#endif

crimild::Bool FramebufferManager::bind( Framebuffer *framebuffer ) noexcept
{
	if ( validate( framebuffer ) ) {
		return true;
	}

	CRIMILD_LOG_TRACE( "Binding Vulkan Framebuffer" );

	auto renderDevice = getRenderDevice();
	auto swapchain = renderDevice->getSwapchain();
    auto imageCount = swapchain->images.size();
	auto width = framebuffer->extent.width;
	auto height = framebuffer->extent.height;
	if ( framebuffer->extent.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
		width *= swapchain->extent.width;
		height *= swapchain->extent.height;
	}

    auto currentRenderPass = renderDevice->getCurrentRenderPass();
    auto renderPass = renderDevice->getBindInfo( currentRenderPass );

    auto handlers = containers::Array< VkFramebuffer >( imageCount );

    for ( auto i = 0l; i < imageCount; i++ ) {
        auto attachments = framebuffer->attachments.map(
            [&]( auto &attachment ) {
                auto imageView = attachment;
                if ( attachment->type == ImageView::Type::IMAGE_VIEW_SWAPCHAIN ) {
                    imageView = swapchain->imageViews[ i ];
                }
                return renderDevice->getBindInfo( crimild::get_ptr( imageView ) );
            }
        );

        auto createInfo = VkFramebufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass,
            .attachmentCount = crimild::UInt32( attachments.size() ),
            .pAttachments = attachments.getData(),
            .width = crimild::UInt32( width ),
            .height = crimild::UInt32( height ),
            .layers = 1,
        };

        CRIMILD_VULKAN_CHECK(
            vkCreateFramebuffer(
                renderDevice->handler,
                &createInfo,
                nullptr,
                &handlers[ i ]
            )
        );
    }

    setHandlers( framebuffer, handlers );

	return ManagerImpl::bind( framebuffer );
}

crimild::Bool FramebufferManager::unbind( Framebuffer *framebuffer ) noexcept
{
	if ( !validate( framebuffer ) ) {
		return false;
	}

	CRIMILD_LOG_TRACE( "Unbind Vulkan Framebuffer" );

	auto renderDevice = getRenderDevice();

    if ( renderDevice != nullptr ) {
    	eachHandler(
            framebuffer,
            [&]( auto handler ) {
        		vkDestroyFramebuffer(
                	renderDevice->handler,
                    handler,
                	nullptr
                );
    		}
        );
    }

    removeHandlers( framebuffer );

	return ManagerImpl::unbind( framebuffer );
}

