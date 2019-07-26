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
 *     * Neither the name of the copyright holder nor the
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

#include "VulkanRenderDevice.hpp"
#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanFence.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "Foundation/Log.hpp"

#include <set>

using namespace crimild;
using namespace crimild::vulkan;

SharedPointer< VulkanRenderDevice > VulkanRenderDevice::create( VulkanInstance *instance, VulkanSurface *surface ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating Vulkan rendering device" );

	auto physicalDevice = pickPhysicalDevice(
		instance->getInstanceHandler(),
		surface->getSurfaceHandler()
	);
	if ( physicalDevice == VK_NULL_HANDLE ) {
		return nullptr;
	}

	auto logicalDevice = createLogicalDevice(
		physicalDevice,
		surface->getSurfaceHandler()
	);
	if ( logicalDevice == VK_NULL_HANDLE ) {
		// No need to destroy the physical device
		return nullptr;
	}

	return crimild::alloc< VulkanRenderDevice >(
		instance,
		surface,
		physicalDevice,
		logicalDevice
	);
}

VkPhysicalDevice VulkanRenderDevice::pickPhysicalDevice( const VkInstance &instance, const VkSurfaceKHR &surface ) noexcept
{
	CRIMILD_LOG_TRACE( "Picking physical device" );

	crimild::UInt32 deviceCount = 0;
	vkEnumeratePhysicalDevices( instance, &deviceCount, nullptr );
	if ( deviceCount == 0 ) {
		CRIMILD_LOG_ERROR( "Failed to find GPUs with Vulkan support" );
		return VK_NULL_HANDLE;
	}

	std::vector< VkPhysicalDevice > devices( deviceCount );
	vkEnumeratePhysicalDevices( instance, &deviceCount, devices.data() );
	for ( const auto &device : devices ) {
		if ( isDeviceSuitable( device, surface ) ) {
			CRIMILD_LOG_INFO( "Vulkan physical device found" );
			return device;
		}
	}
	
	CRIMILD_LOG_ERROR( "Failed to find a suitable GPU" );
	return VK_NULL_HANDLE;
}

crimild::Bool VulkanRenderDevice::isDeviceSuitable( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
{
	CRIMILD_LOG_TRACE( "Checking device properties" );
	
	auto indices = findQueueFamilies( device, surface );
	auto extensionsSupported = checkDeviceExtensionSupport( device );

	auto swapChainAdequate = false;
	if ( extensionsSupported ) {
		swapChainAdequate = Swapchain::swapchainSupported( device, surface );
	}
	
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures( device, &supportedFeatures );
	
	return indices.isComplete()
		&& extensionsSupported
		&& swapChainAdequate
		&& supportedFeatures.samplerAnisotropy;
}

/**
   \brief Check if a given device met all required extensions
*/
crimild::Bool VulkanRenderDevice::checkDeviceExtensionSupport( const VkPhysicalDevice &device ) noexcept
{
	CRIMILD_LOG_TRACE( "Checking device extension support" );

	auto &deviceExtensions = getDeviceExtensions();

	crimild::UInt32 extensionCount;
	vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );
	
	std::vector< VkExtensionProperties > availableExtensions( extensionCount );
	vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions.data() );

	std::set< std::string > requiredExtensions(
		std::begin( deviceExtensions ),
		std::end( deviceExtensions )
	);

	for ( const auto &extension : availableExtensions ) {
		requiredExtensions.erase( extension.extensionName );
	}
	
	if ( !requiredExtensions.empty() ) {
		std::stringstream ss;
		for ( const auto &name : requiredExtensions ) {
			ss << "\n\t" << name;
		}
		CRIMILD_LOG_ERROR( "Required extensions not met: ", ss.str() );
		return false;
	}
	
	CRIMILD_LOG_DEBUG( "All required extensions met" );
	
	return true;
}

VulkanRenderDevice::QueueFamilyIndices VulkanRenderDevice::findQueueFamilies( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
{
	CRIMILD_LOG_TRACE( "Finding device queue families" );

	QueueFamilyIndices indices;
	
	crimild::UInt32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );
	if ( queueFamilyCount == 0 ) {
		CRIMILD_LOG_ERROR( "No queue family found for device" );
		return indices;
	}
	
	std::vector< VkQueueFamilyProperties > queueFamilies( queueFamilyCount );
	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.data() );
	
	crimild::UInt32 i = 0;
	for ( const auto &queueFamily : queueFamilies ) {

		if ( queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
			indices.graphicsFamily.push_back( i );
		}

		// Find a queue family that supports presenting to the Vulkan surface
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface, &presentSupport );
		if ( queueFamily.queueCount > 0 && presentSupport ) {
			// This might probably be same as the graphics queue, though.
			indices.presentFamily.push_back( i );
		}
		
		if ( indices.isComplete() ) {
			break;
		}
		
		i++;
	}
	
	return indices;
}

