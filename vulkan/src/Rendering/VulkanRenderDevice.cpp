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

#include "Rendering/VulkanRenderDevice.hpp"

#include "Foundation/Log.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanSurface.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanFence.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanInstance.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanSwapchain.hpp"

#include <array>
#include <set>

using namespace crimild;
using namespace crimild::vulkan;

const int CRIMILD_MAX_FRAMES_IN_FLIGHT = 2;

RenderDevice::RenderDevice( PhysicalDevice *physicalDevice, VulkanSurface *surface, const Extent2D &extent ) noexcept
    : m_physicalDevice( physicalDevice ),
      m_surface( surface ),
      m_extent( extent )
{
    CRIMILD_LOG_TRACE( "Creating Vulkan logical device" );

    auto indices = utils::findQueueFamilies( physicalDevice->getHandle(), surface->getHandle() );
    if ( !indices.isComplete() ) {
        // Should never happen
        CRIMILD_LOG_FATAL( "Invalid physical device" );
        exit( EXIT_FAILURE );
    }

    // Make sure we're creating queue for unique families,
    // since both graphics and presenatation might be same family
    // \see utils::findQueueFamilies()
    std::unordered_set< crimild::UInt32 > uniqueQueueFamilies = {
        indices.graphicsFamily[ 0 ],
        indices.computeFamily[ 0 ],
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
        .fillModeNonSolid = VK_TRUE,
        .samplerAnisotropy = VK_TRUE,
    };

    const auto &deviceExtensions = utils::getDeviceExtensions();
    // deviceExtensions.push_back( "VK_KHR_portability_subset" );

    auto createInfo = VkDeviceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast< crimild::UInt32 >( queueCreateInfos.size() ),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .enabledExtensionCount = static_cast< crimild::UInt32 >( deviceExtensions.size() ),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures,
    };

    if ( utils::checkValidationLayersEnabled() ) {
        // New Vulkan implementations seem to be ignoring validation layers per device
        // Still, it might be a good idea to register them here for backward compatibility
        const auto &validationLayers = utils::getValidationLayers();
        createInfo.enabledLayerCount = static_cast< crimild::UInt32 >( validationLayers.size() );
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    CRIMILD_VULKAN_CHECK(
        vkCreateDevice(
            physicalDevice->getHandle(),
            &createInfo,
            nullptr,
            &m_handle ) );

    // Fetch device queues
    vkGetDeviceQueue( m_handle, indices.graphicsFamily[ 0 ], 0, &m_graphicsQueueHandle );
    vkGetDeviceQueue( m_handle, indices.computeFamily[ 0 ], 0, &m_computeQueueHandle );
    vkGetDeviceQueue( m_handle, indices.presentFamily[ 0 ], 0, &m_presentQueueHandle );

    createCommandPool( m_commandPool );

    createSwapchain();

    createSyncObjects();

    // Allocate one command buffer per swapchain image
    m_commandBuffers.resize( m_swapchainImages.size() );
    for ( auto &commandBuffer : m_commandBuffers ) {
        createCommandBuffer( commandBuffer );
    }
}

RenderDevice::~RenderDevice( void ) noexcept
{
    for ( auto &commandBuffer : m_commandBuffers ) {
        destroyCommandBuffer( commandBuffer );
    }

    destroyCommandPool( m_commandPool );

    destroySyncObjects();

    destroySwapchain();

    CRIMILD_LOG_TRACE( "Destroying Vulkan logical device" );

    if ( m_handle != VK_NULL_HANDLE ) {
        vkDestroyDevice( m_handle, nullptr );
    }

    m_handle = VK_NULL_HANDLE;
    m_physicalDevice = nullptr;
    m_graphicsQueueHandle = VK_NULL_HANDLE;
    m_computeQueueHandle = VK_NULL_HANDLE;
    m_presentQueueHandle = VK_NULL_HANDLE;
}

