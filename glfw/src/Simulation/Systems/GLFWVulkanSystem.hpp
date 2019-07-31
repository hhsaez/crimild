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
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_GLFW_SIMULATION_SYSTEMS_VULKAN_SYSTEM_
#define CRIMILD_GLFW_SIMULATION_SYSTEMS_VULKAN_SYSTEM_

#include <Simulation/Systems/System.hpp>

#include "Foundation/GLFWUtils.hpp"

namespace crimild {

	namespace vulkan {

		class VulkanInstance;
		class Semaphore;
		class RenderPass;
		class Pipeline;
		class CommandPool;
		class CommandBuffer;
		class Framebuffer;

	}

	namespace glfw {

		class GLFWVulkanSystem : public System {
			CRIMILD_IMPLEMENT_RTTI( crimild::glfw::GLFWVulkanSystem )
			
		public:
			System::Priority getInitPriority( void ) const noexcept override { return System::PriorityType::HIGH; }
			System::Priority getPriority( void ) const noexcept override { return System::PriorityType::RENDER; }

			crimild::Bool start( void ) override;
			void update( void ) override;
			void stop( void ) override;

			vulkan::VulkanInstance *getInstance( void ) noexcept { return crimild::get_ptr( m_instance ); }

		private:
			crimild::Bool createInstance( void ) noexcept;
			crimild::Bool createSurface( void ) noexcept;
			crimild::Bool createRenderDevice( void ) noexcept;
			crimild::Bool createSwapchain( void ) noexcept;			

		private:
			SharedPointer< vulkan::VulkanInstance > m_instance;
			SharedPointer< vulkan::RenderPass > m_renderPass;
			SharedPointer< vulkan::Pipeline > m_pipeline;
			std::vector< SharedPointer< vulkan::Framebuffer >> m_framebuffers;
			SharedPointer< vulkan::CommandPool > m_commandPool;
			std::vector< SharedPointer< vulkan::CommandBuffer >> m_commandBuffers;
			SharedPointer< vulkan::Semaphore > m_imageAvailableSemaphore;
			SharedPointer< vulkan::Semaphore > m_renderFinishedSemaphore;
		};
    
	}

}
	
#endif
	
