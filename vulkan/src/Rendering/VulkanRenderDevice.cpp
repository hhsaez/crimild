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

#include "VulkanRenderDevice.hpp"
#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanFence.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanCommandBuffer.hpp"
#include "Foundation/Log.hpp"

#include <set>

using namespace crimild;
using namespace crimild::vulkan;

RenderDevice::RenderDevice( void )
    : BufferManager( this ),
      CommandBufferManager( this ),
      CommandPoolManager( this ),
      DescriptorPoolManager( this ),
      DescriptorSetManager( this ),
      DescriptorSetLayoutManager( this ),
	  SwapchainManager( this ),
      FenceManager( this ),
      FramebufferManager( this ),
      ImageManager( this ),
	  ImageViewManager( this ),
      PipelineManager( this ),
      PipelineLayoutManager( this ),
      RenderPassManager( this ),
      SemaphoreManager( this ),
	  ShaderModuleManager( this )
{

}

RenderDevice::~RenderDevice( void )
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

void RenderDevice::submitGraphicsCommands( const Semaphore *wait, CommandBuffer *commandBuffer, crimild::UInt32 imageIndex, const Semaphore *signal, const Fence *fence ) noexcept
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

    VkCommandBuffer commandBuffers[] = {
        getHandler( commandBuffer, imageIndex ),
    };

    auto submitInfo = VkSubmitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = commandBuffers,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    CRIMILD_VULKAN_CHECK(
        vkQueueSubmit(
            graphicsQueue,
            1,
            &submitInfo,
            fence != nullptr ? fence->handler : VK_NULL_HANDLE
        )
    );
}

void RenderDevice::submit( CommandBuffer *commands, crimild::Bool wait ) noexcept
{
    auto commandBufferHandler = getHandler( commands, 0 );

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
            VK_NULL_HANDLE
        )
    );

    if ( wait ) {
    	CRIMILD_VULKAN_CHECK(
        	vkQueueWaitIdle(
        		graphicsQueue
        	)
    	);
    }
}

void RenderDevice::waitIdle( void ) const noexcept
{
    if ( handler == VK_NULL_HANDLE ) {
        return;
    }

    vkDeviceWaitIdle( handler );
}

SharedPointer< RenderDevice > RenderDeviceManager::create( RenderDevice::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan logical device" );

    auto physicalDevice = m_physicalDevice;
    if ( physicalDevice == nullptr ) {
        physicalDevice = descriptor.physicalDevice;
    }

    auto surface = physicalDevice->surface;

    auto indices = utils::findQueueFamilies( physicalDevice->handler, surface->handler );
    if ( !indices.isComplete() ) {
        // Should never happen
        CRIMILD_LOG_ERROR( "Invalid physical device" );
        return nullptr;
    }

    // Make sure we're creating queue for unique families,
    // since both graphics and presenatation might be same family
    // \see utils::findQueueFamilies()
    std::unordered_set< crimild::UInt32 > uniqueQueueFamilies = {
        indices.graphicsFamily[ 0 ],
        indices.presentFamily[ 0 ],
    };

    // Required even if there's only one queue
    auto queuePriority = 1.0f;

    std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;
    for ( auto queueFamily : uniqueQueueFamilies ) {
        auto createInfo = VkDeviceQueueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };
        queueCreateInfos.push_back( createInfo );
    }

    // Set the requried device features
    auto deviceFeatures = VkPhysicalDeviceFeatures {
        .samplerAnisotropy = VK_TRUE,
    };

    const auto &deviceExtensions = utils::getDeviceExtensions();

    auto createInfo = VkDeviceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast< crimild::UInt32 >( queueCreateInfos.size() ),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .pEnabledFeatures = &deviceFeatures,
        .enabledLayerCount = 0,
        .enabledExtensionCount = static_cast< crimild::UInt32 >( deviceExtensions.size() ),
        .ppEnabledExtensionNames = deviceExtensions.data(),
    };

    if ( utils::checkValidationLayersEnabled() ) {
        // New Vulkan implementations seem to be ignoring validation layers per device
        // Still, it might be a good idea to register them here for backward compatibility
        const auto &validationLayers = utils::getValidationLayers();
        createInfo.enabledLayerCount = static_cast< crimild::UInt32 >( validationLayers.size() );
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    VkDevice deviceHandler;
    CRIMILD_VULKAN_CHECK(
		vkCreateDevice(
       		physicalDevice->handler,
           	&createInfo,
           	nullptr,
           	&deviceHandler
       	)
	);

    // Fetch device queues
    VkQueue graphisQueueHandler, presentQueueHandler;
    vkGetDeviceQueue( deviceHandler, indices.graphicsFamily[ 0 ], 0, &graphisQueueHandler );
    vkGetDeviceQueue( deviceHandler, indices.presentFamily[ 0 ], 0, &presentQueueHandler );

    auto renderDevice = crimild::alloc< RenderDevice >();
    renderDevice->handler = deviceHandler;
    renderDevice->physicalDevice = physicalDevice;
    renderDevice->surface = surface;
    renderDevice->manager = this;
    renderDevice->graphicsQueue = graphisQueueHandler;
    renderDevice->presentQueue = presentQueueHandler;
    insert( crimild::get_ptr( renderDevice ) );

    return renderDevice;
}

void RenderDeviceManager::destroy( RenderDevice *renderDevice ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan logical device" );

    static_cast< DescriptorPoolManager * >( renderDevice )->clear();
    static_cast< DescriptorSetManager * >( renderDevice )->clear();
    static_cast< DescriptorSetLayoutManager * >( renderDevice )->clear();
    static_cast< BufferManager * >( renderDevice )->clear();
    static_cast< FenceManager * >( renderDevice )->cleanup();
    static_cast< SemaphoreManager * >( renderDevice )->cleanup();
    static_cast< CommandBufferManager * >( renderDevice )->clear();
    static_cast< CommandPoolManager * >( renderDevice )->cleanup();
    static_cast< FramebufferManager * >( renderDevice )->cleanup();
    static_cast< ShaderModuleManager * >( renderDevice )->cleanup();
    static_cast< PipelineManager * >( renderDevice )->clear();
    static_cast< PipelineLayoutManager * >( renderDevice )->cleanup();
    static_cast< RenderPassManager * >( renderDevice )->cleanup();
    static_cast< ImageViewManager * >( renderDevice )->cleanup();
    static_cast< ImageManager * >( renderDevice )->cleanup();
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
