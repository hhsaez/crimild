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
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "VulkanInstance.hpp"
#include "VulkanRenderDevice.hpp"
#include "Foundation/Log.hpp"

using namespace crimild;
using namespace crimild::vulkan;

VulkanInstance::VulkanInstance( void )
    : VulkanDebugMessengerManager( this )
{

}

VulkanInstance::~VulkanInstance( void )
{
	CRIMILD_LOG_TRACE( "Destroying instance" );

    VulkanDebugMessengerManager::cleanup();
	
//	if ( m_renderDevice != nullptr ) {
//		CRIMILD_LOG_TRACE( "Waiting for pending operations" );
//		m_renderDevice->waitIdle();
//	}
//
//	m_renderDevice = nullptr;
//
//	destroyDebugMessenger();
//
//	m_surface = nullptr;
//
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

/*
VulkanRenderDevice *VulkanInstance::createRenderDevice( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan Physical Device" );

    auto physicalDevice = utils::pickPhysicalDevice( m_instanceHandler, m_surface->getInstanceHandler() );
    if ( physicalDevice == VK_NULL_HANDLE ) {
        return nullptr;
    }

    auto logicalDevice = utils::createLogicalDevice( physicalDevice, m_surface->getInstanceHandler() );
    if ( logicalDevice == VK_NULL_HANDLE ) {
        // no need to destroy physical device?
        return nullptr;
    }

    m_renderDevice = crimild::alloc< VulkanRenderDevice >( this, getSurface(), physicalDevice, logicalDevice );
    return crimild::get_ptr( m_renderDevice );
}
 */

SharedPointer< VulkanInstance > VulkanInstanceManager::create( VulkanInstance::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan instance" );

    auto validationLayersEnabled = utils::checkValidationLayersEnabled();
    auto validationLayers = utils::getValidationLayers();
    if ( validationLayersEnabled && !utils::checkValidationLayerSupport( validationLayers ) ) {
        CRIMILD_LOG_ERROR( "Validation layers requested, but not available" );
        return nullptr;
    }

    auto appInfo = VkApplicationInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = descriptor.appName.c_str(),
        .applicationVersion = VK_MAKE_VERSION(
            descriptor.appVersionMajor,
            descriptor.appVersionMinor,
            descriptor.appVersionPatch
        ),
        .pEngineName = "Crimild",
        .engineVersion = VK_MAKE_VERSION(
            CRIMILD_VERSION_MAJOR,
            CRIMILD_VERSION_MINOR,
            CRIMILD_VERSION_PATCH
        ),
    };

    auto extensions = utils::getRequiredExtensions();

    auto createInfo = VkInstanceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = static_cast< crimild::UInt32 >( extensions.size() ),
        .ppEnabledExtensionNames = extensions.data(),
        .enabledLayerCount = 0,
    };

    // Keep reference outside block to it is not automatically destroyed before calling VkCreateInstance
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if ( validationLayersEnabled ) {
        createInfo.enabledLayerCount = static_cast< crimild::UInt32 >( validationLayers.size() );
        createInfo.ppEnabledLayerNames = validationLayers.data();

        // Enable debug messenger specifically for create/destroy instance
        utils::populateDebugMessengerCreateInfo( debugCreateInfo );
        createInfo.pNext = ( VkDebugUtilsMessengerCreateInfoEXT * ) &debugCreateInfo;
    }

    VkInstance instanceHandler;
    if ( vkCreateInstance( &createInfo, nullptr, &instanceHandler ) != VK_SUCCESS ) {
        CRIMILD_LOG_ERROR( "Failed to create Vulkan instance" );
        return nullptr;
    }

    auto instance = crimild::alloc< VulkanInstance >();
    instance->handler = instanceHandler;
    insert( crimild::get_ptr( instance ) );
    return instance;
}

void VulkanInstanceManager::destroy( VulkanInstance *instance ) noexcept
{
    if ( instance->handler != VK_NULL_HANDLE ) {
        CRIMILD_LOG_TRACE( "Destroying Vulkan instance" );
        vkDestroyInstance( instance->handler, nullptr );
        instance->handler = VK_NULL_HANDLE;
    }
}

