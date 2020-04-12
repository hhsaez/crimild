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
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool FramebufferManager::bind( Framebuffer *framebuffer ) noexcept
{
    return false;
    /*
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
     */
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

