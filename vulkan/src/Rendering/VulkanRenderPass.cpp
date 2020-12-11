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
#include "Rendering/FrameGraph.hpp"
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
    auto extent = utils::getExtent( renderPass->extent, renderDevice );

    auto createAttachmentImage = [&]( Attachment *attachment ) {
        auto image = crimild::alloc< Image >();
        if ( attachment->imageView != nullptr ) {
            if ( attachment->imageView->image != nullptr ) {
                image = attachment->imageView->image;
            }
        }
        image->extent = {
            .scalingMode = ScalingMode::FIXED,
            .width = crimild::Real32( extent.width ),
            .height = crimild::Real32( extent.height ),
        };
        image->format = attachment->format;
        image->setMipLevels( 1 ); // only 1 mip level is allowed for framebuffer's images
        return image;
    };

    auto createAttachmentImageView = [&]( Attachment *attachment ) {
        auto imageView = attachment->imageView != nullptr ? attachment->imageView : crimild::alloc< ImageView >();
        imageView->type = ImageView::Type::IMAGE_VIEW_2D;
        imageView->image = createAttachmentImage( attachment );
        imageView->format = imageView->image->format;
        return imageView;
    };

	auto colorReferences = Array< VkAttachmentReference >();
	auto depthStencilReferences = Array< VkAttachmentReference >();

    bool hasPresentation = false;

    auto attachments = renderPass->attachments.map(
       	[ &, idx = 0u ]( auto const &attachment ) mutable {
            auto format = attachment->format;
        	if ( format != Format::COLOR_SWAPCHAIN_OPTIMAL ) {
				// Create an image view for this attachemnt
                // TODO: Have a pool of image views instead?
                attachment->imageView = createAttachmentImageView( crimild::get_ptr( attachment ) );
        	}

			auto initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			auto finalLayout = VK_IMAGE_LAYOUT_GENERAL;

            auto isPresentation = attachment->getFrameGraph()->isPresentation( attachment );

			if ( utils::formatIsDepthStencil( format ) ) {
				depthStencilReferences.add(
					VkAttachmentReference {
						.attachment = idx++,
						.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					}
				);
				finalLayout = isPresentation ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else {
				colorReferences.add(
					VkAttachmentReference {
						.attachment = idx++,
						.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					}
				);
                hasPresentation |= isPresentation;
				finalLayout = isPresentation ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}

            // 1. Write only: LoadOp::CLEAR or LoadOp::DONT_CARE
        	// 2. Read-Write: LoadOp::LOAD
        	// 3. Read only: LoadOp::LOAD?

            return VkAttachmentDescription {
                .format = utils::getFormat( renderDevice, attachment->format ),
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = utils::formatIsDepthStencil( attachment->format ) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = initialLayout,
                .finalLayout = finalLayout,
            };
    	}
   	);

	auto subpass = VkSubpassDescription {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = crimild::UInt32( colorReferences.size() ),
		.pColorAttachments = colorReferences.getData(),
		.pDepthStencilAttachment = !depthStencilReferences.empty() ? depthStencilReferences.getData() : nullptr,
		.inputAttachmentCount = 0,
		.pInputAttachments = nullptr,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr,
		.pResolveAttachments = nullptr,
	};

    auto dependencies = std::vector< VkSubpassDependency > {
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,//hasPresentation ? VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT : VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,//hasPresentation ? VK_ACCESS_MEMORY_READ_BIT : VK_ACCESS_SHADER_READ_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,//hasPresentation ? VkAccessFlags( VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT ) : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        },
        {
            .srcSubpass = 0,
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,//hasPresentation ? VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT : VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,//hasPresentation ? VkAccessFlags( VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT ) : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,//hasPresentation ? VK_ACCESS_MEMORY_READ_BIT : VK_ACCESS_SHADER_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        }
    };

	auto createInfo = VkRenderPassCreateInfo {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = static_cast< crimild::UInt32 >( attachments.size() ),
        .pAttachments = attachments.getData(),
		.subpassCount = 1,
		.pSubpasses = &subpass,
        .dependencyCount = crimild::UInt32( dependencies.size() ),
        .pDependencies = dependencies.data(),
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
            .width = extent.width,
            .height = extent.height,
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
