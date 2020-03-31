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

#include "Foundation/Log.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool vulkan::ImageViewManager::bind( ImageView *imageView ) noexcept
{
    if ( validate( imageView ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan Image View" );

    auto renderDevice = getRenderDevice();

    auto getImageViewType = []( ImageView::Type type ) {
        switch ( type ) {
            case ImageView::Type::IMAGE_VIEW_1D:
                return VK_IMAGE_VIEW_TYPE_1D;
            case ImageView::Type::IMAGE_VIEW_2D:
                return VK_IMAGE_VIEW_TYPE_2D;
            case ImageView::Type::IMAGE_VIEW_3D:
                return VK_IMAGE_VIEW_TYPE_3D;
            case ImageView::Type::IMAGE_VIEW_CUBE:
                return VK_IMAGE_VIEW_TYPE_CUBE;
            default:
                return VK_IMAGE_VIEW_TYPE_2D;
        }
    };

    auto getAspectFlags = []( Image::Usage usage ) {
        if ( usage & Image::Usage::DEPTH_STENCIL_ATTACHMENT ) {
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        return VK_IMAGE_ASPECT_COLOR_BIT;
    };

    auto viewInfo = VkImageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = renderDevice->getBindInfo( crimild::get_ptr( imageView->image ) ).imageHandler,

        // We're dealing with 2D images
        .viewType = getImageViewType( imageView->type ),

        // Match the specified format
        .format = utils::getFormat( renderDevice, imageView->format ),

        // We don't need to swizzle (swap around) any of the color components
        .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,

        // Determine what is affected by the image operations (color, depth, stencil, etc)
        .subresourceRange.aspectMask = getAspectFlags( imageView->image->usage ),
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = imageView->mipLevels,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = imageView->layerCount,

        // optional
        .flags = 0
    };

    VkImageView handler;
    CRIMILD_VULKAN_CHECK(
         vkCreateImageView(
              renderDevice->handler,
              &viewInfo,
              nullptr,
              &handler
        )
    );

    setBindInfo( imageView, handler );

    return ManagerImpl::bind( imageView );
}

crimild::Bool vulkan::ImageViewManager::unbind( ImageView *imageView ) noexcept
{
    if ( !validate( imageView ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbind Vulkan Image View" );

    auto renderDevice = getRenderDevice();
    auto handler = renderDevice->getBindInfo( imageView );

    if ( renderDevice != nullptr && handler != VK_NULL_HANDLE ) {
        vkDestroyImageView( renderDevice->handler, handler, nullptr );
    }

    removeBindInfo( imageView );

    return ManagerImpl::unbind( imageView );
}

