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

#ifndef CRIMILD_VULKAN_RENDERING_IMAGE_
#define CRIMILD_VULKAN_RENDERING_IMAGE_

#include "Foundation/Types.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"

namespace crimild {

	namespace vulkan {

		class VulkanRenderDevice;

		/**
		 */
		class Image : public SharedObject {
		public:
			/**
			   \brief Creates an image from a device and a handler

			   It migth be possible to create an image with a null device, in which
			   case the image handler won't be destroyed since the image is managed
			   elsewhere (\see Swapchain)
			 */
			Image( VulkanRenderDevice *device, const VkImage &imageHandler );
			~Image( void );
			
			const VkImage &getImageHandler( void ) const noexcept { return m_imageHandler; }

		private:
			VulkanRenderDevice *m_renderDevice = nullptr;
			VkImage m_imageHandler = VK_NULL_HANDLE;
		};

	}

}
	
#endif
	
