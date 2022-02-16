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

#include "Rendering/VulkanSwapchain.hpp"

#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanSurface.hpp"
#include "Simulation/Simulation.hpp"
#include "VulkanImage.hpp"
#include "VulkanSemaphore.hpp"

using namespace crimild;
using namespace crimild::vulkan;

vulkan::Swapchain::Swapchain( const RenderDevice *renderDevice, const Extent2D &desiredExtent ) noexcept
    : m_renderDevice( renderDevice )
{
    CRIMILD_LOG_TRACE( "Creating Vulkan swapchain" );

    auto physicalDevice = renderDevice->getPhysicalDevice();
    auto surface = renderDevice->getSurface();

    auto swapchainSupport = utils::querySwapchainSupportDetails( physicalDevice->getHandle(), surface->getHandle() );
    auto surfaceFormat = utils::chooseSurfaceFormat( swapchainSupport.formats );
    auto presentMode = utils::choosePresentationMode( swapchainSupport.presentModes );
    auto extent = utils::chooseExtent( swapchainSupport.capabilities, utils::getExtent( desiredExtent ) );

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
        .surface = surface->getHandle(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1, // This may change for stereoscopic rendering (VR)
    };

    // Images created by the swapchain are usually used as color attachments
    // It might be possible to used the for other purposes, like sampling or
    // to copy to or from them to other surfaces.
    // VK_IMAGE_USAGE_TRANSFER_SRC_BIT is requried for taking screenshots.
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    auto indices = utils::findQueueFamilies( physicalDevice->getHandle(), surface->getHandle() );
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

    VkSwapchainKHR swapchainHandler;
    CRIMILD_VULKAN_CHECK(
        vkCreateSwapchainKHR(
            renderDevice->getHandle(),
            &createInfo,
            nullptr,
            &m_handle ) );

    setExtent( utils::getExtent( extent ) );
    setFormat( utils::getFormat( surfaceFormat.format ) );

    CRIMILD_LOG_TRACE( "Retrieving swapchain images" );

    getImages().clear();

    vkGetSwapchainImagesKHR(
        renderDevice->getHandle(),
        m_handle,
        &imageCount,
        nullptr );

    std::vector< VkImage > imagesHandlers( imageCount );
    vkGetSwapchainImagesKHR(
        renderDevice->getHandle(),
        m_handle,
        &imageCount,
        imagesHandlers.data() );

    for ( const auto &imageHandler : imagesHandlers ) {
        auto image = crimild::alloc< Image >();
        image->format = getFormat();
        image->extent.scalingMode = ScalingMode::SWAPCHAIN_RELATIVE;
        // TODO!!!
        // if ( auto img = crimild::get_ptr( image ) ) {
        //     renderDevice->setBindInfo( img, { .imageHandler = imageHandler } );
        // }
        getImages().add( image );
    }

    CRIMILD_LOG_TRACE( "Creating image views" );

    getImageViews().clear();
    getImages().each(
        [ & ]( auto &image ) {
            auto imageView = crimild::alloc< ImageView >();
            imageView->type = ImageView::Type::IMAGE_VIEW_SWAPCHAIN;
            imageView->image = image;
            imageView->format = image->format;
            // TODO!!!
            // renderDevice->bind( crimild::get_ptr( imageView ) );
            getImageViews().add( imageView );
        } );

    CRIMILD_LOG_INFO( "Created Vulkan Swapchain with extents ", extent.width, "x", extent.height );
}

vulkan::Swapchain::~Swapchain( void ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying swapchain" );

    getImageViews().clear();
    getImages().clear();

    if ( m_handle != VK_NULL_HANDLE ) {
        vkDestroySwapchainKHR( m_renderDevice->getHandle(), m_handle, nullptr );
    }

    m_handle = VK_NULL_HANDLE;
    m_renderDevice = nullptr;
}

//////////////////////
// DELETE FROM HERE //
//////////////////////

vulkan::SwapchainOLD::~SwapchainOLD( void ) noexcept
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

vulkan::SwapchainOLD::AcquireImageResult vulkan::SwapchainOLD::acquireNextImage( const Semaphore *imageAvailableSemaphore ) const noexcept
{
    crimild::UInt32 imageIndex;
    auto ret = vkAcquireNextImageKHR(
        renderDevice->handler,
        handler,
        std::numeric_limits< uint64_t >::max(), // disable timeout
        imageAvailableSemaphore->handler,
        VK_NULL_HANDLE,
        &imageIndex );

    return AcquireImageResult {
        .imageIndex = imageIndex,
        .success = ( ret == VK_SUCCESS ),
        .outOfDate = ( ret == VK_ERROR_OUT_OF_DATE_KHR ),
    };
}

vulkan::SwapchainOLD::PresentImageResult vulkan::SwapchainOLD::presentImage( crimild::UInt32 imageIndex, const Semaphore *signal ) const noexcept
{
    VkSemaphore signalSemaphores[] = {
        signal->handler,
    };

    VkSwapchainKHR swapchains[] = {
        handler,
    };

    auto presentInfo = VkPresentInfoKHR {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    auto ret = vkQueuePresentKHR(
        renderDevice->presentQueue,
        &presentInfo );

    return PresentImageResult {
        .success = ( ret == VK_SUCCESS ),
        .outOfDate = ( ret == VK_ERROR_OUT_OF_DATE_KHR || VK_SUBOPTIMAL_KHR ),
    };
}

SharedPointer< vulkan::SwapchainOLD > SwapchainManager::create( SwapchainOLD::Descriptor const &descriptor ) noexcept
{
    return nullptr;
}

void SwapchainManager::destroy( SwapchainOLD *swapchain ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying swapchain" );

    swapchain->getImageViews().clear();
    swapchain->getImages().clear();

    if ( swapchain->handler != VK_NULL_HANDLE ) {
        vkDestroySwapchainKHR( swapchain->renderDevice->handler, swapchain->handler, nullptr );
    }

    swapchain->handler = VK_NULL_HANDLE;
    swapchain->renderDevice = nullptr;
    swapchain->manager = nullptr;
    erase( swapchain );
}
