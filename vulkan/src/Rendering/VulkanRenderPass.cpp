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

#include "VulkanRenderPass.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanRenderDevice.hpp"
#include "VulkanSwapchain.hpp"

using namespace crimild;
using namespace crimild::vulkan;

#if 0

RenderPass::~RenderPass( void ) noexcept
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

crimild::SharedPointer< RenderPass > RenderPassManager::create( RenderPass::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating render pass" );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    auto physicalDevice = renderDevice->physicalDevice;
    auto msaaSamples = physicalDevice->msaaSamples;
    auto swapchain = descriptor.swapchain;

    auto colorAttachment = VkAttachmentDescription {
        // Format must match the one in the swapchain
        .format = swapchain->format,

        // Configure multisampling based on device
        .samples = msaaSamples,

        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

        // We don't need stencil right now
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

        // We don't care what the previous image was and we're going to clear it anyway
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,

        // We want the image ready for presentation after rendering
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    auto colorAttachmentRef = VkAttachmentReference {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    auto depthAttachment = VkAttachmentDescription {
        .format = utils::findDepthFormat( renderDevice ),
        .samples = msaaSamples,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    auto depthAttachmentRef = VkAttachmentReference {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    auto colorAttachmentResolve = VkAttachmentDescription {
		.format = swapchain->format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    auto colorAttachmentResolveRef = VkAttachmentReference {
        .attachment = 2,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    auto subpass = VkSubpassDescription {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pDepthStencilAttachment = &depthAttachmentRef,
    };

    auto attachments = std::vector< VkAttachmentDescription > {
        colorAttachment,
        depthAttachment,
    };

    if ( msaaSamples != VK_SAMPLE_COUNT_1_BIT ) {
        // Setup for multisampling
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;
        attachments.push_back( colorAttachmentResolve );
    }

    auto subpassDependency = VkSubpassDependency {
        // Which subpass to wait on
        .srcSubpass = VK_SUBPASS_EXTERNAL, // any previous subpass
        .dstSubpass = 0,

        // Operations to wait on and the stage in which they occur
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,

        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
        	| VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    auto createInfo = VkRenderPassCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = static_cast< crimild::UInt32 >( attachments.size() ),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency,
    };

    VkRenderPass renderPassHandler;
    CRIMILD_VULKAN_CHECK(
     	vkCreateRenderPass(
       		renderDevice->handler,
           	&createInfo,
           	nullptr,
           	&renderPassHandler
   		)
 	);

    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->handler = renderPassHandler;
    renderPass->manager = this;
    renderPass->renderDevice = renderDevice;
    insert( crimild::get_ptr( renderPass ) );
    return renderPass;
}

void RenderPassManager::destroy( RenderPass *renderPass ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan render pass" );

    if ( renderPass->renderDevice != nullptr && renderPass->handler != VK_NULL_HANDLE ) {
        vkDestroyRenderPass(
            renderPass->renderDevice->handler,
            renderPass->handler,
            nullptr
        );
    }
    renderPass->manager = nullptr;
    renderPass->renderDevice = nullptr;
    renderPass->handler = VK_NULL_HANDLE;
    erase( renderPass );
}

#endif

crimild::Bool RenderPassManager::bind( RenderPass *renderPass ) noexcept
{
	if ( validate( renderPass ) ) {
		return true;
	}

	CRIMILD_LOG_TRACE( "Binding Vulkan Render Pass" );

	auto renderDevice = getRenderDevice();

	auto getImageLayout = [&]( const crimild::Image::Usage &usage ) -> VkImageLayout {
		if ( usage & crimild::Image::Usage::COLOR_ATTACHMENT ) {
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		else if ( usage & crimild::Image::Usage::DEPTH_STENCIL_ATTACHMENT ) {
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		else {
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}
	};

    std::map< Attachment *, VkAttachmentReference > attachmentReferences;
    auto attachments = renderPass->attachments.map(
       	[ &, idx = 0l ]( auto const &attachment ) mutable {
            attachmentReferences[ crimild::get_ptr( attachment ) ] = {
                .attachment = crimild::UInt32( idx++ ),
                .layout = getImageLayout( attachment->usage ),
            };
            return VkAttachmentDescription {
                .format = utils::getFormat( renderDevice, attachment->format ),
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = ( attachment->usage & crimild::Image::Usage::PRESENTATION ) ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = ( attachment->usage & crimild::Image::Usage::PRESENTATION ) ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : getImageLayout( attachment->usage ),
            };
    	}
   	);

	std::vector< VkSubpassDescription > subpasses;
	std::vector< VkSubpassDependency > subpassDependencies;
    std::vector< std::vector< VkAttachmentReference >> subpassColorAttachments( renderPass->subpasses.size() );
    std::vector< std::vector< VkAttachmentReference >> subpassDepthStencilAttachments( renderPass->subpasses.size() );

	renderPass->subpasses.each(
		[&]( auto const &subpass, auto idx ) {
			auto &colorAttachments = subpassColorAttachments[ idx ];
			subpass->colorAttachments.each(
				[&]( const auto &attachment ) {
					colorAttachments.push_back( attachmentReferences[ crimild::get_ptr( attachment ) ] );
				}
			);
			auto &depthStencilAttachments = subpassDepthStencilAttachments[ idx ];
			if ( auto att = crimild::get_ptr( subpass->depthStencilAttachment ) ) {
				depthStencilAttachments.push_back( attachmentReferences[ att ] );
			}
			subpasses.push_back(
				VkSubpassDescription {
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
					.colorAttachmentCount = crimild::UInt32( colorAttachments.size() ),
					.pColorAttachments = colorAttachments.data(),
					.pDepthStencilAttachment = depthStencilAttachments.data(),
				}
			);
			subpassDependencies.push_back(
				VkSubpassDependency {
					.srcSubpass = VK_SUBPASS_EXTERNAL,
					.dstSubpass = crimild::UInt32( idx ),
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask = 0,
					.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				}
			);
		}
	);

	auto createInfo = VkRenderPassCreateInfo {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = static_cast< crimild::UInt32 >( attachments.size() ),
        .pAttachments = attachments.getData(),
		.subpassCount = static_cast< crimild::UInt32 >( subpasses.size() ),
		.pSubpasses = subpasses.data(),
		.dependencyCount = static_cast< crimild::UInt32 >( subpassDependencies.size() ),
		.pDependencies = subpassDependencies.data(),
	};

	VkRenderPass handler;
	CRIMILD_VULKAN_CHECK(
		vkCreateRenderPass(
			renderDevice->handler,
			&createInfo,
			nullptr,
			&handler
		)
	);

	setBindInfo( renderPass, handler );		

	return ManagerImpl::bind( renderPass );
}

crimild::Bool vulkan::RenderPassManager::unbind( RenderPass *renderPass ) noexcept
{
	if ( !validate( renderPass ) ) {
		return false;
	}

	CRIMILD_LOG_TRACE( "Unbidn Vulkan Render Pass" );

	auto renderDevice = getRenderDevice();

	auto handler = getBindInfo( renderPass );

	if ( renderDevice != nullptr && handler != VK_NULL_HANDLE ) {
		vkDestroyRenderPass(
			renderDevice->handler,
			handler,
			nullptr
		);
	}

	removeBindInfo( renderPass );

	return ManagerImpl::unbind( renderPass );
}

