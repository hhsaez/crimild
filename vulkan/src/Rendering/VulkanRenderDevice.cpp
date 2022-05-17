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
    createDepthStencilResources();
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

    for ( auto &it : m_imageViews ) {
        for ( auto imageView : it.second ) {
            vkDestroyImageView( m_handle, imageView, nullptr );
        }
    }
    m_imageViews.clear();

    for ( auto &it : m_images ) {
        for ( auto image : it.second ) {
            vkDestroyImage( m_handle, image, nullptr );
        }
    }
    m_images.clear();

    for ( auto &it : m_samplers ) {
        for ( auto sampler : it.second ) {
            vkDestroySampler( m_handle, sampler, nullptr );
        }
    }
    m_samplers.clear();

    for ( auto &it : m_memories ) {
        for ( auto memory : it.second ) {
            vkFreeMemory( m_handle, memory, nullptr );
        }
    }
    m_memories.clear();

    destroyCommandBuffers();
    destroySyncObjects();
    destroyDepthStencilResources();
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
            destroyDepthStencilResources();
            destroySwapchain();

            createSwapchain();
            createDepthStencilResources();
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
        setObjectName(
            m_swapchainImageViews[ i ],
            StringUtils::toString( "RenderDevice::swapchainImageView[", uint32_t( i ), "]" ).c_str() );
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

void RenderDevice::createDepthStencilResources( void ) noexcept
{
    m_depthStencilResources.format = m_physicalDevice->findSupportedFormat(
        {
            // Important: These formats must be sorted by priority
            // Only the very first one that is found is going to
            // be used in the end.
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D32_SFLOAT,
        },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );

    createImage(
        m_swapchainExtent.width,
        m_swapchainExtent.height,
        m_depthStencilResources.format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        1,
        VK_SAMPLE_COUNT_1_BIT,
        1,
        0,
        m_depthStencilResources.image,
        m_depthStencilResources.memory );

    utils::createImageView(
        getHandle(),
        m_depthStencilResources.image,
        m_depthStencilResources.format,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &m_depthStencilResources.imageView );
}

void RenderDevice::destroyDepthStencilResources( void ) noexcept
{
    vkDestroyImageView( m_handle, m_depthStencilResources.imageView, nullptr );
    m_depthStencilResources.imageView = VK_NULL_HANDLE;

    vkDestroyImage( m_handle, m_depthStencilResources.image, nullptr );
    m_depthStencilResources.image = VK_NULL_HANDLE;

    vkFreeMemory( m_handle, m_depthStencilResources.memory, nullptr );
    m_depthStencilResources.memory = VK_NULL_HANDLE;

    m_depthStencilResources.format = VK_FORMAT_UNDEFINED;
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
        // TODO: Instad of using copyToBuffer (which requires a vkMapMemory call), we should
        // append these commands to the current command buffer. Better still, we should use
        // a transfer queue if possible and add synchronization barriers as needed.
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
        if ( vertexBuffer->getBufferView()->getUsage() == BufferView::Usage::DYNAMIC ) {
            copyToBuffer(
                m_memories[ id ][ getCurrentFrameIndex() ],
                vertexBuffer->getBufferView()->getData(),
                vertexBuffer->getBufferView()->getLength() );
        }

        return m_buffers[ id ][ getCurrentFrameIndex() ];
    }

    CRIMILD_LOG_TRACE();

    auto bufferView = vertexBuffer->getBufferView();
    auto bufferSize = bufferView->getLength();

    VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    for ( size_t i = 0; i < getSwapchainImageCount(); ++i ) {
        VkBuffer bufferHandler;
        VkDeviceMemory bufferMemory;

        // TODO: for static buffers, we should use staging buffers instead and send the data to the GPU only once.

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
        if ( indexBuffer->getBufferView()->getUsage() == BufferView::Usage::DYNAMIC ) {
            copyToBuffer(
                m_memories[ id ][ getCurrentFrameIndex() ],
                indexBuffer->getBufferView()->getData(),
                indexBuffer->getBufferView()->getLength() );
        }
        return m_buffers[ id ][ getCurrentFrameIndex() ];
    }

    CRIMILD_LOG_TRACE();

    auto bufferView = indexBuffer->getBufferView();
    auto bufferSize = bufferView->getLength();

    VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    for ( size_t i = 0; i < getSwapchainImageCount(); ++i ) {
        VkBuffer bufferHandler;
        VkDeviceMemory bufferMemory;

        // TODO: for static buffers, we should use staging buffers instead and send the data to the GPU only once.

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

VkCommandBuffer RenderDevice::beginSingleTimeCommands( void ) const noexcept
{
    auto allocInfo = VkCommandBufferAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;

    CRIMILD_VULKAN_CHECK(
        vkAllocateCommandBuffers(
            m_handle,
            &allocInfo,
            &commandBuffer ) );

    auto beginInfo = VkCommandBufferBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    vkBeginCommandBuffer( commandBuffer, &beginInfo );

    return commandBuffer;
}

void RenderDevice::endSingleTimeCommands( VkCommandBuffer commandBuffer ) const noexcept
{
    vkEndCommandBuffer( commandBuffer );

    auto submitInfo = VkSubmitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    CRIMILD_VULKAN_CHECK(
        vkQueueSubmit(
            m_graphicsQueueHandle,
            1,
            &submitInfo,
            nullptr ) );

    CRIMILD_VULKAN_CHECK(
        vkQueueWaitIdle(
            m_graphicsQueueHandle ) );

    vkFreeCommandBuffers(
        m_handle,
        m_commandPool,
        1,
        &commandBuffer );
}

void RenderDevice::transitionImageLayout( VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, crimild::UInt32 mipLevels, crimild::UInt32 layerCount ) const noexcept
{
    auto commandBuffer = beginSingleTimeCommands();
    transitionImageLayout( commandBuffer, image, format, oldLayout, newLayout, mipLevels, layerCount );
    endSingleTimeCommands( commandBuffer );
}

void RenderDevice::transitionImageLayout( VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, crimild::UInt32 mipLevels, crimild::UInt32 layerCount ) const noexcept
{
    auto barrier = VkImageMemoryBarrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = 0, // See below
        .dstAccessMask = 0, // See below
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = VkImageSubresourceRange {
            .aspectMask = 0, // Defined below
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = layerCount,
        },
    };

    VkPipelineStageFlags sourceStage = 0;
    VkPipelineStageFlags destinationStage = 0;

    if ( oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if ( utils::hasStencilComponent( format ) ) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        CRIMILD_LOG_ERROR( "Unsupported Vulkan Layout Transition" );
        CRIMILD_VULKAN_CHECK( VK_ERROR_FORMAT_NOT_SUPPORTED );
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier );
}

