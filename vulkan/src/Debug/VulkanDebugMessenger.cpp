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

#include "VulkanDebugMessenger.hpp"
#include "Rendering/VulkanInstance.hpp"

using namespace crimild;
using namespace crimild::vulkan;

VulkanDebugMessenger::~VulkanDebugMessenger( void )
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

SharedPointer< VulkanDebugMessenger > VulkanDebugMessengerManager::create( VulkanDebugMessenger::Descriptor const &descriptor ) noexcept
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
        descriptor.instance->handler,
        &createInfo,
        nullptr,
        &debugMessengerHandler ) != VK_SUCCESS ) {
        CRIMILD_LOG_ERROR( "Failed to setup debug messenger" );
        return nullptr;
    }

    auto debugMessenger = crimild::alloc< VulkanDebugMessenger >();
    debugMessenger->handler = debugMessengerHandler;
    debugMessenger->instance = descriptor.instance;
    debugMessenger->manager = this;
    insert( crimild::get_ptr( debugMessenger ) );
    return debugMessenger;
}

void VulkanDebugMessengerManager::destroy( VulkanDebugMessenger *debugMessenger ) noexcept
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
        destroyDebugUtilsMessengerEXT( debugMessenger->instance->handler, debugMessenger->handler, nullptr );
        debugMessenger->handler = VK_NULL_HANDLE;
    }
    debugMessenger->instance = nullptr;
    debugMessenger->manager = nullptr;
    debugMessenger->handler = VK_NULL_HANDLE;
    erase( debugMessenger );
}

