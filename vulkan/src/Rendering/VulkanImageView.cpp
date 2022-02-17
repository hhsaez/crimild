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
#include "Rendering/VulkanRenderDeviceOLD.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool vulkan::ImageViewManager::bind( ImageView *imageView ) noexcept
{
    if ( validate( imageView ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan Image View: ", imageView->getName() );

    auto renderDevice = getRenderDevice();

    auto mipLevels = imageView->mipLevels;
    if ( mipLevels == 0 ) {
        mipLevels = Numerici::max( 1, imageView->image->getMipLevels() );
    }

    auto image = renderDevice->getBindInfo( crimild::get_ptr( imageView->image ) ).imageHandler;

    auto layerCount = imageView->layerCount;
    if ( layerCount == 0 ) {
        layerCount = imageView->image->getLayerCount();
    }

    auto viewInfo = VkImageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .flags = 0,
        .image = image,
        .viewType = utils::getImageViewType( imageView ),
        .format = utils::getImageViewFormat( renderDevice, imageView ),
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

    VkImageView handler;
    CRIMILD_VULKAN_CHECK(
        vkCreateImageView(
            renderDevice->handler,
            &viewInfo,
            nullptr,
            &handler ) );

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