void RenderDevice::copyBufferToImage( VkBuffer buffer, VkImage image, crimild::UInt32 width, crimild::UInt32 height, UInt32 layerCount ) const noexcept
{
    auto commandBuffer = beginSingleTimeCommands();

    auto region = VkBufferImageCopy {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = layerCount,
        },
        .imageOffset = { 0, 0 },
        .imageExtent = {
            .width = width,
            .height = height,
            .depth = 1,
        },
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region );

    endSingleTimeCommands( commandBuffer );
}

void RenderDevice::generateMipmaps( VkImage image, VkFormat imageFormat, crimild::Int32 width, crimild::Int32 height, crimild::UInt32 mipLevels ) const noexcept
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties( getPhysicalDevice()->getHandle(), imageFormat, &formatProperties );
    if ( !( formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT ) ) {
        CRIMILD_LOG_FATAL( "Texture image format does not support linear blitting!" );
        exit( -1 );
    }

    auto commandBuffer = beginSingleTimeCommands();

    auto barrier = VkImageMemoryBarrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    auto mipWidth = width;
    auto mipHeight = height;

    for ( crimild::UInt32 i = 1; i < mipLevels; i++ ) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier );

        auto blit = VkImageBlit {
            .srcSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = i - 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .srcOffsets = {
                { 0, 0, 0 },
                { mipWidth, mipHeight, 1 },
            },
            .dstSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = i,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .dstOffsets = {
                { 0, 0, 0 },
                {
                    mipWidth > 1 ? mipWidth / 2 : 1,
                    mipHeight > 1 ? mipHeight / 2 : 1,
                    1,
                },
            },
        };

        vkCmdBlitImage(
            commandBuffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &blit,
            VK_FILTER_LINEAR );

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier );

        if ( mipWidth > 1 )
            mipWidth /= 2;
        if ( mipHeight > 1 )
            mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier );

    endSingleTimeCommands( commandBuffer );
}

