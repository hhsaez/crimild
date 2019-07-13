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

using namespace crimild;
using namespace crimild::vulkan;

#if 0

VkSurfaceFormatKHR chooseSwapSurfaceFormat( const std::vector< VkSurfaceFormatKHR > &availableFormats ) const noexcept
{
	if ( availableFormats.size() == 1 && availableFormats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
		return {
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		};
	}
	
	for ( const auto &availableFormat : availableFormats ) {
		if ( availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
			return availableFormat;
		}
	}
	
	return availableFormats[ 0 ];
}

VkPresentModeKHR chooseSwapPresentMode( const std::vector< VkPresentModeKHR > &availablePresentModes ) const noexcept
{
	// VSync by default, but may introduce latency
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

VkExtent2D chooseSwapExtent( const VkSurfaceCapabilitiesKHR capabilities )
{
	if ( capabilities.currentExtent.width != std::numeric_limits< uint32_t >::max() ) {
		// Capabilites are enforcing a given extent. Return that one
		return capabilities.currentExtent;
	}

	// If there's no extent limit, compute one based on current settings
	// Keep width/heigth values within the allowed ones, though.

	auto settings = Simulation::getInstance()->getSettings();
	auto width = settings->get< crimild::Int32 >( "video.width", 0 );
	auto height = settings->get< crimild::Int32 >( "video.height", 0 );
	
	//int width, height;
	//glfwGetFramebufferSize( _window, &width, &height );
	
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

crimild::Bool createSwapChain( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating swapchain" );
	
	auto swapChainSupport = querySwapChainSupport( m_physicalDevice );
	auto surfaceFormat = chooseSwapSurfaceFormat( swapChainSupport.formats );
	auto presentMode = chooseSwapPresentMode( swapChainSupport.presentModes );
	auto extent = chooseSwapExtent( swapChainSupport.capabilities );

	// The Vulkan implementation defines a minimum number of images to work with.
	// We request one more image than the minimum. Also, make sure not to exceed the
	// maximum number of images (a value of 0 means there's no maximum)
	auto imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if ( swapChainSupport.capabilities.maxImageCount > 0 ) {
		imageCount = std::min(
			swapChainSupport.capabilities.maxImageCount,
			imageCount
		);
	}
	
	auto createInfo = VkSwapchainCreateInfoKHR {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = _surface,
		.minImageCount = imageCount,
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1, // This may change for stereoscopic rendering (VR)
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	};
	
	auto indices = findQueueFamilies( m_physicalDevice );
	uint32_t queueFamilyIndices[] = {
		indices.graphicsFamily[ 0 ],
		indices.presentFamily[ 0 ],
	};
	
	if ( indices.graphicsFamily != indices.presentFamily ) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	// This can be used to rotate images or flip them horizontally
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	// Use for blending with other windows in the window system
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	
	createInfo.presentMode = presentMode;

	// Ignore pixels that are obscured by other windows
	createInfo.clipped = VK_TRUE;

	// This is used during the recreation of a swapchain (maybe due to window is resized)
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	
	if ( vkCreateSwapchainKHR( m_device, &createInfo, nullptr, &m_swapChain ) != VK_SUCCESS ) {
		CRIMILD_LOG_ERROR( "Failed to create swapchain" );
		return false;
	}
	
	vkGetSwapchainImagesKHR( m_device, m_swapChain, &imageCount, nullptr );
	m_swapChainImages.resize( imageCount );
	vkGetSwapchainImagesKHR( m_device, m_swapChain, &imageCount, m_swapChainImages.data() );
	
	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;

	return true;
}

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

#endif

