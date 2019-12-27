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
#include "VulkanRenderDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanImage.hpp"
#include "VulkanSemaphore.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool Swapchain::swapchainSupported( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
{
	CRIMILD_LOG_TRACE( "Check swapchain support" );
	
	auto swapchainSupport = querySupportDetails(
		device,
		surface
	);
	
	return !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
}

Swapchain::SwapchainSupportDetails Swapchain::querySupportDetails( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
{
	CRIMILD_LOG_TRACE( "Query swapchain support details" );
	
	SwapchainSupportDetails details;   
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		device,
		surface,
		&details.capabilities
	);
	
	crimild::UInt32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		device,
		surface,
		&formatCount,
		nullptr
	);
	if ( formatCount > 0 ) {
		details.formats.resize( formatCount );
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			device,
			surface,
			&formatCount,
			details.formats.data()
		);
	}
	
	crimild::UInt32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		device,
		surface,
		&presentModeCount,
		nullptr
	);
	if ( presentModeCount > 0 ) {
		details.presentModes.resize( presentModeCount );
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			device,
			surface,
			&presentModeCount,
			details.presentModes.data()
		);
	}
	
	return details;
}

VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat( const std::vector< VkSurfaceFormatKHR > &availableFormats ) noexcept
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

VkPresentModeKHR Swapchain::choosePresentationMode( const std::vector< VkPresentModeKHR > &availablePresentModes ) noexcept
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

VkExtent2D Swapchain::chooseExtent( const VkSurfaceCapabilitiesKHR &capabilities ) noexcept
{
	CRIMILD_LOG_TRACE( "Choosing swapchain extent" );
	
	if ( capabilities.currentExtent.width != std::numeric_limits< uint32_t >::max() ) {
		// Capabilites are enforcing a given extent. Return that one
		return capabilities.currentExtent;
	}

	// If there's no extent limit, compute one based on current settings
	// Keep width/heigth values within the allowed ones, though.

	auto settings = Simulation::getInstance()->getSettings();
	auto width = settings->get< crimild::Int32 >( "video.width", 0 );
	auto height = settings->get< crimild::Int32 >( "video.height", 0 );
	
	VkExtent2D actualExtent = {
		static_cast< uint32_t >( width ),
		static_cast< uint32_t >( height ),
	};
	
	actualExtent.width = std::max(
		capabilities.minImageExtent.width,
		std::min( capabilities.maxImageExtent.width, actualExtent.width )
	);
	actualExtent.height = std::max(
		capabilities.minImageExtent.height,
		std::min( capabilities.maxImageExtent.height, actualExtent.height )
	);
	
	return actualExtent;
}

SharedPointer< Swapchain > Swapchain::create( VulkanRenderDevice *device, VulkanSurface *surface ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating swapchain" );

	const auto &physicalDeviceHandler = device->getPhysicalDeviceHandler();
	const auto &surfaceHandler = surface->handler;
	
	auto swapchainSupport = querySupportDetails( physicalDeviceHandler, surfaceHandler );
	auto surfaceFormat = chooseSurfaceFormat( swapchainSupport.formats );
	auto presentMode = choosePresentationMode( swapchainSupport.presentModes );
	auto extent = chooseExtent( swapchainSupport.capabilities );

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
		.surface = surfaceHandler,
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
	
	auto indices = device->getQueueFamilies();
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
	if ( vkCreateSwapchainKHR(
		device->getDeviceHandler(),
		&createInfo,
		nullptr,
		&swapchainHandler ) != VK_SUCCESS ) {
		CRIMILD_LOG_ERROR( "Failed to create swapchain" );
		return nullptr;
	}

	return crimild::alloc< Swapchain >(
		device,
		surface,
		swapchainHandler,
		surfaceFormat.format,
		extent
	);
}

Swapchain::Swapchain(
	VulkanRenderDevice *device,
	VulkanSurface *surface,
	const VkSwapchainKHR &swapchain,
	const VkFormat &format,
	const VkExtent2D &extent ) noexcept
	: m_device( device ),
	  m_surface( surface ),
	  m_swapchainHandler( swapchain ),
	  m_format( format ),
	  m_extent( extent )
{
	CRIMILD_LOG_TRACE( "Swapchain created" );

	retrieveSwapchainImages();
	createImageViews();
}

