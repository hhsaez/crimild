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

#include "VulkanSwapchain.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanRenderDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanImage.hpp"
#include "VulkanSemaphore.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::vulkan;

Swapchain::~Swapchain( void ) noexcept
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

Swapchain::AcquireImageResult Swapchain::acquireNextImage( const Semaphore *imageAvailableSemaphore ) const noexcept
{
    crimild::UInt32 imageIndex;
	auto ret = vkAcquireNextImageKHR(
		renderDevice->handler,
		handler,
		std::numeric_limits< uint64_t >::max(), // disable timeout
		imageAvailableSemaphore->handler,
		VK_NULL_HANDLE,
		&imageIndex
	);

    return AcquireImageResult {
        .success = ( ret == VK_SUCCESS ),
        .imageIndex = imageIndex,
        .outOfDate = ( ret == VK_ERROR_OUT_OF_DATE_KHR ),
    };
}

Swapchain::PresentImageResult Swapchain::presentImage( crimild::UInt32 imageIndex, const Semaphore *signal ) const noexcept
{
	VkSemaphore signalSemaphores[] = {
		signal->handler,
	};

	VkSwapchainKHR swapchains[] = {
		handler,
	};
	
	auto presentInfo = VkPresentInfoKHR {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphores,
		.swapchainCount = 1,
		.pSwapchains = swapchains,
		.pImageIndices = &imageIndex,
		.pResults = nullptr,
	};

	auto ret = vkQueuePresentKHR(
		renderDevice->presentQueue,
		&presentInfo
	);

    return PresentImageResult {
        .success = ( ret == VK_SUCCESS ),
        .outOfDate = ( ret == VK_ERROR_OUT_OF_DATE_KHR || VK_SUBOPTIMAL_KHR ),
    };
}

void Swapchain::retrieveSwapchainImages( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Retrieving swapchain images" );

    images.clear();
	
	crimild::UInt32 imageCount;
	vkGetSwapchainImagesKHR(
		renderDevice->handler,
		handler,
		&imageCount,
		nullptr
	);
	std::vector< VkImage > imagesHandlers( imageCount );
	vkGetSwapchainImagesKHR(
		renderDevice->handler,
		handler,
		&imageCount,
		imagesHandlers.data()
	);

	for ( const auto &imageHandler : imagesHandlers ) {
        auto image = crimild::alloc< Image >();
        image->format = utils::getFormat( format );
        image->extent.scalingMode = ScalingMode::SWAPCHAIN_RELATIVE;
        if ( auto img = crimild::get_ptr( image ) ) {
            renderDevice->setBindInfo( img, { .imageHandler = imageHandler } );
        }
        images.add( image );
	}
}

void Swapchain::createImageViews( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating image views" );

    imageViews.clear();

    images.each(
        [&]( auto &image ) {
            auto imageView = crimild::alloc< ImageView >();
            imageView->type = ImageView::Type::IMAGE_VIEW_SWAPCHAIN;
            imageView->image = image;
            imageView->format = image->format;
            renderDevice->bind( crimild::get_ptr( imageView ) );
            imageViews.add( imageView );
        }
    );
}

