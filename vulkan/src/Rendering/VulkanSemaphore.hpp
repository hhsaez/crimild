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

#ifndef CRIMILD_VULKAN_RENDERING_SEMAPHORE_
#define CRIMILD_VULKAN_RENDERING_SEMAPHORE_

#include "Foundation/Types.hpp"
#include "Foundation/RTTI.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"

namespace crimild {

	namespace vulkan {

		class VulkanRenderDevice;

		/**
		   \brief Implements a Vulkan semaphore

		   Semaphores are used to synchronize device's queues. When commands are submitted
		   for processing, they may require other jobs to be finished first. We can specify that
		   the former ones wait for them to complete before they're executed by using semaphores.
		 */
		class Semaphore : public SharedObject, public RTTI {
			CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::Semaphore );
			
		public:
			explicit Semaphore( VulkanRenderDevice *device, const VkSemaphore &semaphoreHandler );
			~Semaphore( void );
			
			const VkSemaphore &getSemaphoreHandler( void ) const noexcept { return m_semaphoreHandler; }

		private:
			VulkanRenderDevice *m_renderDevice = nullptr;
			VkSemaphore m_semaphoreHandler = VK_NULL_HANDLE;
		};

	}

}
	
#endif
	