void RenderDevice::createSwapchain( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan swapchain" );

    auto swapchainSupport = utils::querySwapchainSupportDetails( m_physicalDevice->getHandle(), m_surface->getHandle() );
    auto surfaceFormat = utils::chooseSurfaceFormat( swapchainSupport.formats );
    auto presentMode = utils::choosePresentationMode( swapchainSupport.presentModes );

    m_swapchainExtent = utils::chooseExtent( swapchainSupport.capabilities, utils::getExtent( m_extent ) );
    m_swapchainFormat = surfaceFormat.format;

    // The Vulkan implementation defines a minimum number of images to work with.
    // We request one more image than the minimum. Also, make sure not to exceed the
    // maximum number of images (a value of 0 means there's no maximum)
    auto imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if ( swapchainSupport.capabilities.maxImageCount > 0 ) {
        imageCount = std::min(
            swapchainSupport.capabilities.maxImageCount,
            imageCount );
    }

    auto createInfo = VkSwapchainCreateInfoKHR {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = m_surface->getHandle(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = m_swapchainExtent,
        .imageArrayLayers = 1, // This may change for stereoscopic rendering (VR)
    };

    // Images created by the swapchain are usually used as color attachments
    // It might be possible to used the for other purposes, like sampling or
    // to copy to or from them to other surfaces.
    // VK_IMAGE_USAGE_TRANSFER_SRC_BIT is requried for taking screenshots.
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    auto indices = utils::findQueueFamilies( m_physicalDevice->getHandle(), m_surface->getHandle() );
    uint32_t queueFamilyIndices[] = {
        indices.graphicsFamily[ 0 ],
        indices.presentFamily[ 0 ],
    };

    if ( indices.graphicsFamily != indices.presentFamily ) {
        // If the graphics and present families don't match we need to
        // share images since they're not exclusive to queues
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        // If they are the same, the queue can have exclusive access to images
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    // This can be used to rotate images or flip them horizontally
    createInfo.preTransform = swapchainSupport.capabilities.currentTransform;

    // Use for blending with other windows in the window system
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;

    // Ignore pixels that are obscured by other windows
    createInfo.clipped = VK_TRUE;

    // This is used during the recreation of a swapchain (maybe due to window is resized)
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    CRIMILD_VULKAN_CHECK(
        vkCreateSwapchainKHR(
            getHandle(),
            &createInfo,
            nullptr,
            &m_swapchain ) );

    CRIMILD_LOG_TRACE( "Retrieving Vulkan swapchain images" );

    vkGetSwapchainImagesKHR(
        getHandle(),
        m_swapchain,
        &imageCount,
        nullptr );

    m_swapchainImages.resize( imageCount );

    vkGetSwapchainImagesKHR(
        getHandle(),
        m_swapchain,
        &imageCount,
        m_swapchainImages.data() );

    CRIMILD_LOG_TRACE( "Creating Vulkan swapchain image views" );

    m_swapchainImageViews.resize( imageCount );

    for ( uint8_t i = 0; i < imageCount; ++i ) {
        utils::createImageView( getHandle(), m_swapchainImages[ i ], VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &m_swapchainImageViews[ i ] );
    }

    CRIMILD_LOG_INFO( "Created Vulkan Swapchain with extents ", m_swapchainExtent.width, "x", m_swapchainExtent.height );
}

void RenderDevice::destroySwapchain( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan swapchain image views" );

    for ( auto &imageView : m_swapchainImageViews ) {
        vkDestroyImageView( getHandle(), imageView, nullptr );
    }
    m_swapchainImageViews.clear();

    CRIMILD_LOG_TRACE( "Destroying Vulkan swapchain image" );

    m_swapchainImages.clear();

    CRIMILD_LOG_TRACE( "Destroying Vulkan swapchain" );

    vkDestroySwapchainKHR( getHandle(), m_swapchain, nullptr );
    m_swapchain = VK_NULL_HANDLE;
}

void RenderDevice::createSyncObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan sync objects" );

    m_imageAvailableSemaphores.resize( CRIMILD_MAX_FRAMES_IN_FLIGHT );
    m_renderFinishedSemaphores.resize( CRIMILD_MAX_FRAMES_IN_FLIGHT );
    m_inFlightFences.resize( CRIMILD_MAX_FRAMES_IN_FLIGHT );
    m_imagesInFlight.resize( m_swapchainImages.size(), VK_NULL_HANDLE );

    auto createInfo = VkSemaphoreCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    auto fenceInfo = VkFenceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for ( int i = 0; i < CRIMILD_MAX_FRAMES_IN_FLIGHT; ++i ) {
        CRIMILD_VULKAN_CHECK(
            vkCreateSemaphore(
                m_handle,
                &createInfo,
                nullptr,
                &m_imageAvailableSemaphores[ i ] ) );

        CRIMILD_VULKAN_CHECK(
            vkCreateSemaphore(
                m_handle,
                &createInfo,
                nullptr,
                &m_renderFinishedSemaphores[ i ] ) );

        CRIMILD_VULKAN_CHECK(
            vkCreateFence(
                m_handle,
                &fenceInfo,
                nullptr,
                &m_inFlightFences[ i ] ) );
    }
}

void RenderDevice::destroySyncObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan sync objects" );

    // Wait for all operations to complete before destroying sync objects
    vkDeviceWaitIdle( m_handle );

    for ( auto &s : m_imageAvailableSemaphores ) {
        vkDestroySemaphore( m_handle, s, nullptr );
    }
    m_imageAvailableSemaphores.clear();

    for ( auto &s : m_renderFinishedSemaphores ) {
        vkDestroySemaphore( m_handle, s, nullptr );
    }
    m_renderFinishedSemaphores.clear();

    for ( auto &f : m_inFlightFences ) {
        vkDestroyFence( m_handle, f, nullptr );
    }
    m_inFlightFences.clear();
}