VkDevice VulkanRenderDevice::createLogicalDevice( const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating vulkan logical device" );
	
	QueueFamilyIndices indices = findQueueFamilies( physicalDevice, surface );
	if ( !indices.isComplete() ) {
		// should never happen
		CRIMILD_LOG_ERROR( "Invalid physical device" );
		return VK_NULL_HANDLE;
	}

	// Make sure we're creating queues for unique families,
	// since both graphics and presentation might be same family
	// \see findQueueFamilies()
	std::set< crimild::UInt32 > uniqueQueueFamilies = {
		indices.graphicsFamily[ 0 ],
		indices.presentFamily[ 0 ],
	};

	// Required even if there's only one queue
	auto queuePriority = 1.0f;
	
	std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;
	for ( auto queueFamily : uniqueQueueFamilies ) {
		auto queueCreateInfo = VkDeviceQueueCreateInfo {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queueFamily,
			.queueCount = 1,
			.pQueuePriorities = &queuePriority,
		};
		queueCreateInfos.push_back( queueCreateInfo );
	}
	
	VkPhysicalDeviceFeatures deviceFeatures = {
		.samplerAnisotropy = VK_TRUE,
	};

	auto &deviceExtensions = getDeviceExtensions();
	
	VkDeviceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = static_cast< crimild::UInt32 >( queueCreateInfos.size() ),
		.pQueueCreateInfos = queueCreateInfos.data(),
		.pEnabledFeatures = &deviceFeatures,
		.enabledLayerCount = 0,
		.enabledExtensionCount = static_cast< crimild::UInt32 >( deviceExtensions.size() ),
		.ppEnabledExtensionNames = deviceExtensions.data(),
	};
	
	if ( VulkanInstance::enableValidationLayers() ) {
		// New Vulkan implementations seem to be ignoring validation layers per device
		// Still, it might be a good idea to register them here.
		auto &validationLayers = VulkanInstance::getValidationLayers();
		createInfo.enabledLayerCount = static_cast< crimild::UInt32 >( validationLayers.size() );
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}

	VkDevice device;
	if ( vkCreateDevice( physicalDevice, &createInfo, nullptr, &device ) != VK_SUCCESS ) {
		CRIMILD_LOG_ERROR( "Failed to create logical device" );
		return VK_NULL_HANDLE;
	}

	return device;
}

VulkanRenderDevice::VulkanRenderDevice( VulkanInstance *instance, VulkanSurface *surface, const VkPhysicalDevice &physicalDevice, const VkDevice &device )
	: m_instance( instance ),
	  m_surface( surface ),
	  m_physicalDevice( physicalDevice ),
	  m_device( device )
{
	m_msaaSamples = getMaxUsableSampleCount();

	QueueFamilyIndices indices = findQueueFamilies( physicalDevice, surface->getSurfaceHandler() );
	
	// Get queue handles
	vkGetDeviceQueue( m_device, indices.graphicsFamily[ 0 ], 0, &m_graphicsQueue );
	vkGetDeviceQueue( m_device, indices.presentFamily[ 0 ], 0, &m_presentQueue );
}

VulkanRenderDevice::~VulkanRenderDevice( void )
{
	m_swapchain = nullptr;
	
	vkDestroyDevice( m_device, nullptr );
	m_device = VK_NULL_HANDLE;
}

VkSampleCountFlagBits VulkanRenderDevice::getMaxUsableSampleCount( void ) const noexcept
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties( m_physicalDevice, &physicalDeviceProperties );
	
	auto counts = std::min(
		physicalDeviceProperties.limits.framebufferColorSampleCounts,
		physicalDeviceProperties.limits.framebufferDepthSampleCounts
	);
	
	if ( counts & VK_SAMPLE_COUNT_64_BIT ) return VK_SAMPLE_COUNT_64_BIT;
	if ( counts & VK_SAMPLE_COUNT_32_BIT ) return VK_SAMPLE_COUNT_32_BIT;
	if ( counts & VK_SAMPLE_COUNT_16_BIT ) return VK_SAMPLE_COUNT_16_BIT;
	if ( counts & VK_SAMPLE_COUNT_8_BIT ) return VK_SAMPLE_COUNT_8_BIT;
	if ( counts & VK_SAMPLE_COUNT_4_BIT ) return VK_SAMPLE_COUNT_4_BIT;
	if ( counts & VK_SAMPLE_COUNT_2_BIT ) return VK_SAMPLE_COUNT_2_BIT;
	return VK_SAMPLE_COUNT_1_BIT;
}

VulkanRenderDevice::QueueFamilyIndices VulkanRenderDevice::getQueueFamilies( void ) const noexcept
{
	return findQueueFamilies(
		m_physicalDevice,
		m_surface->getSurfaceHandler()
	);
}

void VulkanRenderDevice::waitIdle( void ) const noexcept
{
	if ( m_device == VK_NULL_HANDLE ) {
		return;
	}

	vkDeviceWaitIdle( m_device );
}

SharedPointer< Semaphore > VulkanRenderDevice::createSemaphore( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating semaphore" );
	
	auto semaphoreInfo = VkSemaphoreCreateInfo {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};

	VkSemaphore semaphoreHandler;
	if ( vkCreateSemaphore( m_device, &semaphoreInfo, nullptr, &semaphoreHandler ) != VK_SUCCESS ) {
		CRIMILD_LOG_ERROR( "Failed to create semaphore" );
		return nullptr;
	}

	return crimild::alloc< Semaphore >( this, semaphoreHandler );
}

SharedPointer< Fence > VulkanRenderDevice::createFence( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating fence" );
	
	auto fenceInfo = VkFenceCreateInfo {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};

	VkFence fenceHandler;
	if ( vkCreateFence( m_device, &fenceInfo, nullptr, &fenceHandler ) != VK_SUCCESS ) {
		CRIMILD_LOG_ERROR( "Failed to create fence" );
		return nullptr;
	}

	return crimild::alloc< Fence >( this, fenceHandler );
}

SharedPointer< Image > VulkanRenderDevice::createImage( void )
{
	return nullptr;
}

SharedPointer< ImageView > VulkanRenderDevice::createImageView( Image *image, VkFormat format, VkImageAspectFlags aspectFlags, crimild::UInt32 mipLevels )
{
	return crimild::alloc< ImageView >(
		this,
		crimild::retain( image ),
		format,
		aspectFlags,
		mipLevels
	);
}

