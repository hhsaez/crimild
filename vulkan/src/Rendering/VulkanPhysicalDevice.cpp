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

#include "Rendering/VulkanPhysicalDevice.hpp"

#include "Rendering/VulkanInstance.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanSurface.hpp"

namespace crimild {

    namespace vulkan {

        namespace utils {

            crimild::Bool isDeviceSuitable( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
            {
                CRIMILD_LOG_TRACE( "Checking device properties" );

                auto indices = utils::findQueueFamilies( device, surface );
                auto extensionsSupported = utils::checkDeviceExtensionSupport( device );
                auto swapchainAdequate = false;
                if ( extensionsSupported ) {
                    swapchainAdequate = utils::checkSwapchainSupport( device, surface );
                }

                VkPhysicalDeviceFeatures supportedFeatures;
                vkGetPhysicalDeviceFeatures( device, &supportedFeatures );

                return indices.isComplete()
                       && extensionsSupported
                       && swapchainAdequate
                       && supportedFeatures.fillModeNonSolid
                       && supportedFeatures.samplerAnisotropy;
            }

            VkPhysicalDevice pickPhysicalDevice( VulkanInstance *instance, VulkanSurface *surface ) noexcept
            {
                CRIMILD_LOG_TRACE( "Picking physical device" );

                crimild::UInt32 deviceCount = 0;
                vkEnumeratePhysicalDevices( instance->getHandle(), &deviceCount, nullptr );
                if ( deviceCount == 0 ) {
                    CRIMILD_LOG_ERROR( "Failed to find GPUs with Vulkan support" );
                    return VK_NULL_HANDLE;
                }

                std::vector< VkPhysicalDevice > devices( deviceCount );
                vkEnumeratePhysicalDevices( instance->getHandle(), &deviceCount, devices.data() );
                for ( const auto &device : devices ) {
                    if ( isDeviceSuitable( device, surface->getHandle() ) ) {
                        VkPhysicalDeviceProperties physicalDeviceProperties;
                        vkGetPhysicalDeviceProperties( device, &physicalDeviceProperties );
                        CRIMILD_LOG_INFO( "Vulkan physical device found: ", physicalDeviceProperties.deviceName );
                        return device;
                    }
                }

                CRIMILD_LOG_ERROR( "Failed to find a suitable GPU" );
                return VK_NULL_HANDLE;
            }

        }

    }

}

using namespace crimild;
using namespace crimild::vulkan;

PhysicalDevice::PhysicalDevice( VulkanInstance *instance, VulkanSurface *surface ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan physical device" );

    m_handle = utils::pickPhysicalDevice( instance, surface );
    if ( m_handle == VK_NULL_HANDLE ) {
        CRIMILD_LOG_FATAL( "Failed to pick physical device" );
        exit( -1 );
    }

    m_surface = surface;
    m_msaaSamples = utils::getMaxUsableSampleCount( m_handle );
}

PhysicalDevice::~PhysicalDevice( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan physical device" );

    // No need to destroy anything. Just reset members
    m_handle = VK_NULL_HANDLE;
    m_surface = nullptr;
}

std::unique_ptr< RenderDevice > PhysicalDevice::createRenderDevice( void ) noexcept
{
    return std::make_unique< RenderDevice >( this, m_surface );
}

//////////////////////
// DELETE FROM HERE //
//////////////////////

PhysicalDeviceOLD::PhysicalDeviceOLD( void )
    : RenderDeviceManager( this )
{
}

PhysicalDeviceOLD::~PhysicalDeviceOLD( void )
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

SharedPointer< PhysicalDeviceOLD > PhysicalDeviceManager::create( PhysicalDeviceOLD::Descriptor const &descriptor ) noexcept
{
    // CRIMILD_LOG_TRACE( "Creating Vulkan physical device" );

    // auto physicalDeviceHandler = pickPhysicalDevice(
    //     descriptor.instance->handler,
    //     descriptor.surface->handler );
    // if ( physicalDeviceHandler == VK_NULL_HANDLE ) {
    //     return nullptr;
    // }

    // auto msaaSamples = utils::getMaxUsableSampleCount( physicalDeviceHandler );

    // auto physicalDevice = crimild::alloc< PhysicalDeviceOLD >();
    // physicalDevice->handler = physicalDeviceHandler;
    // physicalDevice->msaaSamples = msaaSamples;
    // physicalDevice->instance = descriptor.instance;
    // physicalDevice->surface = descriptor.surface;
    // physicalDevice->manager = this;
    // insert( crimild::get_ptr( physicalDevice ) );
    // return physicalDevice;
    return nullptr;
}

void PhysicalDeviceManager::destroy( PhysicalDeviceOLD *physicalDevice ) noexcept
{
    // CRIMILD_LOG_TRACE( "Destroying Vulkan physical device" );

    // static_cast< RenderDeviceManager * >( physicalDevice )->cleanup();

    // // No need to destroy anything. Just reset members
    // physicalDevice->handler = VK_NULL_HANDLE;
    // physicalDevice->instance = nullptr;
    // physicalDevice->surface = nullptr;
    // physicalDevice->manager = nullptr;
    // erase( physicalDevice );
}