void RenderDevice::createCommandPool( VkCommandPool &commandPool ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan command pool" );

    // Is this too performance intensive to do it every time?
    auto queueFamilyIndices = utils::findQueueFamilies( getPhysicalDevice()->getHandle(), getSurface()->getHandle() );
    auto createInfo = VkCommandPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        // TODO(hernan): For compute/transfer, we might want to use a different queue, right?
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily[ 0 ],
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateCommandPool(
            m_handle,
            &createInfo,
            nullptr,
            &commandPool ) );
}

void RenderDevice::destroyCommandPool( VkCommandPool &commandPool ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan command pool" );

    if ( commandPool != VK_NULL_HANDLE ) {
        vkDestroyCommandPool(
            m_handle,
            commandPool,
            nullptr );
        m_commandPool = VK_NULL_HANDLE;
    }
}

void RenderDevice::createCommandBuffer( VkCommandBuffer &commandBuffer ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan command buffer" );

    auto allocInfo = VkCommandBufferAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateCommandBuffers(
            m_handle,
            &allocInfo,
            &commandBuffer ) );
}

void RenderDevice::destroyCommandBuffer( VkCommandBuffer &commandBuffer ) noexcept
{
    if ( commandBuffer == VK_NULL_HANDLE ) {
        return;
    }

    CRIMILD_LOG_TRACE( "Destroying Vulkan command buffer" );

    // renderDevice->waitIdle();
    vkFreeCommandBuffers(
        m_handle,
        m_commandPool,
        1,
        &commandBuffer );

    commandBuffer = VK_NULL_HANDLE;
}

void RenderDevice::beginRender( void ) noexcept
{
    CRIMILD_VULKAN_CHECK( vkWaitForFences( m_handle, 1, &m_inFlightFences[ m_currentFrame ], VK_TRUE, UINT64_MAX ) );

    const auto ret = vkAcquireNextImageKHR(
        getHandle(),
        m_swapchain,
        std::numeric_limits< uint64_t >::max(), // disable timeout
        m_imageAvailableSemaphores[ m_currentFrame ],
        VK_NULL_HANDLE,
        &m_imageIndex );
    if ( ret == VK_ERROR_OUT_OF_DATE_KHR ) {
        // TODO: swapchain needs to be recreated
        return;
    }

    if ( ret != VK_SUCCESS ) {
        // no available image index. Skip frame
        return;
    }

    // Wait for any previous frame that is using the image that we've just been assigned for the new frame
    if ( m_imagesInFlight[ m_imageIndex ] != VK_NULL_HANDLE ) {
        vkWaitForFences( m_handle, 1, &m_imagesInFlight[ m_imageIndex ], VK_TRUE, UINT64_MAX );
    }
    m_imagesInFlight[ m_imageIndex ] = m_inFlightFences[ m_currentFrame ];

    auto commandBuffer = getCurrentCommandBuffer();

    CRIMILD_VULKAN_CHECK(
        vkResetCommandBuffer(
            commandBuffer,
            VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT ) );

    const auto beginInfo = VkCommandBufferBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        .pInheritanceInfo = nullptr,
    };

    CRIMILD_VULKAN_CHECK(
        vkBeginCommandBuffer(
            commandBuffer,
            &beginInfo ) );
}

void RenderDevice::endRender( void ) noexcept
{
    auto commandBuffer = getCurrentCommandBuffer();
    vkEndCommandBuffer( commandBuffer );

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submitInfo.pWaitDstStageMask = &waitStageMask;
    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[ m_currentFrame ] };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[ m_currentFrame ] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences( m_handle, 1, &m_inFlightFences[ m_currentFrame ] );

    CRIMILD_VULKAN_CHECK(
        vkQueueSubmit(
            m_graphicsQueueHandle,
            1,
            &submitInfo,
            m_inFlightFences[ m_currentFrame ] ) );

    VkSwapchainKHR swapchains[] = { m_swapchain };

    auto presentInfo = VkPresentInfoKHR {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &m_imageIndex,
        .pResults = nullptr,
    };

    auto ret = vkQueuePresentKHR( m_presentQueueHandle, &presentInfo );
    if ( ret == VK_ERROR_OUT_OF_DATE_KHR || ret == VK_SUBOPTIMAL_KHR ) {
        // swapchain needs to be recreated
        return;
    }

    // Advance to the next frame
    m_currentFrame = ( m_currentFrame + 1 ) % CRIMILD_MAX_FRAMES_IN_FLIGHT;
}

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
