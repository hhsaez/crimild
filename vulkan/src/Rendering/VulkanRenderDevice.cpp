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

#include "Rendering/IndexBuffer.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanSurface.hpp"
#include "Simulation/Event.hpp"

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
    CRIMILD_LOG_TRACE();

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
    createCommandBuffers();
}

RenderDevice::~RenderDevice( void ) noexcept
{
    for ( auto &it : m_buffers ) {
        for ( auto handler : it.second ) {
            vkDestroyBuffer( m_handle, handler, nullptr );
        }
    }
    m_buffers.clear();

    for ( auto &it : m_memories ) {
        for ( auto memory : it.second ) {
            vkFreeMemory( m_handle, memory, nullptr );
        }
    }
    m_memories.clear();

    destroyCommandBuffers();
    destroySyncObjects();
    destroySwapchain();

    destroyCommandPool( m_commandPool );

    CRIMILD_LOG_TRACE();

    if ( m_handle != VK_NULL_HANDLE ) {
        vkDestroyDevice( m_handle, nullptr );
    }

    m_handle = VK_NULL_HANDLE;
    m_physicalDevice = nullptr;
    m_graphicsQueueHandle = VK_NULL_HANDLE;
    m_computeQueueHandle = VK_NULL_HANDLE;
    m_presentQueueHandle = VK_NULL_HANDLE;
}

void RenderDevice::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            m_extent = e.extent;
            vkDeviceWaitIdle( m_handle );

            destroyCommandBuffers();
            destroySyncObjects();
            destroySwapchain();

            createSwapchain();
            createSyncObjects();
            createCommandBuffers();
            m_imageIndex = 0;

            m_currentFrame = 0;

            break;
        }

        default: {
            break;
        }
    }
}

void RenderDevice::flush( void ) noexcept
{
    vkDeviceWaitIdle( m_handle );
}

void RenderDevice::createSwapchain( void ) noexcept
{
    CRIMILD_LOG_TRACE();

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

    CRIMILD_LOG_TRACE();

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

    CRIMILD_LOG_TRACE();

    m_swapchainImageViews.resize( imageCount );

    for ( uint8_t i = 0; i < imageCount; ++i ) {
        utils::createImageView( getHandle(), m_swapchainImages[ i ], VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &m_swapchainImageViews[ i ] );
    }

    CRIMILD_LOG_INFO( "Created Vulkan Swapchain with extents ", m_swapchainExtent.width, "x", m_swapchainExtent.height );
}

void RenderDevice::destroySwapchain( void ) noexcept
{
    CRIMILD_LOG_DEBUG( "Destroying Vulkan swapchain image views" );

    for ( auto &imageView : m_swapchainImageViews ) {
        vkDestroyImageView( getHandle(), imageView, nullptr );
    }
    m_swapchainImageViews.clear();

    CRIMILD_LOG_DEBUG( "Destroying Vulkan swapchain image" );

    m_swapchainImages.clear();

    CRIMILD_LOG_DEBUG( "Destroying Vulkan swapchain" );

    vkDestroySwapchainKHR( getHandle(), m_swapchain, nullptr );
    m_swapchain = VK_NULL_HANDLE;
}

void RenderDevice::createSyncObjects( void ) noexcept
{
    CRIMILD_LOG_TRACE();

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
    CRIMILD_LOG_TRACE();

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

    m_imagesInFlight.clear();
}

void RenderDevice::createCommandPool( VkCommandPool &commandPool ) noexcept
{
    CRIMILD_LOG_TRACE();

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
    CRIMILD_LOG_TRACE();

    if ( commandPool != VK_NULL_HANDLE ) {
        vkDestroyCommandPool(
            m_handle,
            commandPool,
            nullptr );
        m_commandPool = VK_NULL_HANDLE;
    }
}

void RenderDevice::createCommandBuffers( void ) noexcept
{
    // Allocate one command buffer per swapchain image
    m_commandBuffers.resize( m_swapchainImages.size() );
    for ( auto &commandBuffer : m_commandBuffers ) {
        createCommandBuffer( commandBuffer );
    }
}

