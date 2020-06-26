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

#ifndef CRIMILD_VULKAN_RENDERING_SWAPCHAIN_
#define CRIMILD_VULKAN_RENDERING_SWAPCHAIN_

#include "Foundation/VulkanObject.hpp"
#include "Mathematics/Vector.hpp"
#include "Foundation/Containers/Array.hpp"

namespace crimild {

    class Image;
    class ImageView;

	namespace vulkan {

		class RenderDevice;
		class VulkanSurface;
		class Semaphore;
        class SwapchainManager;

		class Swapchain : public VulkanObject {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::Swapchain )

        public:
            struct Descriptor {
                RenderDevice *renderDevice;
                crimild::Vector2ui extent;
            };

		private:
			using ImageArray = Array< SharedPointer< Image >>;
			using ImageViewArray = Array< SharedPointer< ImageView >>;
			
		public:
			~Swapchain( void ) noexcept;

            RenderDevice *renderDevice = nullptr;
            VulkanSurface *surface = nullptr;
            VkSwapchainKHR handler = VK_NULL_HANDLE;
            VkFormat format;
            VkExtent2D extent;
            crimild::UInt32 maxFramesInFlight = 2;
            SwapchainManager *manager = nullptr;
            ImageArray images;
            ImageViewArray imageViews;

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

        class SwapchainManager : public VulkanObjectManager< Swapchain > {
        public:
            explicit SwapchainManager( RenderDevice *renderDevice ) noexcept : m_renderDevice( renderDevice ) { }
            virtual ~SwapchainManager( void ) = default;

            Swapchain *getSwapchain( void ) noexcept { return VulkanObjectManager< Swapchain >::first(); }
            const Swapchain *getSwapchain( void ) const noexcept { return VulkanObjectManager< Swapchain >::first(); }

            SharedPointer< Swapchain > create( Swapchain::Descriptor const &descriptor ) noexcept;
            void destroy( Swapchain *swapchain ) noexcept override;

        private:
            VkSurfaceFormatKHR chooseSurfaceFormat( const std::vector< VkSurfaceFormatKHR > &availableFormats ) noexcept;
            VkPresentModeKHR choosePresentationMode( const std::vector< VkPresentModeKHR > &availablePresentModes ) noexcept;
            VkExtent2D chooseExtent( const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D requestedExtent ) noexcept;

        private:
            RenderDevice *m_renderDevice = nullptr;
        };

	}

}
	
#endif
	
