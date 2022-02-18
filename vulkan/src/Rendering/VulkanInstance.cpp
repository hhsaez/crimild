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

#include "Rendering/VulkanInstance.hpp"

#include "Foundation/Log.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Simulation/Settings.hpp"
#include "VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

VulkanInstance::VulkanInstance( void ) noexcept
{
    createInstance();
    createDebugMessenger();
    createReportCallback();
}

VulkanInstance::~VulkanInstance( void ) noexcept
{
    destroyReportCallback();
    destroyDebugMessenger();
    destroyInstance();
}

void VulkanInstance::createInstance( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    auto validationLayersEnabled = utils::checkValidationLayersEnabled();
    auto validationLayers = utils::getValidationLayers();
    if ( validationLayersEnabled && !utils::checkValidationLayerSupport( validationLayers ) ) {
        CRIMILD_LOG_FATAL( "Validation layers requested, but not available" );
        exit( -1 );
    }

    auto settings = Settings::getInstance();
    auto appName = settings->get< std::string >( Settings::SETTINGS_APP_NAME, "Crimild" );
    auto appVersionMajor = settings->get< crimild::UInt32 >( Settings::SETTINGS_APP_VERSION_MAJOR, 1 );
    auto appVersionMinor = settings->get< crimild::UInt32 >( Settings::SETTINGS_APP_VERSION_MINOR, 0 );
    auto appVersionPatch = settings->get< crimild::UInt32 >( Settings::SETTINGS_APP_VERSION_PATCH, 0 );

    auto appInfo = VkApplicationInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = appName.c_str(),
        .applicationVersion = VK_MAKE_VERSION(
            appVersionMajor,
            appVersionMinor,
            appVersionPatch ),
        .pEngineName = "Crimild",
        .engineVersion = VK_MAKE_VERSION(
            CRIMILD_VERSION_MAJOR,
            CRIMILD_VERSION_MINOR,
            CRIMILD_VERSION_PATCH ),
    };

    auto extensions = utils::getRequiredExtensions();

    auto createInfo = VkInstanceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .enabledExtensionCount = static_cast< crimild::UInt32 >( extensions.size() ),
        .ppEnabledExtensionNames = extensions.data(),
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

    if ( vkCreateInstance( &createInfo, nullptr, &m_instanceHandle ) != VK_SUCCESS ) {
        CRIMILD_LOG_FATAL( "Failed to create Vulkan instance" );
        exit( -1 );
    }

    CRIMILD_LOG_INFO( "VulkanInstance created" );
}

void VulkanInstance::destroyInstance( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    if ( m_instanceHandle != VK_NULL_HANDLE ) {
        vkDestroyInstance( m_instanceHandle, nullptr );
        m_instanceHandle = VK_NULL_HANDLE;
    }

    CRIMILD_LOG_INFO( "VulkanInstance destroyed" );
}

void VulkanInstance::createDebugMessenger( void ) noexcept
{
    if ( !utils::checkValidationLayersEnabled() ) {
        return;
    }

    CRIMILD_LOG_TRACE();

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

    if ( createDebugUtilsMessengerEXT(
             m_instanceHandle,
             &createInfo,
             nullptr,
             &m_debugMessengerHandle )
         != VK_SUCCESS ) {
        CRIMILD_LOG_ERROR( "Failed to setup debug messenger" );
        return;
    }
}

void VulkanInstance::destroyDebugMessenger( void ) noexcept
{
    if ( m_debugMessengerHandle == VK_NULL_HANDLE ) {
        return;
    }

    CRIMILD_LOG_TRACE();

    auto destroyDebugUtilsMessengerEXT = []( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator ) {
        if ( auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" ) ) {
            func( instance, debugMessenger, pAllocator );
        }
    };

    destroyDebugUtilsMessengerEXT( m_instanceHandle, m_debugMessengerHandle, nullptr );
    m_debugMessengerHandle = VK_NULL_HANDLE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL crimild_vulkan_report_callback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char *pLayerPrefix,
    const char *pMessage,
    void *userData )
{
    if ( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) {
        // Performance warnings are silenced to make debug output more readable
        return VK_FALSE;
    }
    CRIMILD_LOG_DEBUG( "Vulkan debug callback (", pLayerPrefix, "): ", pMessage );
    return VK_FALSE;
}

void VulkanInstance::createReportCallback( void ) noexcept
{
    if ( !utils::checkValidationLayersEnabled() ) {
        return;
    }

    CRIMILD_LOG_TRACE();

    auto createInfo = VkDebugReportCallbackCreateInfoEXT {
        .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT,
        .pfnCallback = &crimild_vulkan_report_callback,
        .pUserData = nullptr
    };

    auto createDebugReportCallbackEXT =
        [](
            VkInstance instance,
            const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
            const VkAllocationCallbacks *pAllocator,
            VkDebugReportCallbackEXT *pHandler ) {
            if ( auto func = ( PFN_vkCreateDebugReportCallbackEXT ) vkGetInstanceProcAddr( instance, "vkCreateDebugReportCallbackEXT" ) ) {
                return func( instance, pCreateInfo, pAllocator, pHandler );
            }
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        };

    CRIMILD_VULKAN_CHECK(
        createDebugReportCallbackEXT(
            m_instanceHandle,
            &createInfo,
            nullptr,
            &m_reportCallbackHandle ) );
}

void VulkanInstance::destroyReportCallback( void ) noexcept
{
    if ( m_reportCallbackHandle == VK_NULL_HANDLE ) {
        return;
    }

    CRIMILD_LOG_TRACE();

    auto destroyDebugReportCallbackEXT = []( VkInstance instance, VkDebugReportCallbackEXT handler, const VkAllocationCallbacks *pAllocator ) {
        if ( auto func = ( PFN_vkDestroyDebugReportCallbackEXT ) vkGetInstanceProcAddr( instance, "vkDestroyDebugReportCallbackEXT" ) ) {
            func( instance, handler, pAllocator );
        }
    };

    destroyDebugReportCallbackEXT( m_instanceHandle, m_reportCallbackHandle, nullptr );
    m_reportCallbackHandle = VK_NULL_HANDLE;
}

std::unique_ptr< PhysicalDevice > VulkanInstance::createPhysicalDevice( VulkanSurface *surface ) noexcept
{
    return std::make_unique< PhysicalDevice >( this, surface );
}

//////////////////////
// DELETE FROM HERE //
//////////////////////

VulkanInstanceOLD::~VulkanInstanceOLD( void )
{
    CRIMILD_LOG_TRACE();

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

SharedPointer< VulkanInstanceOLD > VulkanInstanceManager::create( VulkanInstanceOLD::Descriptor const &descriptor ) noexcept
{
    return nullptr;
}

void VulkanInstanceManager::destroy( VulkanInstanceOLD *instance ) noexcept
{
}
