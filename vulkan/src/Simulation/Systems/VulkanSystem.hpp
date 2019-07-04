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

#ifndef CRIMILD_VULKAN_SIMULATION_SYSTEMS_VULKAN_
#define CRIMILD_VULKAN_SIMULATION_SYSTEMS_VULKAN_

#include "Foundation/VulkanUtils.hpp"
#include "Simulation/Systems/System.hpp"

namespace crimild {

	namespace vulkan {

		/**
		   \brief Handle Vulkan instance and device creation
		 */
		class VulkanSystem : public System {
			CRIMILD_IMPLEMENT_RTTI( crimild::VulkanSystem )

		private:
			using ValidationLayerArray = std::vector< const char * >;
			using ExtensionArray = std::vector< const char * >;
			
		public:
			//static Priority getPriority( void ) const noexcept { return Priority::after< WindowSystem >() };

			crimild::Bool start( void ) override;
			void stop( void ) override;

		private:
			crimild::Bool createInstance( void ) noexcept;
			crimild::Bool checkValidationLayerSupport( const ValidationLayerArray &validationLayers ) const noexcept;
			ExtensionArray getRequiredExtensions( void ) const noexcept;

			void destroyInstance( void ) noexcept;

		private:
			VkInstance m_instance;
			crimild::Bool m_enableValidationLayers = false;
			ValidationLayerArray m_validationLayers;

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

			void populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept;

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
	