void RenderDevice::createImage(
    crimild::UInt32 width,
    crimild::UInt32 height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memoryProperties,
    crimild::UInt32 mipLevels,
    VkSampleCountFlagBits numSamples,
    crimild::UInt32 arrayLayers,
    crimild::UInt32 flags,
    VkImage &image,
    VkDeviceMemory &imageMemory,
    void *imageData ) const noexcept
{
    auto createInfo = VkImageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = flags,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {
            .width = width,
            .height = height,
            .depth = 1,
        },
        .mipLevels = mipLevels,
        .arrayLayers = arrayLayers,
        .samples = numSamples,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateImage(
            m_handle,
            &createInfo,
            nullptr,
            &image ) );

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements( m_handle, image, &memRequirements );

    auto allocInfo = VkMemoryAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = m_physicalDevice->findMemoryType( memRequirements.memoryTypeBits, memoryProperties ),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateMemory(
            m_handle,
            &allocInfo,
            nullptr,
            &imageMemory ) );

    CRIMILD_VULKAN_CHECK(
        vkBindImageMemory(
            m_handle,
            image,
            imageMemory,
            0 ) );

    if ( imageData != nullptr ) {
        VkDeviceSize imageSize = memRequirements.size;
        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

        createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory );

        copyToBuffer( stagingBufferMemory, imageData, imageSize );

        transitionImageLayout(
            image,
            format,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            mipLevels,
            arrayLayers );

        copyBufferToImage(
            stagingBuffer,
            image,
            width,
            height,
            arrayLayers );

        transitionImageLayout(
            image,
            format,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            mipLevels,
            arrayLayers );

        vkDestroyBuffer(
            m_handle,
            stagingBuffer,
            nullptr );

        vkFreeMemory(
            m_handle,
            stagingBufferMemory,
            nullptr );
    }
}

void RenderDevice::createImageView( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView &imageView ) const noexcept
{
    utils::createImageView( m_handle, image, format, aspectFlags, &imageView );
}

VkImage RenderDevice::bind( const Image *image ) noexcept
{
    const auto id = image->getUniqueID();
    if ( m_images.contains( id ) ) {
        // TODO: support dynamic images
        return m_images[ id ][ 0 ];
    }

    CRIMILD_LOG_TRACE();

    uint32_t width = image->extent.width;
    uint32_t height = image->extent.height;
    if ( image->extent.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
        width *= m_swapchainExtent.width;
        height *= m_swapchainExtent.height;
    }

    auto mipLevels = image->getMipLevels();
    auto arrayLayers = image->getLayerCount();
    auto type = image->type;

    // TODO: use frame graph to set usage?
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    if ( image->getBufferView() != nullptr ) {
        // If image has data, it will be used for transfer operations
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    } else {
        // If image has no data, then it's used as an attachment
        if ( utils::formatIsColor( image->format ) ) {
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        } else if ( utils::formatIsDepthStencil( image->format ) ) {
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }

        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    auto createInfo = VkImageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = utils::getFormat( image->format ), // TODO: what about device formats (i.e. swapchain, depth, etc)
        .extent = {
            .width = width,
            .height = height,
            .depth = 1,
        },
        .mipLevels = mipLevels,
        .arrayLayers = arrayLayers,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VkImage imageHandle = VK_NULL_HANDLE;
    VkDeviceMemory imageMemoryHandle = VK_NULL_HANDLE;

    CRIMILD_VULKAN_CHECK(
        vkCreateImage(
            m_handle,
            &createInfo,
            nullptr,
            &imageHandle ) );

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements( m_handle, imageHandle, &memRequirements );

    auto allocInfo = VkMemoryAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = utils::findMemoryType(
            getPhysicalDevice()->getHandle(),
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateMemory(
            m_handle,
            &allocInfo,
            nullptr,
            &imageMemoryHandle ) );

    CRIMILD_VULKAN_CHECK(
        vkBindImageMemory(
            m_handle,
            imageHandle,
            imageMemoryHandle,
            0 ) );

    if ( image->getBufferView() != nullptr ) {
        // Image has pixel data. Upload it

        VkDeviceSize imageSize = image->getBufferView()->getLength();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

        createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory );

        copyToBuffer( stagingBufferMemory, image->getBufferView()->getData(), imageSize );

        transitionImageLayout(
            imageHandle,
            utils::getFormat( image->format ),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            mipLevels,
            arrayLayers );

        copyBufferToImage(
            stagingBuffer,
            imageHandle,
            width,
            height,
            arrayLayers );

        if ( type == Image::Type::IMAGE_2D_CUBEMAP ) {
            // No mipmaps. Transition to SHADER_READ_OPTIMAL
            transitionImageLayout(
                imageHandle,
                utils::getFormat( image->format ),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                mipLevels,
                arrayLayers );
        } else {
            // Automatically transitions to SHADER_READ_OPTIMAL layout
            generateMipmaps(
                imageHandle,
                utils::getFormat( image->format ),
                width,
                height,
                mipLevels );
        };

        // TODO: Support dynamic images!!
        // if ( image->getBufferView() == nullptr || image->getBufferView()->getUsage() == BufferView::Usage::STATIC ) {
        // We won't be using the staging buffers for static images anymore
        vkDestroyBuffer(
            m_handle,
            stagingBuffer,
            nullptr );

        vkFreeMemory(
            m_handle,
            stagingBufferMemory,
            nullptr );

        // bindInfo.stagingBuffer = VK_NULL_HANDLE;
        // bindInfo.stagingBufferMemory = VK_NULL_HANDLE;
        // }
    }

    if ( !image->getName().empty() ) {
        utils::setObjectName(
            m_handle,
            UInt64( imageHandle ),
            VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT,
            image->getName().c_str() );
    }

    m_images[ id ] = { imageHandle };
    m_memories[ id ] = { imageMemoryHandle };

    return m_images[ id ][ 0 ];
}

