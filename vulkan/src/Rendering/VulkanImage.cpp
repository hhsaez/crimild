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

#include "Foundation/Log.hpp"
#include "VulkanRenderDeviceOLD.hpp"

using namespace crimild;
using namespace crimild::vulkan;

/*
ImageManager::ImageManager( void ) noexcept
{
    registerMessageHandler< messaging::ImageCreated >(
        [ & ]( messaging::ImageCreated const &msg ) {
            // TODO: bind on demand?
            // TODO: add to images to update set
        } );

    registerMessageHandler< messaging::ImageCreated >(
        [ & ]( messaging::ImageCreated const &msg ) {
            // TODO: add to images to update set (if bound?)
        } );

    registerMessageHandler< messaging::ImageCreated >(
        [ & ]( messaging::ImageCreated const &msg ) {
            // TODO: remove from images to update if needed
            // TODO: unbind
        } );
}
*/

crimild::Bool vulkan::ImageManager::bind( Image *image ) noexcept
{
    if ( validate( image ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan Image: ", image->getName() );

    auto renderDevice = getRenderDevice();

    auto width = image->extent.width;
    auto height = image->extent.height;
    if ( image->extent.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
        auto swapchain = renderDevice->getSwapchain();
        width *= swapchain->extent.width;
        height *= swapchain->extent.height;
    }

    ImageBindInfo bindInfo;

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

    utils::createImage(
        renderDevice,
        utils::ImageDescriptor {
            .width = crimild::UInt32( width ),
            .height = crimild::UInt32( height ),
            .format = utils::getFormat( renderDevice, image->format ),
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            .mipLevels = mipLevels,
            //            .numSamples = descriptor.numSamples,
            .arrayLayers = arrayLayers,
            .flags = ( type == Image::Type::IMAGE_2D_CUBEMAP ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0u ),
        },
        bindInfo.imageHandler,
        bindInfo.imageMemoryHandler );

    if ( image->getBufferView() != nullptr ) {
        // Image has pixel data. Upload it

        VkDeviceSize imageSize = image->getBufferView()->getLength();

        auto success = utils::createBuffer(
            renderDevice,
            utils::BufferDescriptor {
                .size = imageSize,
                .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT },
            bindInfo.stagingBuffer,
            bindInfo.stagingBufferMemory );
        if ( !success ) {
            CRIMILD_LOG_ERROR( "Failed to create image staging buffer" );
            return false;
        }

        utils::copyToBuffer(
            renderDevice->handler,
            bindInfo.stagingBufferMemory,
            image->getBufferView()->getData(),
            imageSize );

        utils::transitionImageLayout(
            renderDevice,
            bindInfo.imageHandler,
            utils::getFormat( renderDevice, image->format ),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            mipLevels,
            arrayLayers );

        utils::copyBufferToImage(
            renderDevice,
            bindInfo.stagingBuffer,
            bindInfo.imageHandler,
            width,
            height,
            arrayLayers );

        if ( type == Image::Type::IMAGE_2D_CUBEMAP ) {
            // No mipmaps. Transition to SHADER_READ_OPTIMAL
            utils::transitionImageLayout(
                renderDevice,
                bindInfo.imageHandler,
                utils::getFormat( renderDevice, image->format ),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                mipLevels,
                arrayLayers );
        } else {
            // Automatically transitions to SHADER_READ_OPTIMAL layout
            utils::generateMipmaps(
                renderDevice,
                bindInfo.imageHandler,
                utils::getFormat( renderDevice, image->format ),
                width,
                height,
                mipLevels );
        };

        if ( image->getBufferView() == nullptr || image->getBufferView()->getUsage() == BufferView::Usage::STATIC ) {
            // We won't be using the staging buffers for static images anymore
            vkDestroyBuffer(
                renderDevice->handler,
                bindInfo.stagingBuffer,
                nullptr );

            vkFreeMemory(
                renderDevice->handler,
                bindInfo.stagingBufferMemory,
                nullptr );

            bindInfo.stagingBuffer = VK_NULL_HANDLE;
            bindInfo.stagingBufferMemory = VK_NULL_HANDLE;
        }
    }

    if ( !image->getName().empty() ) {
        utils::setObjectName(
            renderDevice->handler,
            UInt64( bindInfo.imageHandler ),
            VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT,
            image->getName().c_str() );
    }

    setBindInfo( image, bindInfo );

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

        if ( handler.stagingBuffer != VK_NULL_HANDLE ) {
            vkDestroyBuffer(
                renderDevice->handler,
                handler.stagingBuffer,
                nullptr );
        }
        if ( handler.imageMemoryHandler != VK_NULL_HANDLE ) {
            vkFreeMemory( renderDevice->handler, handler.stagingBufferMemory, nullptr );
        }

        handler.imageHandler = VK_NULL_HANDLE;
        handler.imageMemoryHandler = VK_NULL_HANDLE;
        handler.stagingBuffer = VK_NULL_HANDLE;
        handler.stagingBufferMemory = VK_NULL_HANDLE;
    }

    removeBindInfo( image );

    return ManagerImpl::unbind( image );
}

void vulkan::ImageManager::updateImages( void ) noexcept
{
    // TODO: This is a slow operation. Not only it needs to iterate over all
    // available images to get the ones that actually need updating, but
    // also it's copying data to the device in a synchronous way.
    auto renderDevice = getRenderDevice();
    each( [ this, renderDevice ]( Image *image, ImageBindInfo &bindInfo ) {
        if ( auto bufferView = image->getBufferView() ) {
            if ( bufferView->getUsage() == BufferView::Usage::DYNAMIC ) {
                auto width = image->extent.width;
                auto height = image->extent.height;
                if ( image->extent.scalingMode == ScalingMode::SWAPCHAIN_RELATIVE ) {
                    auto swapchain = renderDevice->getSwapchain();
                    width *= swapchain->extent.width;
                    height *= swapchain->extent.height;
                }
                auto mipLevels = image->getMipLevels();
                auto arrayLayers = image->getLayerCount();
                auto type = image->type;

                VkDeviceSize imageSize = image->getBufferView()->getLength();

                utils::copyToBuffer(
                    renderDevice->handler,
                    bindInfo.stagingBufferMemory,
                    image->getBufferView()->getData(),
                    imageSize );

                utils::transitionImageLayout(
                    renderDevice,
                    bindInfo.imageHandler,
                    utils::getFormat( renderDevice, image->format ),
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    mipLevels,
                    arrayLayers );

                utils::copyBufferToImage(
                    renderDevice,
                    bindInfo.stagingBuffer,
                    bindInfo.imageHandler,
                    width,
                    height,
                    arrayLayers );

                // TODO: Dynamic images should not support mipmapping?
                if ( type == Image::Type::IMAGE_2D_CUBEMAP ) {
                    // No mipmaps. Transition to SHADER_READ_OPTIMAL
                    utils::transitionImageLayout(
                        renderDevice,
                        bindInfo.imageHandler,
                        utils::getFormat( renderDevice, image->format ),
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        mipLevels,
                        arrayLayers );
                } else {
                    // Automatically transitions to SHADER_READ_OPTIMAL layout
                    utils::generateMipmaps(
                        renderDevice,
                        bindInfo.imageHandler,
                        utils::getFormat( renderDevice, image->format ),
                        width,
                        height,
                        mipLevels );
                };
            }
        }
    } );
}
