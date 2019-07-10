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
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_DEVICE_
#define CRIMILD_VULKAN_RENDERING_RENDER_DEVICE_

#include "Foundation/Types.hpp"
#include "Foundation/VulkanUtils.hpp"

#include <vector>

namespace crimild {

	namespace vulkan {

		/**
		   \brief Implements a render device for Vulkan

		   For simplicity, we're picking only one physical device and creating
		   only one logical device. 
		 */
		class VulkanRenderDevice {
		public:
			crimild::Bool configure( void ) noexcept;

			/**
			   This method destroys the logical device. 
			   The physical device is destroy along with the Vulkan instance
			 */
			void cleanup( void ) noexcept;

		private:
			crimild::Bool pickPhysicalDevice( void ) noexcept;
			crimild::Bool isDeviceSuitable( VkPhysicalDevice device ) const noexcept;
			crimild::Bool checkDeviceExtensionSupport( VkPhysicalDevice device ) const noexcept;
			VkSampleCountFlagBits getMaxUsableSampleCount( void ) const noexcept;

			struct QueueFamilyIndices {
				std::vector< crimild::UInt32 > graphicsFamily;
				std::vector< crimild::UInt32 > presentFamily;

				bool isComplete( void )
				{
					return graphicsFamily.size() > 0 && presentFamily.size() > 0;
				}
			};
			
			QueueFamilyIndices findQueueFamilies( VkPhysicalDevice device ) const noexcept;
			
			crimild::Bool createLogicalDevice( void ) noexcept;
			
		private:
			VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
			VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
			VkDevice m_device = VK_NULL_HANDLE;
			VkQueue m_graphicsQueue;
			VkQueue m_presentQueue;
			const std::vector< const char * > m_deviceExtensions {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			};
		};

	}

}
	
#endif
	
