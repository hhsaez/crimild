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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_DEVICE_OLD_
#define CRIMILD_VULKAN_RENDERING_RENDER_DEVICE_OLD_

#include "Foundation/VulkanObject.hpp"
#include "Rendering/VulkanCommandBuffer.hpp"
#include "Rendering/VulkanCommandPool.hpp"
#include "Rendering/VulkanComputePipeline.hpp"
#include "Rendering/VulkanDescriptorPool.hpp"
#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanDescriptorSetLayout.hpp"
#include "Rendering/VulkanFence.hpp"
#include "Rendering/VulkanGraphicsPipelineOLD.hpp"
#include "Rendering/VulkanImageOLD.hpp"
#include "Rendering/VulkanImageViewOLD.hpp"
#include "Rendering/VulkanIndexBuffer.hpp"
#include "Rendering/VulkanPipelineLayout.hpp"
#include "Rendering/VulkanRenderPassOLD.hpp"
#include "Rendering/VulkanSampler.hpp"
#include "Rendering/VulkanSemaphore.hpp"
#include "Rendering/VulkanShaderCompiler.hpp"
#include "Rendering/VulkanShaderModule.hpp"
#include "Rendering/VulkanStorageBuffer.hpp"
#include "Rendering/VulkanSwapchainOLD.hpp"
#include "Rendering/VulkanTexture.hpp"
#include "Rendering/VulkanUniformBuffer.hpp"
#include "Rendering/VulkanVertexBuffer.hpp"

#include <vector>

namespace crimild {

    namespace vulkan {

        //////////////////////
        // DELETE FROM HERE //
        //////////////////////

        class PhysicalDeviceOLD;
        class RenderDeviceManager;
        class VulkanInstanceOLD;
        class VulkanSurfaceOLD;

        class [[deprecated]] RenderDeviceOLD
            : public VulkanObject,
              public CommandBufferManager,
              public CommandPoolManager,
              public DescriptorPoolManager,
              public DescriptorSetManager,
              public DescriptorSetLayoutManager,
              public SwapchainManager,
              public FenceManager,
              public ImageManager,
              public ImageViewManager,
              public IndexBufferManager,
              public GraphicsPipelineManager,
              public ComputePipelineManager,
              public PipelineLayoutManager,
              public RenderPassManager,
              public SamplerManager,
              public SemaphoreManager,
              public ShaderModuleManager,
              public StorageBufferManager,
              public TextureManager,
              public UniformBufferManager,
              public VertexBufferManager {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::RenderDeviceOLD )

        public:
            using CommandBufferManager::bind;
            using CommandBufferManager::getBindInfo;
            using CommandPoolManager::create;
            using ComputePipelineManager::bind;
            using ComputePipelineManager::getBindInfo;
            using DescriptorPoolManager::getHandler;
            using DescriptorSetLayoutManager::bind;
            using DescriptorSetLayoutManager::getHandler;
            using DescriptorSetManager::bind;
            using DescriptorSetManager::getHandler;
            using FenceManager::create;
            using GraphicsPipelineManager::bind;
            using GraphicsPipelineManager::getBindInfo;
            using ImageManager::bind;
            using ImageManager::getBindInfo;
            using ImageManager::setBindInfo;
            using ImageViewManager::bind;
            using ImageViewManager::getBindInfo;
            using ImageViewManager::unbind;
            using IndexBufferManager::bind;
            using IndexBufferManager::getBindInfo;
            using PipelineLayoutManager::create;
            using RenderPassManager::bind;
            using RenderPassManager::getBindInfo;
            using SamplerManager::bind;
            using SamplerManager::getBindInfo;
            using SamplerManager::unbind;
            using SemaphoreManager::create;
            using ShaderModuleManager::create;
            using StorageBufferManager::bind;
            using StorageBufferManager::getBindInfo;
            using SwapchainManager::create;
            using TextureManager::bind;
            using TextureManager::getBindInfo;
            using UniformBufferManager::bind;
            using UniformBufferManager::getBindInfo;
            using VertexBufferManager::bind;
            using VertexBufferManager::getBindInfo;

            struct Descriptor {
                PhysicalDeviceOLD *physicalDevice;
            };

        public:
            RenderDeviceOLD( void );
            ~RenderDeviceOLD( void );

            VkDevice handler = VK_NULL_HANDLE;
            PhysicalDeviceOLD *physicalDevice = nullptr;
            VulkanSurfaceOLD *surface = nullptr;
            RenderDeviceManager *manager = nullptr;
            VkQueue graphicsQueue;
            VkQueue computeQueue;
            VkQueue presentQueue;

        public:
            void submitGraphicsCommands( const Semaphore *wait, Array< CommandBuffer * > &commandBuffer, crimild::UInt32 imageIndex, const Semaphore *signal, const Fence *fence ) noexcept;
            void submitComputeCommands( CommandBuffer *commands ) noexcept;
            void submit( CommandBuffer *commands, crimild::Bool wait ) noexcept;
            void waitIdle( void ) const noexcept;
        };

        class RenderDeviceManager : public VulkanObjectManager< RenderDeviceOLD > {
        public:
            explicit RenderDeviceManager( PhysicalDeviceOLD *physicalDevice = nullptr ) noexcept
                : m_physicalDevice( physicalDevice ) { }
            virtual ~RenderDeviceManager( void ) = default;

            SharedPointer< RenderDeviceOLD > create( RenderDeviceOLD::Descriptor const &descriptor ) noexcept;
            void destroy( RenderDeviceOLD *renderDevice ) noexcept override;

        private:
            PhysicalDeviceOLD *m_physicalDevice = nullptr;
        };

    }

}

#endif