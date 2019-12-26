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
#include "Rendering/VulkanInstance.hpp"

#define MAX_FRAMES_IN_FLIGHT 2

namespace crimild {

	namespace vulkan {

        class VulkanDebugMessenger;
        class VulkanRenderDevice;
        class VulkanSurface;
		class Semaphore;
		class Fence;
		class RenderPass;
		class Pipeline;
		class CommandPool;
		class CommandBuffer;
		class Framebuffer;

	}

	namespace glfw {

		/**
		   What it takes to work with Vulkan:
		   1. Create an instance
		   2. Select a supported phisical device
		   3. Create logical devices and queues for drawing and presentation
		   4. Create a window, a window surface and a swapchain
		   5. Keep track of swapchain images
		   6. Create a render pass that specifies render targets and usage
		   7. Create framebuffers for the render pass
		   8. Set up the graphics pipeline
		   9. Allocate and record a command buffer with the draw commands for every possible swapchain image
		   10. Draw frames by acquiring images, submitting the right draw command buffer and returing the images back to the swapchain
		 */
		class GLFWVulkanSystem :
        	public System,
            public vulkan::VulkanInstanceManager {
			CRIMILD_IMPLEMENT_RTTI( crimild::glfw::GLFWVulkanSystem )
			
		public:
			System::Priority getInitPriority( void ) const noexcept override { return System::PriorityType::HIGH; }
			System::Priority getPriority( void ) const noexcept override { return System::PriorityType::RENDER; }

			crimild::Bool start( void ) override;
			void update( void ) override;
			void stop( void ) override;

			vulkan::VulkanInstance *getInstance( void ) noexcept { return crimild::get_ptr( m_instance ); }
            vulkan::VulkanRenderDevice *getRenderDevice( void ) noexcept { return crimild::get_ptr( m_renderDevice ); }
            vulkan::VulkanSurface *getSurface( void ) noexcept { return crimild::get_ptr( m_surface ); }

		private:
			/**
			   \brief Instance creation and physical device selection

			   An instance is created by describing the application and any API
			   extensions will be using. Then, we query for Vulkan supported
			   hardware and select at least one VkPhysicalDevice for our operations.
			 */
			crimild::Bool createInstance( void ) noexcept;
            crimild::Bool createDebugMessenger( void ) noexcept;
			crimild::Bool createSurface( void ) noexcept;
			
			/**
			   \brief Logical devices and queue families
			 */
			crimild::Bool createRenderDevice( void ) noexcept;
			crimild::Bool createSwapchain( void ) noexcept;			

		private:
			SharedPointer< vulkan::VulkanInstance > m_instance;
            SharedPointer< vulkan::VulkanDebugMessenger > m_debugMessenger;
            SharedPointer< vulkan::VulkanRenderDevice > m_renderDevice;
            SharedPointer< vulkan::VulkanSurface > m_surface;
			SharedPointer< vulkan::RenderPass > m_renderPass;
			SharedPointer< vulkan::Pipeline > m_pipeline;
			std::vector< SharedPointer< vulkan::Framebuffer >> m_framebuffers;
			SharedPointer< vulkan::CommandPool > m_commandPool;
			std::vector< SharedPointer< vulkan::CommandBuffer >> m_commandBuffers;
			std::vector< SharedPointer< vulkan::Semaphore >> m_imageAvailableSemaphores;
			std::vector< SharedPointer< vulkan::Semaphore >> m_renderFinishedSemaphores;
			std::vector< SharedPointer< vulkan::Fence >> m_inFlightFences;
			crimild::UInt32 m_currentFrame = 0;
		};
    
	}

}
	
#endif
	
