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
#include "Rendering/VulkanShadowMap.hpp"
#include "SceneGraph/Light.hpp"

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

size_t RenderDeviceCache::getNewObjectIndex( void ) noexcept
{
    auto ret = 0;
    while ( ret < m_boundObjects.size() && !m_boundObjects[ ret ].expired() ) {
        ++ret;
    }
    if ( ret >= m_boundObjects.size() ) {
        m_boundObjects.resize( m_boundObjects.size() + 1 );
    }
    return ret;
}

void RenderDeviceCache::onBeforeFrame( void ) noexcept
{
    // no-op?
}

void RenderDeviceCache::onAfterFrame( void ) noexcept
{
    // This seems slow...
    for ( size_t i = 0; i < m_boundObjects.size(); ++i ) {
        if ( m_boundObjects[ i ].expired() ) {
            if ( m_reverseIndex.contains( i ) ) {
                m_index.erase( m_reverseIndex.at( i ) );
            }
            m_reverseIndex.erase( i );
            m_buffers.erase( i );
            m_images.erase( i );
            m_imageViews.erase( i );
            m_samplers.erase( i );
            m_shadowMaps.erase( i );
            m_uniforms.erase( i );
            m_descriptorSets.erase( i );
        }
    }
}

size_t RenderDeviceCache::addBoundObject( const std::shared_ptr< const SharedObject > &obj ) noexcept
{
    const size_t id = reinterpret_cast< size_t >( obj.get() );
    auto index = getNewObjectIndex();
    m_index[ id ] = index;
    m_reverseIndex[ index ] = id;
    m_boundObjects[ index ] = obj;
    return index;
}

std::shared_ptr< vulkan::Buffer > &RenderDeviceCache::bind( const std::shared_ptr< const IndexBuffer > &indexBuffer ) noexcept
{
    const auto id = indexBuffer->getUniqueID();
    if ( !m_index.contains( id ) ) {
        auto index = addBoundObject( indexBuffer );
        m_buffers[ index ] = crimild::alloc< vulkan::Buffer >(
            getRenderDevice(),
            indexBuffer->getClassName(),
            indexBuffer->getBufferView()
        );
    }
    return m_buffers.at( m_index.at( id ) );
}

std::shared_ptr< vulkan::Buffer > &RenderDeviceCache::bind( const std::shared_ptr< const VertexBuffer > &vertexBuffer ) noexcept
{
    const auto id = vertexBuffer->getUniqueID();
    if ( !m_index.contains( id ) ) {
        auto index = addBoundObject( vertexBuffer );
        m_buffers[ index ] = crimild::alloc< vulkan::Buffer >(
            getRenderDevice(),
            vertexBuffer->getClassName(),
            vertexBuffer->getBufferView()
        );
    }
    return m_buffers.at( m_index.at( id ) );
}

std::shared_ptr< vulkan::Buffer > &RenderDeviceCache::bind( const std::shared_ptr< const UniformBuffer > &uniformBuffer ) noexcept
{
    const auto id = uniformBuffer->getUniqueID();
    if ( !m_index.contains( id ) ) {
        auto index = addBoundObject( uniformBuffer );
        m_buffers[ index ] = crimild::alloc< vulkan::Buffer >(
            getRenderDevice(),
            uniformBuffer->getClassName(),
            uniformBuffer->getBufferView()
        );
    }
    return m_buffers.at( m_index.at( id ) );
}

std::shared_ptr< vulkan::Image > &RenderDeviceCache::bind( const std::shared_ptr< const crimild::Image > &source ) noexcept
{
    const auto id = source->getUniqueID();
    if ( !m_index.contains( id ) ) {
        auto index = addBoundObject( source );
        m_images[ index ] = crimild::alloc< vulkan::Image >( getRenderDevice(), source.get() );
    }
    return m_images.at( m_index.at( id ) );
}

