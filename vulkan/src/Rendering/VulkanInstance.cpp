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
#include "Debug/VulkanDebugMessenger.hpp"
#include "Foundation/Log.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::vulkan;

//VulkanInstance::VulkanInstance( VkInstance instanceHandler )
//	: m_instanceHandler( instanceHandler )
//{
//	CRIMILD_LOG_TRACE( "Vulkan instance created" );
//
////	createDebugMessenger();
//}
//
VulkanInstance::~VulkanInstance( void )
{
	CRIMILD_LOG_TRACE( "Destroying instance" );
	
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
	if ( handler != VK_NULL_HANDLE ) {
		CRIMILD_LOG_TRACE( "Destroying Vulkan instance" );
		vkDestroyInstance( handler, nullptr );
		handler = VK_NULL_HANDLE;
	}
}

SharedPointer< VulkanDebugMessenger > VulkanInstance::createDebugMessenger( void ) noexcept
{
	if ( !utils::checkValidationLayersEnabled() ) {
		return nullptr;
	}

	CRIMILD_LOG_TRACE( "Setting up vulkan debug messenger" );

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	utils::populateDebugMessengerCreateInfo( createInfo );

    auto createDebugUtilsMessengerEXT = [](
        VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pDebugMessenger ) {
        if ( auto func = ( PFN_vkCreateDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" ) ) {
            return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
        }
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    };

    VkDebugUtilsMessengerEXT debugMessengerHandler = VK_NULL_HANDLE;
	if ( createDebugUtilsMessengerEXT(
		handler,
		&createInfo,
		nullptr,
		&debugMessengerHandler ) != VK_SUCCESS ) {
		CRIMILD_LOG_ERROR( "Failed to setup debug messenger" );
		return nullptr;
	}

    auto ret = crimild::alloc< VulkanDebugMessenger >();
    ret->handler = debugMessengerHandler;
    ret->instance = this;
    return ret;
}

void VulkanInstance::destroyDebugMessenger( VulkanDebugMessenger *debugMessenger ) noexcept
{
	CRIMILD_LOG_TRACE( "Destroying vulkan debug messenger" );
	
	if ( !utils::checkValidationLayersEnabled() ) {
		return;
	}

    auto destroyDebugUtilsMessengerEXT = []( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator ) {
        if ( auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" ) ) {
            func( instance, debugMessenger, pAllocator );
        }
    };

	if ( debugMessenger->handler != VK_NULL_HANDLE ) {
		destroyDebugUtilsMessengerEXT( handler, debugMessenger->handler, nullptr );
		debugMessenger->handler = VK_NULL_HANDLE;
	}
    debugMessenger->instance = nullptr;
    debugMessenger->handler = VK_NULL_HANDLE;
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

