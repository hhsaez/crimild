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

#include "Rendering/VulkanRenderDeviceOLD.hpp"

#include "Foundation/Log.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanSurface.hpp"
#include "Simulation/Event.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanFence.hpp"
#include "VulkanGraphicsPipelineOLD.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanInstance.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanSwapchainOLD.hpp"

#include <array>
#include <set>

using namespace crimild;
using namespace crimild::vulkan;

const int CRIMILD_MAX_FRAMES_IN_FLIGHT = 2;

//////////////////////
// DELETE FROM HERE //
//////////////////////

RenderDeviceOLD::RenderDeviceOLD( void )
    : CommandPoolManager( this ),
      SwapchainManager( this ),
      FenceManager( this ),
      PipelineLayoutManager( this ),
      SemaphoreManager( this ),
      ShaderModuleManager( this )
{
}

RenderDeviceOLD::~RenderDeviceOLD( void )
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

void RenderDeviceOLD::submitGraphicsCommands( const Semaphore *wait, Array< CommandBuffer * > &commandBuffers, crimild::UInt32 imageIndex, const Semaphore *signal, const Fence *fence ) noexcept
{
    VkSemaphore waitSemaphores[] = {
        wait->handler,
    };

    VkSemaphore signalSemaphores[] = {
        signal->handler,
    };

    VkPipelineStageFlags waitStages[] = {
        // Ensure that render passes don't begin until there's an image available
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };

    Array< VkCommandBuffer > commandBufferHandlers = commandBuffers.map(
        [ & ]( auto commandBuffer ) {
            updateCommandBuffer( commandBuffer );
            return getBindInfo( commandBuffer ).handler;
        } );

    auto submitInfo = VkSubmitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = UInt32( commandBufferHandlers.size() ),
        .pCommandBuffers = commandBufferHandlers.getData(),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    CRIMILD_VULKAN_CHECK(
        vkQueueSubmit(
            graphicsQueue,
            1,
            &submitInfo,
            fence != nullptr ? fence->handler : VK_NULL_HANDLE ) );
}

void RenderDeviceOLD::submitComputeCommands( CommandBuffer *commands ) noexcept
{
    VkCommandBuffer commandBuffers[] = {
        getBindInfo( commands ).handler,
    };

    auto submitInfo = VkSubmitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = commandBuffers,
    };

    CRIMILD_VULKAN_CHECK(
        vkQueueSubmit(
            computeQueue,
            1,
            &submitInfo,
            VK_NULL_HANDLE ) );
}

void RenderDeviceOLD::submit( CommandBuffer *commands, crimild::Bool wait ) noexcept
{
    auto commandBufferHandler = getBindInfo( commands ).handler;

    auto submitInfo = VkSubmitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBufferHandler,
    };

    CRIMILD_VULKAN_CHECK(
        vkQueueSubmit(
            graphicsQueue,
            1,
            &submitInfo,
            VK_NULL_HANDLE ) );

    if ( wait ) {
        CRIMILD_VULKAN_CHECK(
            vkQueueWaitIdle(
                graphicsQueue ) );
    }
}

void RenderDeviceOLD::waitIdle( void ) const noexcept
{
    if ( handler == VK_NULL_HANDLE ) {
        return;
    }

    vkDeviceWaitIdle( handler );
    vkQueueWaitIdle( graphicsQueue );
    vkQueueWaitIdle( computeQueue );
}

SharedPointer< RenderDeviceOLD > RenderDeviceManager::create( RenderDeviceOLD::Descriptor const &descriptor ) noexcept
{
    return nullptr;
}