void RenderDevice::unbind( const Image *image ) noexcept
{
    const auto id = image->getUniqueID();
    if ( !m_images.contains( id ) ) {
        return;
    }

    CRIMILD_LOG_TRACE();

    for ( auto handle : m_images[ id ] ) {
        vkDestroyImage( m_handle, handle, nullptr );
    }

    for ( auto handle : m_memories[ id ] ) {
        vkFreeMemory( m_handle, handle, nullptr );
    }

    // if ( handler.stagingBuffer != VK_NULL_HANDLE ) {
    //     vkDestroyBuffer(
    //         renderDevice->handler,
    //         handler.stagingBuffer,
    //         nullptr );
    // }
    // if ( handler.imageMemoryHandler != VK_NULL_HANDLE ) {
    //     vkFreeMemory( renderDevice->handler, handler.stagingBufferMemory, nullptr );
    // }

    m_images.erase( id );
    m_memories.erase( id );
}

VkImageView RenderDevice::bind( const ImageView *imageView ) noexcept
{
    const auto id = imageView->getUniqueID();
    if ( m_imageViews.contains( id ) ) {
        return m_imageViews[ id ][ 0 ];
    }

    CRIMILD_LOG_TRACE();

    auto mipLevels = imageView->mipLevels;
    if ( mipLevels == 0 ) {
        mipLevels = Numerici::max( 1, imageView->image->getMipLevels() );
    }

    auto image = bind( imageView->image.get() );

    auto layerCount = imageView->layerCount;
    if ( layerCount == 0 ) {
        layerCount = imageView->image->getLayerCount();
    }

    auto viewInfo = VkImageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .flags = 0,
        .image = image,
        .viewType = utils::getImageViewType( imageView ),
        .format = [ & ] {
            auto format = imageView->format;
            if ( format == Format::UNDEFINED ) {
                format = imageView->image->format;
            }
            return utils::getFormat( format );
        }(),
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        .subresourceRange = {
            .aspectMask = utils::getImageViewAspectFlags( imageView ),
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = layerCount,
        },
    };

    VkImageView handle;
    CRIMILD_VULKAN_CHECK(
        vkCreateImageView(
            m_handle,
            &viewInfo,
            nullptr,
            &handle ) );

    m_imageViews[ id ] = { handle };

    return m_imageViews[ id ][ 0 ];
}

void RenderDevice::unbind( const ImageView *imageView ) noexcept
{
    const auto id = imageView->getUniqueID();
    if ( !m_imageViews.contains( id ) ) {
        return;
    }

    unbind( imageView->image.get() );

    CRIMILD_LOG_TRACE();

    for ( auto handle : m_imageViews[ id ] ) {
        vkDestroyImageView( m_handle, handle, nullptr );
    }

    m_imageViews.erase( id );
}

VkSampler RenderDevice::bind( const Sampler *sampler ) noexcept
{
    const auto id = sampler->getUniqueID();
    if ( m_samplers.contains( id ) ) {
        return m_samplers[ id ][ 0 ];
    }

    CRIMILD_LOG_TRACE();

    auto addressMode = utils::getSamplerAddressMode( sampler->getWrapMode() );
    auto compareOp = utils::getCompareOp( sampler->getCompareOp() );
    auto borderColor = utils::getBorderColor( sampler->getBorderColor() );
    auto minLod = crimild::Real32( sampler->getMinLod() );
    auto maxLod = crimild::Real32( sampler->getMaxLod() );

    auto samplerInfo = VkSamplerCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = utils::getSamplerFilter( sampler->getMagFilter() ),
        .minFilter = utils::getSamplerFilter( sampler->getMinFilter() ),
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = addressMode,
        .addressModeV = addressMode,
        .addressModeW = addressMode,
        .mipLodBias = 0,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = 16,
        .compareEnable = VK_FALSE,
        .compareOp = compareOp,
        .minLod = minLod,
        .maxLod = maxLod,
        .borderColor = borderColor,
        .unnormalizedCoordinates = VK_FALSE,
    };

    VkSampler handle = VK_NULL_HANDLE;
    CRIMILD_VULKAN_CHECK(
        vkCreateSampler(
            m_handle,
            &samplerInfo,
            nullptr,
            &handle ) );

    m_samplers[ id ] = { handle };

    return m_samplers[ id ][ 0 ];
}

