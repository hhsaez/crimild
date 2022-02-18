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
#include "Simulation/Settings.hpp"

namespace crimild {

    namespace vulkan {

        namespace utils {

            crimild::Bool isDeviceSuitable( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept
            {
                CRIMILD_LOG_TRACE();

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
                CRIMILD_LOG_TRACE();

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
    CRIMILD_LOG_TRACE();

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
    CRIMILD_LOG_TRACE();

    // No need to destroy anything. Just reset members
    m_handle = VK_NULL_HANDLE;
    m_surface = nullptr;
}

std::unique_ptr< RenderDevice > PhysicalDevice::createRenderDevice( void ) noexcept
{
    const auto extent = [] {
        auto settings = Settings::getInstance();
        const auto width = settings->get< Real >( "video.width", 1 );
        const auto height = settings->get< Real >( "video.height", 1 );
        const auto scale = settings->get< Real >( "video.framebufferScale", 1 );
        return Extent2D {
            .width = width * scale,
            .height = height * scale,
        };
    }();

    return std::make_unique< RenderDevice >( this, m_surface, extent );
}
