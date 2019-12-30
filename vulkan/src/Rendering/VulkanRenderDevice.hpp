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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_DEVICE_
#define CRIMILD_VULKAN_RENDERING_RENDER_DEVICE_

#include "Foundation/VulkanObject.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanImageView.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanShaderModule.hpp"

#include <vector>

namespace crimild {

	namespace vulkan {

        class CommandBuffer;
        class CommandPool;
        class Fence;
        class PhysicalDevice;
        class RenderDeviceManager;
        class Semaphore;
        class Swapchain;
		class VulkanInstance;
		class VulkanSurface;

        class RenderDevice :
        	public VulkanObject,
        	public SwapchainManager,
        	public ImageManager,
        	public ImageViewManager,
        	public PipelineManager,
        	public PipelineLayoutManager,
        	public ShaderModuleManager {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::RenderDevice )

        public:
            using SwapchainManager::create;
            using ImageManager::create;
            using ImageViewManager::create;
            using PipelineManager::create;
            using PipelineLayoutManager::create;
            using ShaderModuleManager::create;

            struct Descriptor {
                PhysicalDevice *physicalDevice;
            };

        public:
            RenderDevice( void );
            ~RenderDevice( void );

            VkDevice handler = VK_NULL_HANDLE;
            PhysicalDevice *physicalDevice = nullptr;
            VulkanSurface *surface = nullptr;
            RenderDeviceManager *manager = nullptr;
            VkQueue graphicsQueue;
            VkQueue presentQueue;
        };

        class RenderDeviceManager : public VulkanObjectManager< RenderDevice > {
        public:
            explicit RenderDeviceManager( PhysicalDevice *physicalDevice = nullptr ) noexcept : m_physicalDevice( physicalDevice ) { }
            virtual ~RenderDeviceManager( void ) = default;

            SharedPointer< RenderDevice > create( RenderDevice::Descriptor const &descriptor ) noexcept;
            void destroy( RenderDevice *renderDevice ) noexcept override;

        private:
            PhysicalDevice *m_physicalDevice = nullptr;
        };

		/**
		   \brief Implements a render device for Vulkan

		   For simplicity, we're picking only one physical device and creating
		   only one logical device. 
		 */
		class VulkanRenderDevice : public SharedObject {
		public:
			static SharedPointer< VulkanRenderDevice > create( VulkanInstance *instance, VulkanSurface *surface ) noexcept;

		private:
			static VkPhysicalDevice pickPhysicalDevice( const VkInstance &instance, const VkSurfaceKHR &surface ) noexcept;
			static crimild::Bool pickPhysicalDevice( void ) noexcept;
			static crimild::Bool isDeviceSuitable( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept;

			using DeviceExtensionArray = std::vector< const char * >;
			static const DeviceExtensionArray &getDeviceExtensions( void ) noexcept
			{
				static DeviceExtensionArray deviceExtensions = {
					VK_KHR_SWAPCHAIN_EXTENSION_NAME,
				};
				return deviceExtensions;
			}

			static crimild::Bool checkDeviceExtensionSupport( const VkPhysicalDevice &device ) noexcept;
			
			struct QueueFamilyIndices {
				std::vector< crimild::UInt32 > graphicsFamily;
				std::vector< crimild::UInt32 > presentFamily;

				bool isComplete( void )
				{
					return graphicsFamily.size() > 0 && presentFamily.size() > 0;
				}
			};

			/**
			   TODO: How do we handle the case of an option surface param? 
			 */
//			static QueueFamilyIndices findQueueFamilies( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept;
			
			struct SwapchainSupportDetails {
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector< VkSurfaceFormatKHR > formats;
				std::vector< VkPresentModeKHR > presentModes;
			};

			static SwapchainSupportDetails querySwapchainSupport( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept;
			
			static VkDevice createLogicalDevice( const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface ) noexcept;

		public:
			VulkanRenderDevice( VulkanInstance *instance, VulkanSurface *surface, const VkPhysicalDevice &physicalDevice, const VkDevice &deivv );

			/**
			   This method destroys the logical device. 
			   The physical device is destroy along with the Vulkan instance
			 */
			~VulkanRenderDevice( void );

			const VkPhysicalDevice &getPhysicalDeviceHandler( void ) const noexcept { return m_physicalDevice; }
			const VkDevice &getDeviceHandler( void ) const noexcept { return m_device; }
			const VkQueue &getGraphicsQueueHandler( void ) const noexcept { return m_graphicsQueue; }
			const VkQueue &getPresentQueueHandler( void ) const noexcept { return m_presentQueue; }

			QueueFamilyIndices getQueueFamilies( void ) const noexcept;


			/**
			   \brief Wait for the device to complete any pending operation

			   \remarks This blocks the current thread until completion
			 */
			void waitIdle( void ) const noexcept;

		private:
			VkSampleCountFlagBits getMaxUsableSampleCount( void ) const noexcept;

		private:
			VulkanInstance *m_instance = nullptr;
			VulkanSurface *m_surface = nullptr;
			VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
			VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
			VkDevice m_device = VK_NULL_HANDLE;
			VkQueue m_graphicsQueue;
			VkQueue m_presentQueue;

			/**
			   \name Swapchain
			 */
			//@{
			
		public:
			void setSwapchain( SharedPointer< Swapchain > const &swapchain ) noexcept { m_swapchain = swapchain; }
			Swapchain *getSwapchain( void ) noexcept { return crimild::get_ptr( m_swapchain ); }
			const Swapchain *getSwapchain( void ) const noexcept { return crimild::get_ptr( m_swapchain ); }

		private:
			SharedPointer< Swapchain > m_swapchain;
			
			//@}

			/**
			   \name Synchronization objects
			 */
			//@{
			
		public:
			SharedPointer< Semaphore > createSemaphore( void ) const noexcept;
			SharedPointer< Fence > createFence( void ) noexcept;

			//@}

			/**
			   \name Images and Image Views
			*/
			//@{
		public:
			SharedPointer< Image > createImage( void );
			SharedPointer< ImageView > createImageView( Image *image, VkFormat format, VkImageAspectFlags aspectFlags, crimild::UInt32 mipLevels );

			//@}

			/**
			   \name Pipelines
			*/
			//@{

		public:
			SharedPointer< Pipeline > createPipeline( const Pipeline::Descriptor &descriptor ) const noexcept;

			//@}

			/**
			   \name Render passes and framebuffers
			 */
			//@{

		public:
			SharedPointer< RenderPass > createRenderPass( void ) const noexcept;
			SharedPointer< Framebuffer > createFramebuffer( const Framebuffer::Descriptor &descriptor ) const noexcept;

			//@}

			/**
			   \name Command Pools and Buffers
			 */
			//@{
			
		public:
			SharedPointer< CommandPool > createGraphicsCommandPool( void ) const noexcept;
			void submitGraphicsCommands( const Semaphore *wait, const CommandBuffer *commandBuffer, const Semaphore *signal, const Fence *fence ) const;

			//@}
		};

	}

}
	
#endif
	
