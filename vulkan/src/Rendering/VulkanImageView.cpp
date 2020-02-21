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

#include "VulkanImageView.hpp"
#include "VulkanImage.hpp"
#include "VulkanRenderDevice.hpp"
#include "Exceptions/VulkanException.hpp"
#include "Foundation/Log.hpp"

using namespace crimild;
using namespace crimild::vulkan;

ImageView::~ImageView( void ) noexcept
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

SharedPointer< ImageView > ImageViewManager::create( ImageView::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating image view" );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    auto viewInfo = VkImageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = descriptor.image->handler,

        // We're dealing with 2D images
        .viewType = descriptor.imageType,

        // Match the specified format
        .format = descriptor.format,

        // We don't need to swizzle (swap around) any of the color components
        .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,

        // Determine what is affected by the image operations (color, depth, stencil, etc)
        .subresourceRange.aspectMask = descriptor.aspectFlags,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = descriptor.mipLevels,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = descriptor.layerCount,

        // optional
        .flags = 0
    };

    VkImageView imageViewHandler;
    CRIMILD_VULKAN_CHECK(
     	vkCreateImageView(
      		renderDevice->handler,
          	&viewInfo,
          	nullptr,
          	&imageViewHandler
		)
 	);

    auto imageView = crimild::alloc< ImageView >();
    imageView->handler = imageViewHandler;
    imageView->renderDevice = renderDevice;
    imageView->image = descriptor.image;
    imageView->manager = this;
    insert( crimild::get_ptr( imageView ) );

    return imageView;
}

void ImageViewManager::destroy( ImageView *imageView ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying image view" );

    if ( imageView->renderDevice != nullptr && imageView->handler != VK_NULL_HANDLE ) {
        vkDestroyImageView(
       		imageView->renderDevice->handler,
           	imageView->handler,
       		nullptr
       	);
    }

    imageView->handler = nullptr;
    imageView->image = nullptr;
    imageView->manager = nullptr;
    imageView->renderDevice = nullptr;
    erase( imageView );
}