void RenderDeviceManager::destroy( RenderDeviceOLD *renderDevice ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan logical device" );

    static_cast< TextureManager * >( renderDevice )->clear();
    static_cast< SamplerManager * >( renderDevice )->clear();
    static_cast< DescriptorPoolManager * >( renderDevice )->clear();
    static_cast< DescriptorSetManager * >( renderDevice )->clear();
    static_cast< DescriptorSetLayoutManager * >( renderDevice )->clear();
    static_cast< IndexBufferManager * >( renderDevice )->clear();
    static_cast< VertexBufferManager * >( renderDevice )->clear();
    static_cast< StorageBufferManager * >( renderDevice )->clear();
    static_cast< UniformBufferManager * >( renderDevice )->clear();
    static_cast< FenceManager * >( renderDevice )->cleanup();
    static_cast< SemaphoreManager * >( renderDevice )->cleanup();
    static_cast< CommandBufferManager * >( renderDevice )->clear();
    static_cast< CommandPoolManager * >( renderDevice )->cleanup();
    static_cast< ShaderModuleManager * >( renderDevice )->cleanup();
    static_cast< GraphicsPipelineManager * >( renderDevice )->clear();
    static_cast< ComputePipelineManager * >( renderDevice )->clear();
    static_cast< PipelineLayoutManager * >( renderDevice )->cleanup();
    static_cast< RenderPassManager * >( renderDevice )->clear();
    static_cast< ImageViewManager * >( renderDevice )->clear();
    static_cast< ImageManager * >( renderDevice )->clear();
    static_cast< SwapchainManager * >( renderDevice )->cleanup();

    if ( renderDevice->handler != VK_NULL_HANDLE ) {
        vkDestroyDevice( renderDevice->handler, nullptr );
    }

    renderDevice->handler = VK_NULL_HANDLE;
    renderDevice->physicalDevice = nullptr;
    renderDevice->surface = nullptr;
    renderDevice->manager = nullptr;
    renderDevice->graphicsQueue = VK_NULL_HANDLE;
    renderDevice->presentQueue = VK_NULL_HANDLE;
    erase( renderDevice );
}

/*

VulkanRenderDevice::VulkanRenderDevice( VulkanInstance *instance, VulkanSurface *surface, const VkPhysicalDevice &physicalDevice, const VkDevice &device )
	: m_instance( instance ),
	  m_surface( surface ),
	  m_physicalDevice( physicalDevice ),
	  m_device( device )
{
	m_msaaSamples = getMaxUsableSampleCount();

//	QueueFamilyIndices indices = findQueueFamilies( physicalDevice, surface->handler );

	// Get queue handles
//	vkGetDeviceQueue( m_device, indices.graphicsFamily[ 0 ], 0, &m_graphicsQueue );
//	vkGetDeviceQueue( m_device, indices.presentFamily[ 0 ], 0, &m_presentQueue );
}

VulkanRenderDevice::~VulkanRenderDevice( void )
{
	m_swapchain = nullptr;

	vkDestroyDevice( m_device, nullptr );
	m_device = VK_NULL_HANDLE;
}

VkSampleCountFlagBits VulkanRenderDevice::getMaxUsableSampleCount( void ) const noexcept
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties( m_physicalDevice, &physicalDeviceProperties );

	auto counts = std::min(
		physicalDeviceProperties.limits.framebufferColorSampleCounts,
		physicalDeviceProperties.limits.framebufferDepthSampleCounts
	);

	if ( counts & VK_SAMPLE_COUNT_64_BIT ) return VK_SAMPLE_COUNT_64_BIT;
	if ( counts & VK_SAMPLE_COUNT_32_BIT ) return VK_SAMPLE_COUNT_32_BIT;
	if ( counts & VK_SAMPLE_COUNT_16_BIT ) return VK_SAMPLE_COUNT_16_BIT;
	if ( counts & VK_SAMPLE_COUNT_8_BIT ) return VK_SAMPLE_COUNT_8_BIT;
	if ( counts & VK_SAMPLE_COUNT_4_BIT ) return VK_SAMPLE_COUNT_4_BIT;
	if ( counts & VK_SAMPLE_COUNT_2_BIT ) return VK_SAMPLE_COUNT_2_BIT;
	return VK_SAMPLE_COUNT_1_BIT;
}
*/
