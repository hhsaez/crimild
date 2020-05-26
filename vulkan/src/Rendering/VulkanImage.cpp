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

    ImageBindInfo bindInfo;

	auto channels = crimild::UInt32( 4 ); //< TODO: Fetch channels from image format
	auto mipLevels = image->getMipLevels();
	auto layerCount = crimild::UInt32( 1 );

	// TODO: use frame graph to set usage?
	VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	if ( image->data.size() > 0 ) {
		// If image has data, it will be used for transfer operations
		usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
    else {
        // If image has no data, then it's used as an attachment
        if ( utils::formatIsColor( image->format ) ) {
        	usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        else if ( utils::formatIsDepthStencil( image->format ) ) {
        	usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
    }

    utils::createImage(
        renderDevice,
        utils::ImageDescriptor {
            .width = crimild::UInt32( width ),
            .height = crimild::UInt32( height ),
            .format = utils::getFormat( renderDevice, image->format ),
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            .mipLevels = mipLevels,
//            .numSamples = descriptor.numSamples,
        },
       	bindInfo.imageHandler,
       	bindInfo.imageMemoryHandler
    );

	if ( image->data.size() > 0 ) {
		// Image has pixel data. Upload it

		// TODO: Cubemap

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VkDeviceSize layerSize = width * height * channels;
		VkDeviceSize imageSize = layerSize * layerCount;

		auto success = utils::createBuffer(
			renderDevice,
			utils::BufferDescriptor {
				.size = imageSize,
				.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			},
			stagingBuffer,
			stagingBufferMemory
		);
		if ( !success ) {
			CRIMILD_LOG_ERROR( "Failed to create image staging buffer" );
			return false;
		}

		std::vector< const void * > layers;
		// TODO: cubemap
		layers.push_back( image->data.getData() );

		utils::copyToBuffer(
			renderDevice->handler,
			stagingBufferMemory,
			layers.data(),
			layers.size(),
			layerSize
		);
		
		utils::transitionImageLayout(
			renderDevice,
			bindInfo.imageHandler,
			utils::getFormat( renderDevice, image->format ),
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			mipLevels,
			layerCount
		);

		// TODO: handle cubemaps

		utils::copyBufferToImage(
			renderDevice,
			stagingBuffer,
			bindInfo.imageHandler,
			width,
			height
		);

		// Automatically transitions to SHADER_READ_OPTIMAL layout
		utils::generateMipmaps(
			renderDevice,
			bindInfo.imageHandler,
			VK_FORMAT_R8G8B8A8_UNORM,
			width,
			height,
			mipLevels
		);

		vkDestroyBuffer(
			renderDevice->handler,
			stagingBuffer,
			nullptr
		);

		vkFreeMemory(
			renderDevice->handler,
			stagingBufferMemory,
			nullptr
		);
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
    }

    removeBindInfo( image );

    return ManagerImpl::unbind( image );
}

