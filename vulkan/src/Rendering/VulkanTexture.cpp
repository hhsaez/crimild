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

#include "Rendering/VulkanTexture.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild::vulkan;

crimild::Bool TextureManager::bind( Texture *texture ) noexcept
{
    return false;
    /*
    if ( validate( texture ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan Texture" );

    auto renderDevice = getRenderDevice();

    auto image = texture->getImage();
    if ( image == nullptr ) {
        CRIMILD_LOG_ERROR( "Texture does not contain a valid image" );
        return false;
    }

    crimild::UInt32 imageWidth = image->getWidth();
    crimild::UInt32 imageHeight = image->getHeight();
    auto isCubemap = texture->getTarget() == Texture::Target::CUBE_MAP;

    // Assume no layers excepts in the case where we're dealing with cubemaps
    crimild::Size layerCount = isCubemap ? 6 : 1;

    auto mipLevels = crimild::UInt32( 1 );
    // Enable mipmapping only for regular textures
    if ( !isCubemap && texture->isMipmappingEnabled() ) {
        // Each mipmap level is half the size of the previous one
        // At the very least, we'll have 1 mip level (the original size)
    	mipLevels += static_cast< crimild::UInt32 >( Numericf::floor( Numericf::log2( Numericf::max( imageWidth, imageHeight ) ) ) );
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkDeviceSize layerSize = image->getWidth() * image->getHeight() * image->getBpp();
    VkDeviceSize imageSize = layerSize * layerCount;

    auto success = utils::createBuffer(
        renderDevice,
        utils::BufferDescriptor {
        	.size = imageSize,
        	.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        	.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    	},
        stagingBuffer,
        stagingBufferMemory
    );
    if ( !success ) {
        CRIMILD_LOG_ERROR( "Failed to create texture staging buffer" );
        return false;
    }

    std::vector< const void * > layers;
    if ( isCubemap ) {
        layers.push_back( texture->getFace( Texture::CubeMapFace::RIGHT )->getData() );
        layers.push_back( texture->getFace( Texture::CubeMapFace::LEFT )->getData() );
        layers.push_back( texture->getFace( Texture::CubeMapFace::BOTTOM )->getData() );
        layers.push_back( texture->getFace( Texture::CubeMapFace::TOP )->getData() );
        layers.push_back( texture->getFace( Texture::CubeMapFace::BACK )->getData() );
        layers.push_back( texture->getFace( Texture::CubeMapFace::FRONT )->getData() );
    } else {
        layers.push_back( image->getData() );
    }

    utils::copyToBuffer(
        renderDevice->handler,
    	stagingBufferMemory,
        layers.data(),
        layers.size(),
        layerSize
    );

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkFormat textureFormat = []( crimild::Int32 channels ) {
        auto format = VK_FORMAT_R8G8B8A8_UNORM;
        if ( channels == 3 ) {
            format = VK_FORMAT_R8G8B8_UNORM;
        }
        return format;
    }( image->getBpp() );

    success = utils::createImage(
    	renderDevice,
        utils::ImageDescriptor {
        	.width = imageWidth,
        	.height = imageHeight,
        	.format = textureFormat,
        	.tiling = VK_IMAGE_TILING_OPTIMAL,
        	.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        	.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        	.mipLevels = mipLevels,
        	.arrayLayers = ( isCubemap ? 6u : 1u ),
        	.flags = ( isCubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0u ),
        },
        textureImage,
        textureImageMemory
    );
    if ( !success ) {
        CRIMILD_LOG_ERROR( "Failed to create texture's image" );
        return false;
    }

    utils::transitionImageLayout(
        renderDevice,
        textureImage,
        textureFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        mipLevels,
     	layerCount
    );

    if ( isCubemap ) {
        utils::copyBufferToLayeredImage(
            renderDevice,
            stagingBuffer,
            textureImage,
            layerCount,
            layerSize,
            imageWidth,
            imageHeight
        );

        utils::transitionImageLayout(
            renderDevice,
            textureImage,
            textureFormat,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            mipLevels,
            layerCount
        );
    }
    else {
        utils::copyBufferToImage(
            renderDevice,
            stagingBuffer,
            textureImage,
            imageWidth,
            imageHeight
        );

        utils::generateMipmaps( renderDevice, textureImage, VK_FORMAT_R8G8B8A8_UNORM, imageWidth, imageHeight, mipLevels );
    }

    vkDestroyBuffer( renderDevice->handler, stagingBuffer, nullptr );
    vkFreeMemory( renderDevice->handler, stagingBufferMemory, nullptr );

    auto imageView = renderDevice->create(
        ImageView::Descriptor {
            .imageType = ( isCubemap ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D ),
            .image = [ textureImage ] {
                auto image = crimild::alloc< vulkan::Image >();
                image->handler = textureImage;
                return image;
            }(),
            .format = textureFormat,
            .aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevels = mipLevels,
            .layerCount = ( isCubemap ? 6u : 1u ),
    	}
    );

    auto addressMode = utils::getSamplerAddressMode( texture->getWrapMode() );
    auto compareOp = VK_COMPARE_OP_ALWAYS;
    auto borderColor = utils::getBorderColor( texture->getBorderColor() );

    // overrides for cubemap
    if ( isCubemap ) {
        addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        compareOp = VK_COMPARE_OP_NEVER;
    }

    auto samplerInfo = VkSamplerCreateInfo {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = utils::getVulkanFilter( texture->getMagFilter() ),
        .minFilter = utils::getVulkanFilter( texture->getMinFilter() ),
        .addressModeU = addressMode,
        .addressModeV = addressMode,
        .addressModeW = addressMode,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = 16,
        .borderColor = borderColor,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_FALSE,
        .compareOp = compareOp,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .mipLodBias = 0,
        .minLod = 0,
        .maxLod = static_cast< float >( mipLevels ),
    };

    VkSampler textureSampler;
    CRIMILD_VULKAN_CHECK(
        vkCreateSampler(
            renderDevice->handler,
            &samplerInfo,
            nullptr,
            &textureSampler
        )
    );

    setBindInfo(
    	texture,
       	{
        	.textureImage = textureImage,
        	.textureImageMemory = textureImageMemory,
        	.imageView = imageView,
            .sampler = textureSampler,
    	}
    );

    return ManagerImpl::bind( texture );
     */
}

crimild::Bool TextureManager::unbind( Texture *texture ) noexcept
{
    return false;
    /*
    if ( !validate( texture ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbind Vulkan Texture" );

    auto renderDevice = getRenderDevice();

    auto bindInfo = getBindInfo( texture );

    vkDestroySampler( renderDevice->handler, bindInfo.sampler, nullptr );

    bindInfo.imageView = nullptr;

    vkDestroyImage( renderDevice->handler, bindInfo.textureImage, nullptr );
    vkFreeMemory( renderDevice->handler, bindInfo.textureImageMemory, nullptr );

    removeBindInfo( texture );

    return ManagerImpl::unbind( texture );
     */
}

