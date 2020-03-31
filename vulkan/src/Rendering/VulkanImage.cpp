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

#include "VulkanImage.hpp"
#include "VulkanRenderDevice.hpp"
#include "Foundation/Log.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool vulkan::ImageManager::bind( Image *image ) noexcept
{
    if ( validate( image ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan Image" );

    auto renderDevice = getRenderDevice();

    auto width = image->extent.width;
    auto height = image->extent.height;
    if ( image->extent.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
        auto swapchain = renderDevice->getSwapchain();
        width *= swapchain->extent.width;
        height *= swapchain->extent.height;
    }

    ImageBindInfo handler;

    utils::createImage(
        renderDevice,
        utils::ImageDescriptor {
            .width = crimild::UInt32( width ),
            .height = crimild::UInt32( height ),
            .format = utils::getFormat( renderDevice, image->format ),
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = utils::getImageUsage( image->usage ),
            .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//            .mipLevels = 1,
//            .numSamples = descriptor.numSamples,
        },
       	handler.imageHandler,
       	handler.imageMemoryHandler
    );

    setBindInfo( image, handler );

    return ManagerImpl::bind( image );
}

crimild::Bool vulkan::ImageManager::unbind( Image *image ) noexcept
{
    if ( !validate( image ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbind Vulkan Image" );

    auto renderDevice = getRenderDevice();
    auto handler = renderDevice->getBindInfo( image );

    if ( renderDevice != nullptr ) {
        if ( handler.imageHandler != VK_NULL_HANDLE ) {
            vkDestroyImage( renderDevice->handler, handler.imageHandler, nullptr );
        }
        if ( handler.imageMemoryHandler != VK_NULL_HANDLE ) {
            vkFreeMemory( renderDevice->handler, handler.imageMemoryHandler, nullptr );
        }
    }

    removeBindInfo( image );

    return ManagerImpl::unbind( image );
}