void RenderDevice::destroyCommandBuffers( void ) noexcept
{
    for ( auto &commandBuffer : m_commandBuffers ) {
        destroyCommandBuffer( commandBuffer );
    }
}

void RenderDevice::createCommandBuffer( VkCommandBuffer &commandBuffer ) noexcept
{
    CRIMILD_LOG_TRACE();

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

    CRIMILD_LOG_TRACE();

    // renderDevice->waitIdle();
    vkFreeCommandBuffers(
        m_handle,
        m_commandPool,
        1,
        &commandBuffer );

    commandBuffer = VK_NULL_HANDLE;
}

bool RenderDevice::beginRender( void ) noexcept
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
        return false;
    }

    if ( ret != VK_SUCCESS ) {
        // no available image index. Skip frame
        return false;
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

    return true;
}

bool RenderDevice::endRender( void ) noexcept
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
        return false;
    }

    // Advance to the next frame
    m_currentFrame = ( m_currentFrame + 1 ) % CRIMILD_MAX_FRAMES_IN_FLIGHT;

    return true;
}

void RenderDevice::createBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer &bufferHandler,
    VkDeviceMemory &bufferMemory ) const noexcept
{
    auto createInfo = VkBufferCreateInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateBuffer(
            m_handle,
            &createInfo,
            nullptr,
            &bufferHandler ) );

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements( m_handle, bufferHandler, &memRequirements );

    auto allocInfo = VkMemoryAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = getPhysicalDevice()->findMemoryType( memRequirements.memoryTypeBits, properties ),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateMemory(
            m_handle,
            &allocInfo,
            nullptr,
            &bufferMemory ) );

    CRIMILD_VULKAN_CHECK(
        vkBindBufferMemory(
            m_handle,
            bufferHandler,
            bufferMemory,
            0 ) );
}

void RenderDevice::copyToBuffer( VkDeviceMemory &memory, const void *data, VkDeviceSize size ) const noexcept
{
    void *dstData = nullptr;

    CRIMILD_VULKAN_CHECK(
        vkMapMemory(
            m_handle,
            memory,
            0,
            size,
            0,
            &dstData ) );

    memcpy( dstData, data, ( size_t ) size );

    vkUnmapMemory( m_handle, memory );
}

