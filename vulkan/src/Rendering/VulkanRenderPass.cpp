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

#include "Rendering/Image.hpp"
#include "Rendering/VulkanRenderPass.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool RenderPassManager::bind( RenderPass *renderPass ) noexcept
{
	if ( validate( renderPass ) ) {
		return true;
	}

	CRIMILD_LOG_TRACE( "Binding Vulkan Render Pass" );

	auto renderDevice = getRenderDevice();
    auto swapchain = renderDevice->getSwapchain();
    auto imageCount = swapchain->images.size();
    auto viewport = renderPass->viewport;
    auto fbWidth = viewport.dimensions.getWidth();
    auto fbHeight = viewport.dimensions.getHeight();
    if ( viewport.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
        fbWidth *= swapchain->extent.width;
        fbHeight *= swapchain->extent.height;
    }

    auto createAttachmentImage = [&]( Attachment *attachment ) {
        auto image = crimild::alloc< Image >();
        image->extent = {
            .scalingMode = ScalingMode::FIXED,
            .width = fbWidth,
            .height = fbHeight,
        };
        image->usage = attachment->usage;
        image->format = attachment->format;
        return image;
    };

    auto createAttachmentImageView = [&]( Attachment *attachment ) {
        auto imageView = crimild::alloc< ImageView >();
        imageView->type = ImageView::Type::IMAGE_VIEW_2D;
        imageView->image = createAttachmentImage( attachment );
        imageView->format = imageView->image->format;
        return imageView;
    };

    std::map< Attachment *, crimild::UInt32 > attachmentReferences;
    auto attachments = renderPass->attachments.map(
       	[ &, idx = 0l ]( auto const &attachment ) mutable {
        	attachmentReferences[ crimild::get_ptr( attachment ) ] = idx++;
            auto format = attachment->format;
            auto isPresent = false;
        	if ( format != Format::COLOR_SWAPCHAIN_OPTIMAL ) {
				// Create an image view for this attachemnt
                // TODO: Have a pool of image views instead?
                attachment->imageView = createAttachmentImageView( crimild::get_ptr( attachment ) );
        	}
            else {
                isPresent = true;
            }
            return VkAttachmentDescription {
                .format = utils::getFormat( renderDevice, attachment->format ),
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = isPresent ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_GENERAL,
            };
    	}
   	);

    auto subpassCount = renderPass->subpasses.size();

    // These must be kept alive for render pass creation
    containers::Array< VkSubpassDescription > subpasses( subpassCount );
    containers::Array< containers::Array< VkAttachmentReference >> subpassColorReferences( subpassCount );
    containers::Array< containers::Array< VkAttachmentReference >> subpassDepthStencilReferences( subpassCount );
    containers::Array< VkSubpassDependency > subpassDependencies;

    renderPass->subpasses.each(
        [&]( auto &subpass, auto index ) {
            auto &colorReferences = subpassColorReferences[ index ];
            auto &depthStencilReferences = subpassDepthStencilReferences[ index ];
        	subpass->outputs.each( [&]( auto &att ) {
                auto attIdx = attachmentReferences[ crimild::get_ptr( att ) ];
                if ( utils::formatIsColor( att->format ) ) {
                    colorReferences.add({
                        .attachment = attIdx,
                        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    });
                }
                else if ( utils::formatIsDepthStencil( att->format ) ) {
                    depthStencilReferences.add({
                        .attachment = attIdx,
                        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                    });
                }
                else {
                    CRIMILD_LOG_ERROR( "Invalid attachment format: ", crimild::UInt32( att->format ) );
                    return;
                }
            });
            subpasses[ index ] = {
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                .colorAttachmentCount = crimild::UInt32( colorReferences.size() ),
                .pColorAttachments = colorReferences.getData(),
                .pDepthStencilAttachment = depthStencilReferences.getData(),
            };
    	}
    );

	auto createInfo = VkRenderPassCreateInfo {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = static_cast< crimild::UInt32 >( attachments.size() ),
        .pAttachments = attachments.getData(),
		.subpassCount = static_cast< crimild::UInt32 >( subpasses.size() ),
		.pSubpasses = subpasses.getData(),
		.dependencyCount = static_cast< crimild::UInt32 >( subpassDependencies.size() ),
		.pDependencies = subpassDependencies.getData(),
	};

	RenderPassBindInfo bindInfo;
	CRIMILD_VULKAN_CHECK(
		vkCreateRenderPass(
			renderDevice->handler,
			&createInfo,
			nullptr,
           	&bindInfo.handler
		)
	);

    bindInfo.framebuffers.resize( imageCount );
    for ( auto i = 0l; i < imageCount; i++ ) {
        auto fbAttachments = renderPass->attachments.map( [&]( auto &att ) {
            if ( auto iv = crimild::get_ptr( att->imageView ) ) {
                return renderDevice->getBindInfo( iv );
            }
            return renderDevice->getBindInfo( crimild::get_ptr( swapchain->imageViews[ i ] ) );
        });

        auto createInfo = VkFramebufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = bindInfo.handler,
            .attachmentCount = crimild::UInt32( fbAttachments.size() ),
            .pAttachments = fbAttachments.getData(),
            .width = crimild::UInt32( fbWidth ),
            .height = crimild::UInt32( fbHeight ),
            .layers = 1,
        };

        CRIMILD_VULKAN_CHECK(
            vkCreateFramebuffer(
                renderDevice->handler,
                &createInfo,
                nullptr,
                &bindInfo.framebuffers[ i ]
        	)
     	);
    }

    setBindInfo( renderPass, bindInfo );

	return ManagerImpl::bind( renderPass );
}

crimild::Bool vulkan::RenderPassManager::unbind( RenderPass *renderPass ) noexcept
{
	if ( !validate( renderPass ) ) {
		return false;
	}

	CRIMILD_LOG_TRACE( "Unbidn Vulkan Render Pass" );

	auto renderDevice = getRenderDevice();

	auto info = getBindInfo( renderPass );

    if ( renderDevice != nullptr ) {
        info.framebuffers.each( [&]( auto &fb ) {
            vkDestroyFramebuffer( renderDevice->handler, fb, nullptr );
        });
        if ( info.handler != VK_NULL_HANDLE ) {
            vkDestroyRenderPass( renderDevice->handler, info.handler, nullptr );
        }
    }

	removeBindInfo( renderPass );

	return ManagerImpl::unbind( renderPass );
}