SharedPointer< Swapchain > SwapchainManager::create( Swapchain::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan swapchain" );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    auto physicalDevice = renderDevice->physicalDevice;
    auto surface = renderDevice->surface;

    auto swapchainSupport = utils::querySwapchainSupportDetails( physicalDevice->handler, surface->handler );
    auto surfaceFormat = chooseSurfaceFormat( swapchainSupport.formats );
    auto presentMode = choosePresentationMode( swapchainSupport.presentModes );
    auto extent = chooseExtent( swapchainSupport.capabilities, VkExtent2D { descriptor.extent.x(), descriptor.extent.y() } );

    // The Vulkan implementation defines a minimum number of images to work with.
    // We request one more image than the minimum. Also, make sure not to exceed the
    // maximum number of images (a value of 0 means there's no maximum)
    auto imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if ( swapchainSupport.capabilities.maxImageCount > 0 ) {
        imageCount = std::min(
            swapchainSupport.capabilities.maxImageCount,
            imageCount
        );
    }

    auto createInfo = VkSwapchainCreateInfoKHR {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface->handler,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1, // This may change for stereoscopic rendering (VR)
    };

    // Images created by the swapchain are usually used as color attachments
    // It might be possible to used the for other purposes, like sampling or
    // to copy to or from them to other surfaces.
    // TODO: add VK_IMAGE_USAGE_TRANSFER_SRC_BIT maybe? That can be used for screenshots
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto indices = utils::findQueueFamilies( physicalDevice->handler, surface->handler );
    uint32_t queueFamilyIndices[] = {
        indices.graphicsFamily[ 0 ],
        indices.presentFamily[ 0 ],
    };

    if ( indices.graphicsFamily != indices.presentFamily ) {
        // If the graphics and present families don't match we need to
        // share images since they're not exclusive to queues
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        // If they are the same, the queue can have exclusive access to images
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    // This can be used to rotate images or flip them horizontally
    createInfo.preTransform = swapchainSupport.capabilities.currentTransform;

    // Use for blending with other windows in the window system
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;

    // Ignore pixels that are obscured by other windows
    createInfo.clipped = VK_TRUE;

    // This is used during the recreation of a swapchain (maybe due to window is resized)
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchainHandler;
    CRIMILD_VULKAN_CHECK(
     	vkCreateSwapchainKHR(
            renderDevice->handler,
            &createInfo,
            nullptr,
            &swapchainHandler
     	)
	);

    auto swapchain = crimild::alloc< Swapchain >();
    swapchain->handler = swapchainHandler;
    swapchain->renderDevice = renderDevice;
    swapchain->manager = this;
    swapchain->extent = extent;
    swapchain->format = surfaceFormat.format;
    insert( crimild::get_ptr( swapchain ) );
    swapchain->retrieveSwapchainImages();
    swapchain->createImageViews();
    return swapchain;
}

void SwapchainManager::destroy( Swapchain *swapchain ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying swapchain" );

    swapchain->imageViews.clear();
    swapchain->images.clear();

    if ( swapchain->handler != VK_NULL_HANDLE ) {
        vkDestroySwapchainKHR( swapchain->renderDevice->handler, swapchain->handler, nullptr );
    }

    swapchain->handler = VK_NULL_HANDLE;
    swapchain->renderDevice = nullptr;
    swapchain->manager = nullptr;
    erase( swapchain );
}

VkSurfaceFormatKHR SwapchainManager::chooseSurfaceFormat( const std::vector< VkSurfaceFormatKHR > &availableFormats ) noexcept
{
    CRIMILD_LOG_TRACE( "Choosing swapchain surface format" );

    // If no format is available, force what we need
    if ( availableFormats.size() == 1 && availableFormats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
        return {
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        };
    }

    // Favor 32-bit RGBA and sRGBA non-linear colorspace
    for ( const auto &availableFormat : availableFormats ) {
        if ( availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
            return availableFormat;
        }
    }

    // If nothing is found matching what we need, return whatever is available
    return availableFormats[ 0 ];
}

VkPresentModeKHR SwapchainManager::choosePresentationMode( const std::vector< VkPresentModeKHR > &availablePresentModes ) noexcept
{
    CRIMILD_LOG_TRACE( "Choosing swapchain presentation mode" );

    // VSync by default, but may introduce latency
    // FIFO mode is always available (defined in standard)
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for ( const auto &availablePresentMode : availablePresentModes ) {
        if ( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR ) {
            // Triple buffer
            return availablePresentMode;
        }
        else if ( availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR ) {
            // Double buffer. May produce tearing
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

VkExtent2D SwapchainManager::chooseExtent( const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D requestedExtent ) noexcept
{
    CRIMILD_LOG_TRACE( "Choosing swapchain extent" );

    if ( capabilities.currentExtent.width != std::numeric_limits< uint32_t >::max() ) {
        // Capabilites are enforcing a given extent. Return that one
        return capabilities.currentExtent;
    }

    // Keep width/heigth values within the allowed ones, though.
    requestedExtent.width = std::max(
        capabilities.minImageExtent.width,
        std::min( capabilities.maxImageExtent.width, requestedExtent.width )
    );
    requestedExtent.height = std::max(
        capabilities.minImageExtent.height,
        std::min( capabilities.maxImageExtent.height, requestedExtent.height )
    );

    return requestedExtent;
}

