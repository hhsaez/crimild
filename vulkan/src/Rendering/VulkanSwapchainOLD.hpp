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
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_VULKAN_RENDERING_SWAPCHAIN_OLD_
#define CRIMILD_VULKAN_RENDERING_SWAPCHAIN_OLD_

#include "Foundation/Containers/Array.hpp"
#include "Foundation/VulkanObject.hpp"
#include "Mathematics/Vector2.hpp"
#include "Rendering/Swapchain.hpp"

namespace crimild {

    class Image;
    class ImageView;

    namespace vulkan {

        class RenderDevice;
        class VulkanSurface;

        //////////////////////
        // DELETE FROM HERE //
        //////////////////////

        class RenderDeviceOLD;
        class VulkanSurfaceOLD;
        class Semaphore;
        class SwapchainManager;

        class SwapchainOLD : public crimild::Swapchain {
        public:
            struct Descriptor {
                RenderDeviceOLD *renderDevice;
                crimild::Vector2ui extent;
            };

        public:
            ~SwapchainOLD( void ) noexcept;

            RenderDeviceOLD *renderDevice = nullptr;
            VulkanSurfaceOLD *surface = nullptr;
            VkSwapchainKHR handler = VK_NULL_HANDLE;
            VkFormat format;
            VkExtent2D extent;
            SwapchainManager *manager = nullptr;

            struct AcquireImageResult {
                crimild::UInt32 imageIndex;
                crimild::Bool success;
                crimild::Bool outOfDate;
            };
            AcquireImageResult acquireNextImage( const Semaphore *imageAvailableSemaphore ) const noexcept;

            struct PresentImageResult {
                crimild::Bool success;
                crimild::Bool outOfDate;
            };
            PresentImageResult presentImage( crimild::UInt32 imageIndex, const Semaphore *signal ) const noexcept;

            void retrieveSwapchainImages( void ) noexcept;
            void createImageViews( void ) noexcept;
        };

        class SwapchainManager : public VulkanObjectManager< SwapchainOLD > {
        public:
            explicit SwapchainManager( RenderDeviceOLD *renderDevice ) noexcept
                : m_renderDevice( renderDevice ) { }
            virtual ~SwapchainManager( void ) = default;

            SwapchainOLD *getSwapchain( void ) noexcept { return VulkanObjectManager< SwapchainOLD >::first(); }
            const SwapchainOLD *getSwapchain( void ) const noexcept { return VulkanObjectManager< SwapchainOLD >::first(); }

            SharedPointer< SwapchainOLD > create( SwapchainOLD::Descriptor const &descriptor ) noexcept;
            void destroy( SwapchainOLD *swapchain ) noexcept override;

        private:
            VkSurfaceFormatKHR chooseSurfaceFormat( const std::vector< VkSurfaceFormatKHR > &availableFormats ) noexcept;
            VkPresentModeKHR choosePresentationMode( const std::vector< VkPresentModeKHR > &availablePresentModes ) noexcept;
            VkExtent2D chooseExtent( const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D requestedExtent ) noexcept;

        private:
            RenderDeviceOLD *m_renderDevice = nullptr;
        };

    }

}

#endif