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

#include <vector>

namespace crimild {

	namespace vulkan {

		/**
		   \brief Handles creation and setup for the Vulkan instance
		 */
		class VulkanInstance {
		private:
			using ValidationLayerArray = std::vector< const char * >;
			using ExtensionArray = std::vector< const char * >;

		public:
			static crimild::Bool create( void ) noexcept;
			static void destroy( void ) noexcept;
			static VkInstance &get( void ) noexcept { return s_instance; }

			static crimild::Bool enableValidationLayers( void ) noexcept { return s_enableValidationLayers; }
			static ValidationLayerArray &getValidationLayers( void ) noexcept { return s_validationLayers; }
			
		private:
			static crimild::Bool createInstance( void ) noexcept;
			static crimild::Bool checkValidationLayerSupport( const ValidationLayerArray &validationLayers ) noexcept;
			static ExtensionArray getRequiredExtensions( void ) noexcept;
			static void destroyInstance( void ) noexcept;

		private:
			static VkInstance s_instance;
			static crimild::Bool s_enableValidationLayers;
			static ValidationLayerArray s_validationLayers;

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
			static crimild::Bool createDebugMessenger( void ) noexcept;
			static void destroyDebugMessenger( void ) noexcept;
			
		private:
			static VkDebugUtilsMessengerEXT m_debugMessenger;

			//@}
		};
		
	}

}
	
#endif
	
