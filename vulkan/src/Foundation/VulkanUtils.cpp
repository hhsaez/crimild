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

#include "VulkanUtils.hpp"

#include <set>

using namespace crimild;
using namespace crimild::vulkan;

#define CRIMILD_VULKAN_ERROR_STRING( x ) case static_cast< int >( x ): return #x

const char *utils::errorToString( VkResult result ) noexcept
{
	switch ( result ) {
		CRIMILD_VULKAN_ERROR_STRING( VK_SUCCESS );
		CRIMILD_VULKAN_ERROR_STRING( VK_NOT_READY );
		CRIMILD_VULKAN_ERROR_STRING( VK_TIMEOUT );
		CRIMILD_VULKAN_ERROR_STRING( VK_EVENT_SET );
		CRIMILD_VULKAN_ERROR_STRING( VK_EVENT_RESET );
		CRIMILD_VULKAN_ERROR_STRING( VK_INCOMPLETE );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_OUT_OF_HOST_MEMORY );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_OUT_OF_DEVICE_MEMORY );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_INITIALIZATION_FAILED );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_DEVICE_LOST );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_MEMORY_MAP_FAILED );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_LAYER_NOT_PRESENT );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_EXTENSION_NOT_PRESENT );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_FEATURE_NOT_PRESENT );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_INCOMPATIBLE_DRIVER );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_TOO_MANY_OBJECTS );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_FORMAT_NOT_SUPPORTED );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_SURFACE_LOST_KHR );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_NATIVE_WINDOW_IN_USE_KHR );
		CRIMILD_VULKAN_ERROR_STRING( VK_SUBOPTIMAL_KHR );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_OUT_OF_DATE_KHR );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_INCOMPATIBLE_DISPLAY_KHR );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_VALIDATION_FAILED_EXT );
		CRIMILD_VULKAN_ERROR_STRING( VK_ERROR_INVALID_SHADER_NV );
		CRIMILD_VULKAN_ERROR_STRING( VK_RESULT_BEGIN_RANGE );
		CRIMILD_VULKAN_ERROR_STRING( VK_RESULT_RANGE_SIZE );
		default: return "UNKNOWN";
	};	
}

crimild::Bool utils::checkValidationLayersEnabled( void ) noexcept
{
#if defined( CRIMILD_DEBUG )
	return true;
#else
    return false;
#endif
}

const utils::ValidationLayerArray &utils::getValidationLayers( void ) noexcept
{
    static ValidationLayerArray validationLayers = {
        "VK_LAYER_LUNARG_standard_validation",
    };
    return validationLayers;
}

crimild::Bool utils::checkValidationLayerSupport( const utils::ValidationLayerArray &validationLayers ) noexcept
{
    CRIMILD_LOG_TRACE( "Checking validation layer support" );

    crimild::UInt32 layerCount;
    vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

    std::vector< VkLayerProperties > availableLayers( layerCount );
    vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

    for ( const auto &layerName : validationLayers ) {
        auto layerFound = false;
        for ( const auto &layerProperties : availableLayers ) {
            if ( strcmp( layerName, layerProperties.layerName ) == 0 ) {
                CRIMILD_LOG_DEBUG( "Found validation layer: ", layerName );
                layerFound = true;
                break;
            }
        }

        if ( !layerFound ) {
            CRIMILD_LOG_ERROR( "Validation layer not found: ", layerName );
            return false;
        }
    }

    return true;
}

utils::ExtensionArray utils::getRequiredExtensions( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Getting required extensions" );

    auto validationLayersEnabled = checkValidationLayersEnabled();

    if ( validationLayersEnabled ) {
        // list all available extensions
        crimild::UInt32 extensionCount = 0;
        vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );
        std::vector< VkExtensionProperties > extensions( extensionCount );
        vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensions.data() );
        for ( const auto &extension : extensions ) {
            CRIMILD_LOG_DEBUG( "Found extension: ", extension.extensionName );
        }
    }

    // Add extensions to create a presentation surface
    // TODO: It would be great to support "headless" Vulkan in the future. Not
    // only for compute operations, but also for testing. This is definitely something
    // I should try
    auto extensions = ExtensionArray {
        VK_KHR_SURFACE_EXTENSION_NAME,
    };

#if defined( CRIMILD_PLATFORM_OSX )
    // TODO: no macro for platform extensions?
    extensions.push_back( "VK_MVK_macos_surface" );
#endif

    if ( validationLayersEnabled ) {
        extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
    }

    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL crimild_vulkan_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
	void *pUserData )
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

void utils::populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
    	| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
    	| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = crimild_vulkan_debug_callback;
    createInfo.pUserData = nullptr;
}

const utils::ExtensionArray &utils::getDeviceExtensions( void ) noexcept
{
    static ExtensionArray deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    return deviceExtensions;
}

crimild::Bool utils::checkDeviceExtensionSupport( const VkPhysicalDevice &device ) noexcept
{
    CRIMILD_LOG_TRACE( "Checking device extension support" );

    const auto &deviceExtensions = getDeviceExtensions();

    crimild::UInt32 extensionCount;
    vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );
    std::vector< VkExtensionProperties > availableExtensions( extensionCount );
    vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions.data() );

    std::set< std::string > requiredExtensions( std::begin( deviceExtensions ), std::end( deviceExtensions ) );

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

utils::QueueFamilyIndices utils::findQueueFamilies( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
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

        // Find a queue family that supports presenting to the VUlkan surface
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface, &presentSupport );
        if ( queueFamily.queueCount > 0 && presentSupport ) {
            // This might probably be same as the graphics queue in most cases
            indices.presentFamily.push_back( i );
        }

        if ( indices.isComplete() ) {
            break;
        }

        ++i;
    }

    return indices;
}

utils::SwapchainSupportDetails utils::querySwapchainSupportDetails( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
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

crimild::Bool utils::checkSwapchainSupport( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
{
    CRIMILD_LOG_TRACE( "Check swapchain support" );

    auto swapchainSupport = querySwapchainSupportDetails(
        device,
        surface
    );

    return !swapchainSupport.formats.empty()
    	&& !swapchainSupport.presentModes.empty();
}

