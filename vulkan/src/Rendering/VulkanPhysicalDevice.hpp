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

#ifndef CRIMILD_VULKAN_RENDERING_PHYSICAL_DEVICE_
#define CRIMILD_VULKAN_RENDERING_PHYSICAL_DEVICE_

#include "Foundation/VulkanObject.hpp"      // TODO: Remove this?
#include "Rendering/VulkanRenderDevice.hpp" // TODO: Remove this?

namespace crimild {

    namespace vulkan {

        class VulkanInstance;
        class VulkanSurface;
        class RenderDevice;

        class PhysicalDevice {
        public:
            PhysicalDevice( VulkanInstance *instance, VulkanSurface *surface ) noexcept;
            ~PhysicalDevice( void ) noexcept;

            [[nodiscard]] inline const VkPhysicalDevice &getHandle( void ) const noexcept { return m_handle; }

            [[nodiscard]] std::unique_ptr< RenderDevice > createRenderDevice( void ) noexcept;

        private:
            VkPhysicalDevice m_handle = VK_NULL_HANDLE;
            VulkanSurface *m_surface = nullptr;
            VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        };

        //////////////////////
        // DELETE FROM HERE //
        //////////////////////

        class VulkanInstanceOLD;
        class VulkanSurfaceOLD;
        class PhysicalDeviceManager;

        class [[deprecated]] PhysicalDeviceOLD : public VulkanObject, public RenderDeviceManager
        {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::PhysicalDeviceOLD )
        public:
            using RenderDeviceManager::create;

            struct Descriptor {
                VulkanInstanceOLD *instance;
                VulkanSurfaceOLD *surface;
            };

        public:
            PhysicalDeviceOLD( void );
            ~PhysicalDeviceOLD( void );

            VkPhysicalDevice handler = VK_NULL_HANDLE;
            VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
            VulkanInstanceOLD *instance = nullptr;
            VulkanSurfaceOLD *surface = nullptr;
            PhysicalDeviceManager *manager = nullptr;
        };

        // TODO: How to handle an optional surface param?
        class PhysicalDeviceManager : public VulkanObjectManager< PhysicalDeviceOLD > {
        public:
            virtual ~PhysicalDeviceManager( void ) = default;

            SharedPointer< PhysicalDeviceOLD > create( PhysicalDeviceOLD::Descriptor const &descriptor ) noexcept;
            void destroy( PhysicalDeviceOLD *physicalDevice ) noexcept override;

        private:
            VkPhysicalDevice pickPhysicalDevice( const VkInstance &instance, const VkSurfaceKHR &surface ) noexcept;
            crimild::Bool isDeviceSuitable( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept;
        };

    }

}

#endif