std::shared_ptr< vulkan::ImageView > &RenderDeviceCache::bind( const std::shared_ptr< const crimild::ImageView > &source ) noexcept
{
    const auto id = source->getUniqueID();
    if ( !m_index.contains( id ) ) {
        auto index = addBoundObject( source );

        auto mipLevels = source->mipLevels;
        if ( mipLevels == 0 ) {
            mipLevels = Numerici::max( 1, source->image->getMipLevels() );
        }

        auto image = bind( source->image );

        auto layerCount = source->layerCount;
        if ( layerCount == 0 ) {
            layerCount = source->image->getLayerCount();
        }

        auto info = VkImageViewCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .flags = 0,
            .image = image->getHandle(),
            .viewType = utils::getImageViewType( source.get() ),
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
                .aspectMask = utils::getImageViewAspectFlags( source.get() ),
                .baseMipLevel = 0,
                .levelCount = mipLevels,
                .baseArrayLayer = 0,
                .layerCount = layerCount,
            },
        };

        m_imageViews[ index ] = crimild::alloc< vulkan::ImageView >( getRenderDevice(), source->getName(), image, info );
    }
    return m_imageViews.at( m_index.at( id ) );
}

std::shared_ptr< vulkan::Sampler > &RenderDeviceCache::bind( const std::shared_ptr< const crimild::Sampler > &source ) noexcept
{
    const auto id = source->getUniqueID();
    if ( !m_index.contains( id ) ) {
        auto index = addBoundObject( source );

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
        m_samplers[ index ] = crimild::alloc< vulkan::Sampler >( getRenderDevice(), source->getClassName(), info );
    }
    return m_samplers.at( m_index[ id ] );
}

bool RenderDeviceCache::hasShadowMap( const std::shared_ptr< const SharedObject > &obj ) const noexcept
{
    const auto id = getObjectId( obj );
    if ( !m_index.contains( id ) ) {
        // no bounded
        return false;
    }

    return m_shadowMaps.contains( m_index.at( id ) );
}

void RenderDeviceCache::setShadowMap( const std::shared_ptr< const SharedObject > &obj, std::shared_ptr< ShadowMap > const &shadowMap ) noexcept
{
    const auto id = getObjectId( obj );
    if ( !m_index.contains( id ) ) {
        addBoundObject( obj );
    }
    m_shadowMaps[ m_index.at( id ) ] = shadowMap;
}

std::shared_ptr< vulkan::ShadowMap > &RenderDeviceCache::getShadowMap( const std::shared_ptr< const SharedObject > &obj ) noexcept
{
    const auto id = getObjectId( obj );
    return m_shadowMaps.at( m_index.at( id ) );
}

bool RenderDeviceCache::hasUniforms( const std::shared_ptr< const SharedObject > &obj ) const noexcept
{
    const auto id = getObjectId( obj );
    if ( !m_index.contains( id ) ) {
        // no bounded
        return false;
    }

    return m_uniforms.contains( m_index.at( id ) );
}

void RenderDeviceCache::setUniforms( const std::shared_ptr< const SharedObject > &obj, std::shared_ptr< UniformBuffer > const &uniforms ) noexcept
{
    const auto id = getObjectId( obj );
    if ( !m_index.contains( id ) ) {
        addBoundObject( obj );
    }
    m_uniforms[ m_index.at( id ) ] = uniforms;
}

std::shared_ptr< UniformBuffer > &RenderDeviceCache::getUniforms( const std::shared_ptr< const SharedObject > &obj ) noexcept
{
    const auto id = getObjectId( obj );
    return m_uniforms.at( m_index.at( id ) );
}

bool RenderDeviceCache::hasDescriptorSet( const std::shared_ptr< const SharedObject > &obj ) const noexcept
{
    const auto id = getObjectId( obj );
    if ( !m_index.contains( id ) ) {
        // no bounded
        return false;
    }

    return m_descriptorSets.contains( m_index.at( id ) );
}

void RenderDeviceCache::setDescriptorSet( const std::shared_ptr< const SharedObject > &obj, std::shared_ptr< vulkan::DescriptorSet > const &descriptorSet ) noexcept
{
    const auto id = getObjectId( obj );
    if ( !m_index.contains( id ) ) {
        addBoundObject( obj );
    }
    m_descriptorSets[ m_index.at( id ) ] = descriptorSet;
}

std::shared_ptr< vulkan::DescriptorSet > &RenderDeviceCache::getDescriptorSet( const std::shared_ptr< const SharedObject > &obj ) noexcept
{
    const auto id = getObjectId( obj );
    return m_descriptorSets.at( m_index.at( id ) );
}
