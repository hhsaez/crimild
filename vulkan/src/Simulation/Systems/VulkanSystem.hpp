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

#ifndef CRIMILD_VULKAN_SYSTEM_
#define CRIMILD_VULKAN_SYSTEM_

#include "Simulation/Systems/System.hpp"
#include "Rendering/VulkanInstance.hpp"
#include "Rendering/VulkanSurface.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Debug/VulkanDebugMessenger.hpp"

namespace crimild {

    class Buffer;
    class CommandBuffer;
    class DescriptorSetLayout;

    namespace vulkan {

        class CommandPool;
        class Fence;
        class Framebuffer;
        class Semaphore;
        class Swapchain;

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
        class VulkanSystem :
        	public System,
        	public VulkanInstanceManager,
        	public VulkanSurfaceManager,
        	public PhysicalDeviceManager,
            public RenderDeviceManager,
            public VulkanDebugMessengerManager {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::VulkanSystem )

        public:
            using VulkanInstanceManager::create;
            using PhysicalDeviceManager::create;
            using RenderDeviceManager::create;
            using VulkanDebugMessengerManager::create;

        public:
            virtual ~VulkanSystem( void ) = default;

            System::Priority getInitPriority( void ) const noexcept override { return System::PriorityType::HIGH; }
            System::Priority getPriority( void ) const noexcept override { return System::PriorityType::RENDER; }

            crimild::Bool start( void ) override;
            void update( void ) override;
            void stop( void ) override;

            RenderDevice *getRenderDevice( void ) noexcept { return crimild::get_ptr( m_renderDevice ); }
            Swapchain *getSwapchain( void ) noexcept { return crimild::get_ptr( m_swapchain ); }
            RenderPass *getRenderPass( void ) noexcept { return crimild::get_ptr( m_renderPass ); }
            CommandPool *getCommandPool( void ) noexcept { return crimild::get_ptr( m_commandPool ); }
            Framebuffer *getFramebuffer( crimild::Size index ) noexcept { return crimild::get_ptr( m_framebuffers[ index ] ); }

            void setCommandBuffers( std::vector< SharedPointer< CommandBuffer >> &cmds ) noexcept { m_commandBuffers = cmds; }

        protected:
            virtual SharedPointer< VulkanSurface > create( VulkanSurface::Descriptor const &descriptor ) noexcept { return nullptr; }

        private:
            crimild::Bool createInstance( void ) noexcept;
            crimild::Bool createDebugMessenger( void ) noexcept;
            crimild::Bool createSurface( void ) noexcept;
            crimild::Bool createPhysicalDevice( void ) noexcept;
            crimild::Bool createRenderDevice( void ) noexcept;
            crimild::Bool createSwapchain( void ) noexcept;
            crimild::Bool recreateSwapchain( void ) noexcept;
            crimild::Bool createRenderPass( void ) noexcept;
            crimild::Bool createFramebuffers( void ) noexcept;
            crimild::Bool createSyncObjects( void ) noexcept;
            crimild::Bool createCommandPool( void ) noexcept;

            void updateUniformBuffer( crimild::UInt32 currentImage ) noexcept;

            void cleanSwapchain( void ) noexcept;

        private:
            SharedPointer< VulkanInstance > m_instance;
            SharedPointer< VulkanDebugMessenger > m_debugMessenger;
            SharedPointer< VulkanSurface > m_surface;
            SharedPointer< PhysicalDevice > m_physicalDevice;
            SharedPointer< RenderDevice > m_renderDevice;
            SharedPointer< Swapchain > m_swapchain;
            SharedPointer< RenderPass > m_renderPass;
            std::vector< SharedPointer< Framebuffer >> m_framebuffers;
            SharedPointer< CommandPool > m_commandPool;
            std::vector< SharedPointer< CommandBuffer >> m_commandBuffers;
            std::vector< SharedPointer< Semaphore >> m_imageAvailableSemaphores;
            std::vector< SharedPointer< Semaphore >> m_renderFinishedSemaphores;
            std::vector< SharedPointer< Fence >> m_inFlightFences;
            crimild::UInt32 m_currentFrame = 0;
        };

    }

}

#endif