bool RenderDevice::bind( UniformBuffer *uniformBuffer ) noexcept
{
    // TODO(hernan): this is assuming buffers are static. For dynamic buffers,
    // we want to create one handle/memory pair per swapchain image

    const auto id = uniformBuffer->getUniqueID();
    if ( m_buffers.contains( id ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE();

    auto bufferView = uniformBuffer->getBufferView();
    auto bufferSize = bufferView->getLength();

    VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    for ( size_t i = 0; i < getSwapchainImageCount(); ++i ) {
        VkBuffer bufferHandler;
        VkDeviceMemory bufferMemory;

        createBuffer(
            bufferSize,
            usage,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferHandler,
            bufferMemory );

        if ( bufferView->getData() != nullptr ) {
            copyToBuffer(
                bufferMemory,
                bufferView->getData(),
                bufferSize );
        }

        m_buffers[ id ].push_back( bufferHandler );
        m_memories[ id ].push_back( bufferMemory );
    }

    return true;
}

void RenderDevice::unbind( UniformBuffer *uniformBuffer ) noexcept
{
    const auto id = uniformBuffer->getUniqueID();
    if ( !m_buffers.contains( id ) ) {
        return;
    }

    CRIMILD_LOG_TRACE();

    auto &buffers = m_buffers[ id ];
    for ( auto bufferHandler : buffers ) {
        vkDestroyBuffer( m_handle, bufferHandler, nullptr );
    }

    auto &memories = m_memories[ id ];
    for ( auto memory : memories ) {
        vkFreeMemory( m_handle, memory, nullptr );
    }

    m_buffers.erase( id );
    m_memories.erase( id );
}

VkBuffer RenderDevice::getHandle( UniformBuffer *uniformBuffer, Index imageIndex ) const noexcept
{
    const auto id = uniformBuffer->getUniqueID();

    if ( !m_buffers.contains( id ) ) {
        return VK_NULL_HANDLE;
    }

    return m_buffers.at( id )[ imageIndex ];
}

void RenderDevice::update( UniformBuffer *uniformBuffer ) const noexcept
{
    const auto id = uniformBuffer->getUniqueID();

    auto bufferView = uniformBuffer->getBufferView();
    auto bufferSize = bufferView->getLength();

    auto bufferMemory = m_memories.at( id )[ getCurrentFrameIndex() ];

    if ( bufferView->getData() != nullptr ) {
        copyToBuffer(
            bufferMemory,
            bufferView->getData(),
            bufferSize );
    }
}

VkBuffer RenderDevice::bind( VertexBuffer *vertexBuffer ) noexcept
{
    const auto id = vertexBuffer->getUniqueID();
    if ( m_buffers.contains( id ) ) {
        // TODO(hernan): Handle dynamic buffers by updating the buffer here if needed before returning its handler(?)
        return m_buffers[ id ][ getCurrentFrameIndex() ];
    }

    CRIMILD_LOG_TRACE();

    auto bufferView = vertexBuffer->getBufferView();
    auto bufferSize = bufferView->getLength();

    VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    for ( size_t i = 0; i < getSwapchainImageCount(); ++i ) {
        VkBuffer bufferHandler;
        VkDeviceMemory bufferMemory;

        createBuffer(
            bufferSize,
            usage,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferHandler,
            bufferMemory );

        if ( bufferView->getData() != nullptr ) {
            copyToBuffer(
                bufferMemory,
                bufferView->getData(),
                bufferSize );
        }

        m_buffers[ id ].push_back( bufferHandler );
        m_memories[ id ].push_back( bufferMemory );
    }

    return m_buffers[ id ][ getCurrentFrameIndex() ];
}

void RenderDevice::unbind( VertexBuffer *vertexBuffer ) noexcept
{
    const auto id = vertexBuffer->getUniqueID();
    if ( !m_buffers.contains( id ) ) {
        return;
    }

    CRIMILD_LOG_TRACE();

    auto &buffers = m_buffers[ id ];
    for ( auto bufferHandler : buffers ) {
        vkDestroyBuffer( m_handle, bufferHandler, nullptr );
    }

    auto &memories = m_memories[ id ];
    for ( auto memory : memories ) {
        vkFreeMemory( m_handle, memory, nullptr );
    }

    m_buffers.erase( id );
    m_memories.erase( id );
}

VkBuffer RenderDevice::bind( IndexBuffer *indexBuffer ) noexcept
{
    const auto id = indexBuffer->getUniqueID();
    if ( m_buffers.contains( id ) ) {
        // TODO(hernan): Handle dynamic buffers by updating the buffer here if needed before returning its handler(?)
        return m_buffers[ id ][ getCurrentFrameIndex() ];
    }

    CRIMILD_LOG_TRACE();

    auto bufferView = indexBuffer->getBufferView();
    auto bufferSize = bufferView->getLength();

    VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    for ( size_t i = 0; i < getSwapchainImageCount(); ++i ) {
        VkBuffer bufferHandler;
        VkDeviceMemory bufferMemory;

        createBuffer(
            bufferSize,
            usage,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferHandler,
            bufferMemory );

        if ( bufferView->getData() != nullptr ) {
            copyToBuffer(
                bufferMemory,
                bufferView->getData(),
                bufferSize );
        }

        m_buffers[ id ].push_back( bufferHandler );
        m_memories[ id ].push_back( bufferMemory );
    }

    return m_buffers[ id ][ getCurrentFrameIndex() ];
}

void RenderDevice::unbind( IndexBuffer *indexBuffer ) noexcept
{
    const auto id = indexBuffer->getUniqueID();
    if ( !m_buffers.contains( id ) ) {
        return;
    }

    CRIMILD_LOG_TRACE();

    auto &buffers = m_buffers[ id ];
    for ( auto bufferHandler : buffers ) {
        vkDestroyBuffer( m_handle, bufferHandler, nullptr );
    }

    auto &memories = m_memories[ id ];
    for ( auto memory : memories ) {
        vkFreeMemory( m_handle, memory, nullptr );
    }

    m_buffers.erase( id );
    m_memories.erase( id );
}
