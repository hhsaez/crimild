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
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "VulkanSystem.hpp"

#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool VulkanSystem::start( void )
{
#if defined( CRIMILD_DEBUG )
	m_enableValidationLayers = true;
	m_validationLayers.push_back( "VK_LAYER_LUNARG_standard_validation" );
#endif
	
	return System::start() &&
		createInstance() &&
		createDebugMessenger();
}

void VulkanSystem::stop( void )
{
	System::stop();

	destroyDebugMessenger();
	destroyInstance();
}

crimild::Bool VulkanSystem::createInstance( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Creating Vulkan instance" );

	if ( m_enableValidationLayers && !checkValidationLayerSupport( m_validationLayers ) ) {
		CRIMILD_LOG_ERROR( "Validation layers requested, but not available" );
		return false;
	}
	
	auto settings = Simulation::getInstance()->getSettings();
	auto appName = settings->get< std::string >( Settings::SETTINGS_APP_NAME, "Crimild" );
	auto appVersionMajor = settings->get< crimild::UInt32 >( Settings::SETTINGS_APP_VERSION_MAJOR, 1 );
	auto appVersionMinor = settings->get< crimild::UInt32 >( Settings::SETTINGS_APP_VERSION_MINOR, 0 );
	auto appVersionPatch = settings->get< crimild::UInt32 >( Settings::SETTINGS_APP_VERSION_PATCH, 0 );

	auto appInfo = VkApplicationInfo {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = appName.c_str(),
		.applicationVersion = VK_MAKE_VERSION( appVersionMajor, appVersionMinor, appVersionPatch ),
		.pEngineName = "Crimild",
		.engineVersion = VK_MAKE_VERSION( CRIMILD_VERSION_MAJOR, CRIMILD_VERSION_MINOR, CRIMILD_VERSION_PATCH ),
		.apiVersion = VK_API_VERSION_1_0,
	};

	auto extensions = getRequiredExtensions();

	auto createInfo = VkInstanceCreateInfo {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = static_cast< crimild::UInt32 >( extensions.size() ),
		.ppEnabledExtensionNames = extensions.data(),
		.enabledLayerCount = 0,
	};

	// Keep it outside the block so it's not destroyed before calling vkCreateInstance
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if ( m_enableValidationLayers ) {
		createInfo.enabledLayerCount = static_cast< crimild::UInt32 >( m_validationLayers.size() );
		createInfo.ppEnabledLayerNames = m_validationLayers.data();

		// Enable debug messenger specifically for create/destroy instance
		populateDebugMessengerCreateInfo( debugCreateInfo );
		createInfo.pNext = ( VkDebugUtilsMessengerCreateInfoEXT * ) &debugCreateInfo;
	}

	if ( vkCreateInstance( &createInfo, nullptr, &m_instance ) != VK_SUCCESS ) {
		CRIMILD_LOG_ERROR( "Failed to create Vulkan instance" );
		return false;
	}

	return true;
}

crimild::Bool VulkanSystem::checkValidationLayerSupport( const ValidationLayerArray &validationLayers ) const noexcept
{
	CRIMILD_LOG_TRACE( "Checking validation layer support" );
	
	crimild::UInt32 layerCount;
	vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

	std::vector< VkLayerProperties > availableLayers( layerCount );
	vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

	for ( const auto layerName : validationLayers ) {
		auto layerFound = false;
		for ( const auto &layerProperites : availableLayers ) {
			if ( strcmp( layerName, layerProperites.layerName ) == 0 ) {
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

VulkanSystem::ExtensionArray VulkanSystem::getRequiredExtensions( void ) const noexcept
{
	CRIMILD_LOG_TRACE( "Getting required extensions" );
	
	if ( m_enableValidationLayers ) {
		// list all available extensions
		crimild::UInt32 extensionCount = 0;
		vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );
		std::vector< VkExtensionProperties > extensions( extensionCount );
		vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensions.data() );
		for ( const auto &extension : extensions ) {
			CRIMILD_LOG_DEBUG( "Found extension: ", extension.extensionName );
		}
	}

	auto extensions = ExtensionArray {
		VK_KHR_SURFACE_EXTENSION_NAME,
	};

#if defined( CRIMILD_PLATFORM_OSX )
	// TODO: no macro for platform extensions?
	extensions.push_back( "VK_MVK_macos_surface" );
#endif

	if ( m_enableValidationLayers ) {
		extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
	}

	return extensions;
}

void VulkanSystem::destroyInstance( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Destroying Vulkan instance" );
	
	vkDestroyInstance( m_instance, nullptr );
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanSystem::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
	void *pUserData ) noexcept
{
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

VkResult VulkanSystem::createDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
	const VkAllocationCallbacks *pAllocator,
	VkDebugUtilsMessengerEXT *pDebugMessenger ) noexcept
{
	auto func = ( PFN_vkCreateDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );
	if ( func != nullptr ) {
		return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VulkanSystem::destroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks *pAllocator ) noexcept
{
	auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT ) vkGetInstanceProcAddr(
		instance,
		"vkDestroyDebugUtilsMessengerEXT" );
	if ( func != nullptr ) {
		func( instance, debugMessenger, pAllocator );
	}
}

void VulkanSystem::populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept
{
	createInfo = VkDebugUtilsMessengerCreateInfoEXT {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = debugCallback,
		.pUserData = nullptr,
	};
}

crimild::Bool VulkanSystem::createDebugMessenger( void ) noexcept
{
	if ( !m_enableValidationLayers ) {
		return true;
	}

	CRIMILD_LOG_TRACE( "Setting up debug messenger" );

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo( createInfo );
	
	if ( createDebugUtilsMessengerEXT( m_instance, &createInfo, nullptr, &m_debugMessenger ) != VK_SUCCESS ) {
		CRIMILD_LOG_ERROR( "Failed to setup debug messenger" );
		return false;
	}

	return true;
}

void VulkanSystem::destroyDebugMessenger( void ) noexcept
{
	if ( m_enableValidationLayers ) {
		destroyDebugUtilsMessengerEXT( m_instance, m_debugMessenger, nullptr );
	}
}

