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

#ifndef CRIMILD_VULKAN_RENDERING_INSTANCE_
#define CRIMILD_VULKAN_RENDERING_INSTANCE_

#include "Foundation/Types.hpp"
#include "Foundation/VulkanUtils.hpp"
#include "Foundation/SharedObject.hpp"

#include <vector>

namespace crimild {

	namespace vulkan {

		class VulkanSurface;
		class VulkanRenderDevice;
		class Swapchain;

		/**
		   \brief Handles creation and setup for the Vulkan instance
		 */
		class VulkanInstance : public SharedObject {
		private:
			using ValidationLayerArray = std::vector< const char * >;
			using ExtensionArray = std::vector< const char * >;

		public:
			/**
			   \brief Creates a VulkanInstance object holding the VkInstance reference
			 */
			static SharedPointer< VulkanInstance > create( void ) noexcept;

			/**
			   \brief Used for checking if validation layers should be enabled
			 */
			static crimild::Bool enableValidationLayers( void ) noexcept
			{
#if defined( CRIMILD_DEBUG )
				return true;
#else
				return false;
#endif
			}

			/**
			   \brief The list of validation layers (only valid if they're enabled)
			   \see enableValidationLayers()
			 */
			static const ValidationLayerArray &getValidationLayers( void ) noexcept
			{
				static ValidationLayerArray validationLayers = {
					"VK_LAYER_LUNARG_standard_validation",					
				};
				return validationLayers;
			}

		public:
			explicit VulkanInstance( VkInstance instance );
			~VulkanInstance( void );

			const VkInstance &getInstanceHandler( void ) const noexcept { return m_instanceHandler; }

		private:
			VkInstance m_instanceHandler;			

		private:
			static crimild::Bool checkValidationLayerSupport( const ValidationLayerArray &validationLayers ) noexcept;
			static ExtensionArray getRequiredExtensions( void ) noexcept;

			/**
			   \name Surface
			 */
			//@{

		public:
			void setSurface( SharedPointer< VulkanSurface > const &surface ) noexcept { m_surface = surface; }
			VulkanSurface *getSurface( void ) noexcept { return crimild::get_ptr( m_surface ); }

		private:
			SharedPointer< VulkanSurface > m_surface;

			//@}

			/**
			   \name Render Device			   
			 */
			//@{

		public:
			void setRenderDevice( SharedPointer< VulkanRenderDevice > const &renderDevice ) noexcept { m_renderDevice = renderDevice; }
			VulkanRenderDevice *getRenderDevice( void ) noexcept { return crimild::get_ptr( m_renderDevice ); }

		private:
			SharedPointer< VulkanRenderDevice > m_renderDevice;

			//@}

			/**
			   \name Debugging
			*/
			//@{

		private:
			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
				void *pUserData
			) noexcept;

			static VkResult createDebugUtilsMessengerEXT(
				VkInstance instance,
				const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
				const VkAllocationCallbacks *pAllocator,
				VkDebugUtilsMessengerEXT *pDebugMessenger
			) noexcept;

			static void destroyDebugUtilsMessengerEXT(
				VkInstance instance,
				VkDebugUtilsMessengerEXT debugMessenger,
				const VkAllocationCallbacks *pAllocator
			) noexcept;

			static void populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept;

			/**
			   \brief Creates a debug messenger for validation layers
			   \see createInstance for another usage
			 */
			crimild::Bool createDebugMessenger( void ) noexcept;
			void destroyDebugMessenger( void ) noexcept;
			
		private:
			VkDebugUtilsMessengerEXT m_debugMessenger;

			//@}
		};
		
	}

}
	
#endif
	
