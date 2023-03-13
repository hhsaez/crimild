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

#include "Rendering/VulkanRenderDeviceCache.hpp"

#include "Rendering/Image.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "Rendering/VulkanBuffer.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanSampler.hpp"

using namespace crimild;
using namespace crimild::vulkan;

RenderDeviceCache::RenderDeviceCache( RenderDevice *device ) noexcept
    : WithRenderDevice( device )
{
    // no-op
}

RenderDeviceCache::~RenderDeviceCache( void ) noexcept
{
    // no-op
}

void RenderDeviceCache::onBeforeFrame( void ) noexcept
{
}

void RenderDeviceCache::onAfterFrame( void ) noexcept
{
}

std::shared_ptr< vulkan::Buffer > &RenderDeviceCache::bind( IndexBuffer *indexBuffer ) noexcept
{
    if ( !m_boundObjects.contains( indexBuffer ) ) {
        m_buffers[ indexBuffer ] = crimild::alloc< vulkan::Buffer >(
            getRenderDevice(),
            indexBuffer->getClassName(),
            indexBuffer->getBufferView()
        );
        m_boundObjects.insert( indexBuffer );
    }
    return m_buffers.at( indexBuffer );
}

std::shared_ptr< vulkan::Buffer > &RenderDeviceCache::bind( VertexBuffer *vertexBuffer ) noexcept
{
    if ( !m_boundObjects.contains( vertexBuffer ) ) {
        m_buffers[ vertexBuffer ] = crimild::alloc< vulkan::Buffer >(
            getRenderDevice(),
            vertexBuffer->getClassName(),
            vertexBuffer->getBufferView()
        );
        m_boundObjects.insert( vertexBuffer );
    }
    return m_buffers.at( vertexBuffer );
}

std::shared_ptr< vulkan::Buffer > &RenderDeviceCache::bind( UniformBuffer *uniformBuffer ) noexcept
{
    if ( !m_boundObjects.contains( uniformBuffer ) ) {
        m_buffers[ uniformBuffer ] = crimild::alloc< vulkan::Buffer >(
            getRenderDevice(),
            uniformBuffer->getClassName(),
            uniformBuffer->getBufferView()
        );
        m_boundObjects.insert( uniformBuffer );
    }
    return m_buffers.at( uniformBuffer );
}

std::shared_ptr< vulkan::Image > &RenderDeviceCache::bind( crimild::Image *source ) noexcept
{
    if ( !m_boundObjects.contains( source ) ) {
        m_images[ source ] = crimild::alloc< vulkan::Image >( getRenderDevice(), source );
        m_boundObjects.insert( source );
    }
    return m_images.at( source );
}

std::shared_ptr< vulkan::ImageView > &RenderDeviceCache::bind( crimild::ImageView *source ) noexcept
{
    if ( !m_boundObjects.contains( source ) ) {
        auto mipLevels = source->mipLevels;
        if ( mipLevels == 0 ) {
            mipLevels = Numerici::max( 1, source->image->getMipLevels() );
        }

        auto image = bind( source->image.get() );

        auto layerCount = source->layerCount;
        if ( layerCount == 0 ) {
            layerCount = source->image->getLayerCount();
        }

        auto info = VkImageViewCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .flags = 0,
            .image = image->getHandle(),
            .viewType = utils::getImageViewType( source ),
            .format = [ & ] {
                auto format = source->format;
                if ( format == Format::UNDEFINED ) {
                    format = source->image->format;
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
                .aspectMask = utils::getImageViewAspectFlags( source ),
                .baseMipLevel = 0,
                .levelCount = mipLevels,
                .baseArrayLayer = 0,
                .layerCount = layerCount,
            },
        };

        m_imageViews[ source ] = crimild::alloc< vulkan::ImageView >( getRenderDevice(), source->getName(), image, info );
        m_boundObjects.insert( source );
    }

    return m_imageViews.at( source );
}

std::shared_ptr< vulkan::Sampler > &RenderDeviceCache::bind( crimild::Sampler *source ) noexcept
{
    if ( !m_boundObjects.contains( source ) ) {
        auto addressMode = utils::getSamplerAddressMode( source->getWrapMode() );
        auto compareOp = utils::getCompareOp( source->getCompareOp() );
        auto borderColor = utils::getBorderColor( source->getBorderColor() );
        auto minLod = crimild::Real32( source->getMinLod() );
        auto maxLod = crimild::Real32( source->getMaxLod() );

        auto info = VkSamplerCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = utils::getSamplerFilter( source->getMagFilter() ),
            .minFilter = utils::getSamplerFilter( source->getMinFilter() ),
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
        m_samplers[ source ] = crimild::alloc< vulkan::Sampler >( getRenderDevice(), source->getClassName(), info );
        m_boundObjects.insert( source );
    }
    return m_samplers.at( source );
}
