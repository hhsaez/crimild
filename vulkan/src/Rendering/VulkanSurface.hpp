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

#ifndef CRIMILD_VULKAN_RENDERING_SURFACE_
#define CRIMILD_VULKAN_RENDERING_SURFACE_

#include "Foundation/Types.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"

namespace crimild {

	namespace vulkan {

		class VulkanInstance;

		/**
		   \brief Handles a Vulkan presentation surface

		   The presentation surface represents an application's window. It's purpose is to 
		   acquire window's paramenters like dimensions, supported color formats or presentation
		   modes. 

		   \remarks The surface must be created by each platform
		 */
		class VulkanSurface : public SharedObject {
		public:
			VulkanSurface( VulkanInstance *instance, VkSurfaceKHR surfaceHandler );
			virtual ~VulkanSurface( void );

			VulkanInstance *getInstance( void ) noexcept { return m_instance; }
			
			const VkSurfaceKHR &getSurfaceHandler( void ) const noexcept { return m_surfaceHandler; }

		private:
			VulkanInstance *m_instance = nullptr;
			VkSurfaceKHR m_surfaceHandler = VK_NULL_HANDLE;
		};

	}

}
	
#endif
	