void RenderDevice::unbind( const Sampler *sampler ) noexcept
{
    const auto id = sampler->getUniqueID();
    if ( !m_samplers.contains( id ) ) {
        return;
    }

    CRIMILD_LOG_TRACE();

    for ( auto handle : m_samplers[ id ] ) {
        vkDestroySampler( m_handle, handle, nullptr );
    }

    m_samplers.erase( id );
}

void RenderDevice::setObjectName( UInt64 object, VkDebugReportObjectTypeEXT objectType, std::string name ) const noexcept
{
#if defined( CRIMILD_PLATFORM_OSX )
    static auto vkDebugMarkerSetObjectName = ( PFN_vkDebugMarkerSetObjectNameEXT ) vkGetDeviceProcAddr( m_handle, "vkDebugMarkerSetObjectNameEXT" );
    if ( vkDebugMarkerSetObjectName == VK_NULL_HANDLE ) {
        CRIMILD_LOG_ERROR( "Cannot get procedure address for vkDebugMarkerSetObjectName" );
        return;
    }

    VkDebugMarkerObjectNameInfoEXT nameInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT,
        .objectType = objectType,
        .object = object,
        .pObjectName = name.c_str(),
    };

    vkDebugMarkerSetObjectName( m_handle, &nameInfo );
#endif
}

bool RenderDevice::formatIsColor( VkFormat format ) const
{
    return !formatIsDepthStencil( format );
}

bool RenderDevice::formatIsDepthStencil( VkFormat format ) const
{
    switch ( format ) {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
    }
}

VkViewport RenderDevice::getViewport( const ViewportDimensions &viewport ) const noexcept
{
    CRIMILD_LOG_TRACE();

    auto x = viewport.dimensions.origin.x;
    auto y = viewport.dimensions.origin.y;
    auto w = viewport.dimensions.size.width;
    auto h = viewport.dimensions.size.height;
    auto minD = viewport.depthRange.x;
    auto maxD = viewport.depthRange.y;

    if ( viewport.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
        const auto extent = getSwapchainExtent();
        x *= extent.width;
        y *= extent.height;
        w *= extent.width;
        h *= extent.height;
    }

    // Because Vulkan's coordinate system is different from Crimild's one,
    // we need to specify the viewport in a different way than usual.
    // WARNING: This trick requires VK_KHR_maintenance1 support (which should
    // be part of the core spec at the time of this writing).
    // See: https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/
    //
    // Hernan: I tried the trick specified here:
    // https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
    // but didn't really worked for me. On one hand, things like computing
    // reflection or refraction required me to reverse the resulting vector
    // which is very error prone. On the other, the view is zoomed with respect
    // to other platforms like OpenGL (this might be a bug, though).
    // Also, don't forget to reverse face culling (see createRasterizer below)

    return VkViewport {
        .x = x,
        .y = h + y,
        .width = w,
        .height = -h,
        .minDepth = minD,
        .maxDepth = maxD,
    };
}

VkRect2D RenderDevice::getScissor( const ViewportDimensions &scissor ) const noexcept
{
    CRIMILD_LOG_TRACE();

    auto x = scissor.dimensions.origin.x;
    auto y = scissor.dimensions.origin.y;
    auto w = scissor.dimensions.size.width;
    auto h = scissor.dimensions.size.height;

    if ( scissor.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
        const auto extent = getSwapchainExtent();
        x *= extent.width;
        y *= extent.height;
        w *= extent.width;
        h *= extent.height;
    }

    return VkRect2D {
        .offset = {
            static_cast< crimild::Int32 >( x ),
            static_cast< crimild::Int32 >( y ),
        },
        .extent = VkExtent2D {
            static_cast< crimild::UInt32 >( w ),
            static_cast< crimild::UInt32 >( h ),
        },
    };
}
