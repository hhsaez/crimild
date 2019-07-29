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

#include "Foundation/Types.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"
#include "Foundation/Containers/Array.hpp"

#include <vector>

namespace crimild {

	namespace vulkan {

		class VulkanRenderDevice;
		class VulkanSurface;
		class Image;
		class ImageView;

		class Swapchain : public SharedObject {
		private:
			using ImageArray = containers::Array< SharedPointer< Image >>;
			using ImageViewArray = containers::Array< SharedPointer< ImageView >>;
			
		public:
			static crimild::Bool swapchainSupported( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept;

			static SharedPointer< Swapchain > create( VulkanRenderDevice *device, VulkanSurface *surface ) noexcept;

		private:
			struct SwapchainSupportDetails {
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector< VkSurfaceFormatKHR > formats;
				std::vector< VkPresentModeKHR > presentModes;
			};

			static SwapchainSupportDetails querySupportDetails( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept;
			static VkSurfaceFormatKHR chooseSurfaceFormat( const std::vector< VkSurfaceFormatKHR > &availableFormats ) noexcept;
			static VkPresentModeKHR choosePresentationMode( const std::vector< VkPresentModeKHR > &availablePresentModes ) noexcept;
			static VkExtent2D chooseExtent( const VkSurfaceCapabilitiesKHR &capabilities ) noexcept;
			
		public:
			Swapchain(
				VulkanRenderDevice *device,
				VulkanSurface *surface,
				const VkSwapchainKHR &swapchainHandler,
				const VkFormat &format,
				const VkExtent2D &extent
			) noexcept;

			~Swapchain( void ) noexcept;

			const VkSwapchainKHR &getSwapchainHandler( void ) const noexcept { return m_swapchainHandler; }
			const VkFormat &getFormat( void ) const noexcept { return m_format; }
			const VkExtent2D &getExtent( void ) const noexcept { return m_extent; }
			const ImageArray &getImages( void ) const { return m_images; }
			const ImageViewArray &getImageViews( void ) const { return m_imageViews; }

			crimild::UInt32 acquireNextImage( void ) const noexcept;
			void presentImage( crimild::UInt32 imageIndex ) const noexcept;

		private:
			void retrieveSwapchainImages( void ) noexcept;
			void createImageViews( void ) noexcept;

		private:
			VulkanRenderDevice *m_device = nullptr;
			VulkanSurface *m_surface = nullptr;
			VkSwapchainKHR m_swapchainHandler = VK_NULL_HANDLE;
			VkFormat m_format;
			VkExtent2D m_extent;
			crimild::UInt32 m_maxFramesInFlight = 2;
			containers::Array< SharedPointer< Image >> m_images;
			containers::Array< SharedPointer< ImageView >> m_imageViews;			
		};

	}

}
	
#endif
	
