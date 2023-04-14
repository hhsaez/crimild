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
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/VulkanSwapchain.hpp"

#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanSemaphore.hpp"
#include "Rendering/VulkanSurface.hpp"

using namespace crimild::vulkan;

Swapchain::Swapchain( RenderDevice *device, VulkanSurface *surface, const VkExtent2D &extent ) noexcept
    : WithRenderDevice( device ),
      m_extent( extent )
{
    CRIMILD_LOG_TRACE();

    auto physicalDevice = device->getPhysicalDevice();

    auto swapchainSupport = utils::querySwapchainSupportDetails( physicalDevice->getHandle(), surface->getHandle() );
    auto surfaceFormat = utils::chooseSurfaceFormat( swapchainSupport.formats );
    auto presentMode = utils::choosePresentationMode( swapchainSupport.presentModes );

    m_extent = utils::chooseExtent( swapchainSupport.capabilities, m_extent );
    m_format = surfaceFormat.format;

    // The Vulkan implementation defines a minimum number of images to work with.
    // We request one more image than the minimum. Also, make sure not to exceed the
    // maximum number of images (a value of 0 means there's no maximum)
    auto imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if ( swapchainSupport.capabilities.maxImageCount > 0 ) {
        imageCount = std::min(
            swapchainSupport.capabilities.maxImageCount,
            imageCount
        );
    }

    auto createInfo = VkSwapchainCreateInfoKHR {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface->getHandle(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = m_extent,
        .imageArrayLayers = 1, // This may change for stereoscopic rendering (VR)
    };

    // Images created by the swapchain are usually used as color attachments
    // It might be possible to used the for other purposes, like sampling or
    // to copy to or from them to other surfaces.
    // VK_IMAGE_USAGE_TRANSFER_SRC_BIT is requried for taking screenshots.
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    uint32_t queueFamilyIndices[] = {
        device->getGraphicsQueueFamily(),
        device->getPresentQueueFamily(),
    };

    if ( device->getGraphicsQueueFamily() != device->getPresentQueueFamily() ) {
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

    VkSwapchainKHR handle;
    CRIMILD_VULKAN_CHECK(
        vkCreateSwapchainKHR(
            getRenderDevice()->getHandle(),
            &createInfo,
            nullptr,
            &handle
        )
    );
    setHandle( handle );

    CRIMILD_LOG_DEBUG( "Getting swapchain images" );

    vkGetSwapchainImagesKHR(
        getRenderDevice()->getHandle(),
        getHandle(),
        &imageCount,
        nullptr
    );

    std::vector< VkImage > images( imageCount );
    vkGetSwapchainImagesKHR(
        getRenderDevice()->getHandle(),
        getHandle(),
        &imageCount,
        images.data()
    );

    m_images.clear();
    for ( auto &image : images ) {
        m_images.push_back(
            crimild::alloc< vulkan::Image >(
                getRenderDevice(),
                image,
                m_extent,
                m_format,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                "Swapchain/Image"
            )
        );
    }

    CRIMILD_LOG_DEBUG( "Creating swapchain image views" );

    m_imageViews.clear();
    for ( auto &image : m_images ) {
        m_imageViews.push_back(
            [ & ] {
                auto createInfo = vulkan::ImageView::createInfo();
                createInfo.image = image->getHandle();
                createInfo.format = m_format; // VK_FORMAT_B8G8R8A8_UNORM
                auto imageView = crimild::alloc< vulkan::ImageView >(
                    getRenderDevice(),
                    "Swapchain/ImageView",
                    image,
                    createInfo
                );
                return imageView;
            }()
        );
    }

    CRIMILD_LOG_INFO( "Created Vulkan Swapchain with extents ", m_extent.width, "x", m_extent.height );
}

Swapchain::~Swapchain( void ) noexcept
{
    CRIMILD_LOG_DEBUG( "Destroying Vulkan swapchain image views" );
    m_imageViews.clear();

    CRIMILD_LOG_DEBUG( "Destroying Vulkan swapchain image" );
    m_images.clear();

    CRIMILD_LOG_DEBUG( "Destroying Vulkan swapchain" );

    vkDestroySwapchainKHR( getRenderDevice()->getHandle(), getHandle(), getRenderDevice()->getAllocator() );
    setHandle( VK_NULL_HANDLE );
}

bool Swapchain::acquireNextImage( uint32_t &imageIndex, const SyncOptions &options ) noexcept
{
    // TODO: wait for fences

    // Will signal this semaphore when image is available
    VkSemaphore imageAvailableSemaphore =
        !options.signal.empty()
            ? options.signal.front()->getHandle()
            : VK_NULL_HANDLE;

    const auto result = vkAcquireNextImageKHR(
        getRenderDevice()->getHandle(),
        getHandle(),
        std::numeric_limits< uint64_t >::max(),
        imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );
    if ( result != VK_SUCCESS ) {
        if ( result == VK_ERROR_OUT_OF_DATE_KHR ) {
            // swapchain needs recreation
        }
        return false;
    }

    // TODO: wait for images in flight

    return true;
}

bool Swapchain::present( uint32_t imageIndex, const SyncOptions &options ) noexcept
{
    VkSwapchainKHR swapchains[] = { getHandle() };

    std::vector< VkSemaphore > waitSemaphores;
    for ( auto &semaphore : options.wait ) {
        waitSemaphores.push_back( semaphore->getHandle() );
    }

    auto presentInfo = VkPresentInfoKHR {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = ( uint32_t ) waitSemaphores.size(),
        .pWaitSemaphores = waitSemaphores.data(),
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    auto ret = vkQueuePresentKHR( getRenderDevice()->getPresentQueue(), &presentInfo );
    if ( ret == VK_ERROR_OUT_OF_DATE_KHR || ret == VK_SUBOPTIMAL_KHR ) {
        // swapchain needs to be recreated
        return false;
    }
    return true;
}