Swapchain::~Swapchain( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Destroying swapchain" );

	CRIMILD_LOG_DEBUG( "Destroying image views" );
	m_imageViews.clear();

	CRIMILD_LOG_DEBUG( "Destroying images" );
	m_images.clear();

	if ( m_swapchainHandler != VK_NULL_HANDLE ) {
		vkDestroySwapchainKHR(
			m_device->getDeviceHandler(),
			m_swapchainHandler,
			nullptr
		);
		m_swapchainHandler = VK_NULL_HANDLE;
	}
}

crimild::UInt32 Swapchain::acquireNextImage( const Semaphore *imageAvailableSemaphore ) const noexcept
{
	crimild::UInt32 imageIndex;
	vkAcquireNextImageKHR(
		m_device->getDeviceHandler(),
		m_swapchainHandler,
		std::numeric_limits< uint64_t >::max(), // disable timeout
		imageAvailableSemaphore->getSemaphoreHandler(),
		VK_NULL_HANDLE,
		&imageIndex		
	);

	return imageIndex;
}

void Swapchain::presentImage( crimild::UInt32 imageIndex, const Semaphore *signal ) const noexcept
{
	VkSemaphore signalSemaphores[] = {
		signal->getSemaphoreHandler(),
	};

	VkSwapchainKHR swapchains[] = {
		m_swapchainHandler,
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

	vkQueuePresentKHR(
		m_device->getPresentQueueHandler(),
		&presentInfo
	);
}

void Swapchain::retrieveSwapchainImages( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Retrieving swapchain images" );
	
	crimild::UInt32 imageCount;
	vkGetSwapchainImagesKHR(
		m_device->getDeviceHandler(),
		m_swapchainHandler,
		&imageCount,
		nullptr
	);
	std::vector< VkImage > images( imageCount );
	vkGetSwapchainImagesKHR(
		m_device->getDeviceHandler(),
		m_swapchainHandler,
		&imageCount,
		images.data()
	);

	for ( const auto &imageHandler : images ) {
		// Create images without 
		m_images.add( crimild::alloc< Image >( nullptr, imageHandler ) );
	}
}

void Swapchain::createImageViews( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating image views" );
	
	m_images.each( [ this ]( SharedPointer< Image > &image ) {
		m_imageViews.add(
			m_device->createImageView(
				crimild::get_ptr( image ),
				m_format,
				VK_IMAGE_ASPECT_COLOR_BIT,
				1
			)
		);
	});
}

/*
void cleanupSwapChain( void )
{
	vkDestroyImageView( m_device, m_colorImageView, nullptr );
	vkDestroyImage( m_device, m_colorImage, nullptr );
	vkFreeMemory( m_device, m_colorImageMemory, nullptr );
	
	vkDestroyImageView( m_device, m_depthImageView, nullptr );
	vkDestroyImage( m_device, m_depthImage, nullptr );
	vkFreeMemory( m_device, m_depthImageMemory, nullptr );
	
	for ( auto i = 0l; i < m_swapChainFramebuffers.size(); i++ ) {
		vkDestroyFramebuffer( m_device, m_swapChainFramebuffers[ i ], nullptr );
	}
	
	vkFreeCommandBuffers(
		m_device,
		m_commandPool,
		static_cast< uint32_t >( m_commandBuffers.size() ),
		m_commandBuffers.data()
	);
	
	vkDestroyPipeline( m_device, m_graphicsPipeline, nullptr );
	vkDestroyPipelineLayout( m_device, m_pipelineLayout, nullptr );
	vkDestroyRenderPass( m_device, m_renderPass, nullptr );
	
	for ( auto i = 0l; i < m_swapChainImageViews.size(); ++i ) {
		vkDestroyImageView( m_device, m_swapChainImageViews[ i ], nullptr );
	}
	
	vkDestroySwapchainKHR( m_device, m_swapChain, nullptr );
	
	for ( auto i = 0l; i < m_swapChainImages.size(); ++i ) {
		vkDestroyBuffer( m_device, m_uniformBuffers[ i ], nullptr );
		vkFreeMemory( m_device, m_uniformBuffersMemory[ i ], nullptr );
	}
	
	vkDestroyDescriptorPool( m_device, m_descriptorPool, nullptr );
}

void recreateSwapChain( void )
{
	int width = 0;
	int height = 0;
	while ( width == 0 || height == 0 ) {
		glfwGetFramebufferSize( _window, &width, &height );
		glfwWaitEvents();
	}
	
	vkDeviceWaitIdle( m_device );
	
	cleanupSwapChain();
	
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createColorResources();
	createDepthResources();
	createFramebuffers();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
}
*/


