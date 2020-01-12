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
#include "Rendering/VulkanBuffer.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanCommandPool.hpp"
#include "Rendering/VulkanDescriptorPool.hpp"
#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanDescriptorSetLayout.hpp"
#include "Rendering/VulkanFence.hpp"
#include "Rendering/VulkanFramebuffer.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanPipeline.hpp"
#include "Rendering/VulkanPipelineLayout.hpp"
#include "Rendering/VulkanRenderPass.hpp"
#include "Rendering/VulkanSemaphore.hpp"
#include "Rendering/VulkanShaderModule.hpp"
#include "Rendering/VulkanSwapchain.hpp"

#include <vector>

namespace crimild {

	namespace vulkan {

        class PhysicalDevice;
        class RenderDeviceManager;
		class VulkanInstance;
		class VulkanSurface;

        class RenderDevice :
        	public VulkanObject,
            public BufferManager,
        	public CommandBufferManager,
        	public CommandPoolManager,
            public DescriptorPoolManager,
            public DescriptorSetManager,
            public DescriptorSetLayoutManager,
        	public SwapchainManager,
            public FenceManager,
        	public FramebufferManager,
        	public ImageManager,
        	public ImageViewManager,
        	public PipelineManager,
        	public PipelineLayoutManager,
            public RenderPassManager,
        	public SemaphoreManager,
        	public ShaderModuleManager {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::RenderDevice )

        public:
            using BufferManager::getHandler;
            using BufferManager::bind;
            using CommandBufferManager::getHandler;
            using CommandBufferManager::bind;
            using CommandPoolManager::create;
            using DescriptorPoolManager::getHandler;
            using DescriptorSetManager::getHandler;
            using DescriptorSetManager::bind;
            using DescriptorSetLayoutManager::getHandler;
            using DescriptorSetLayoutManager::bind;
            using SwapchainManager::create;
            using FenceManager::create;
            using FramebufferManager::create;
            using ImageManager::create;
            using ImageViewManager::create;
            using PipelineManager::getHandler;
            using PipelineManager::bind;
            using PipelineLayoutManager::create;
            using RenderPassManager::create;
            using SemaphoreManager::create;
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

        public:
            void submitGraphicsCommands( const Semaphore *wait, CommandBuffer *commandBuffer, crimild::UInt32 imageIndex, const Semaphore *signal, const Fence *fence ) noexcept;
            void submit( CommandBuffer *commands, crimild::Bool wait ) noexcept;
            void waitIdle( void ) const noexcept;
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

	}

}
	
#endif
	
