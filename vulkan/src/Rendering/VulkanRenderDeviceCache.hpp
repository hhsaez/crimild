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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_DEVICE_CACHE
#define CRIMILD_VULKAN_RENDERING_RENDER_DEVICE_CACHE

#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"

namespace crimild {

    class IndexBuffer;
    class ImageView;
    class Light;
    class Sampler;
    class UniformBuffer;
    class VertexBuffer;

}

namespace crimild::vulkan {

    class Buffer;
    class Image;
    class ImageView;
    class Sampler;
    class ShadowMap;
    class DescriptorSet;

    /**
     * \todo Each instance of RenderDeviceCache keeps a copy of everything bound to it. This is
     * inefficient for static objects, which are loaded once and never modified again. In this
     * case, it will be beneficial to bind these objects to a shared cache, avoinging duplicating
     * data in the GPU.
     */
    class RenderDeviceCache
        : public SharedObject,
          public WithRenderDevice {
    public:
        RenderDeviceCache( RenderDevice *device ) noexcept;
        ~RenderDeviceCache( void ) noexcept;

        void onBeforeFrame( void ) noexcept;
        void onAfterFrame( void ) noexcept;

        std::shared_ptr< Buffer > &bind( const std::shared_ptr< const crimild::IndexBuffer > &indices ) noexcept;
        std::shared_ptr< Buffer > &bind( const std::shared_ptr< const crimild::VertexBuffer > &vertices ) noexcept;
        std::shared_ptr< Buffer > &bind( const std::shared_ptr< const crimild::UniformBuffer > &uniforms ) noexcept;

        std::shared_ptr< Image > &bind( const std::shared_ptr< const crimild::Image > &image ) noexcept;
        std::shared_ptr< ImageView > &bind( const std::shared_ptr< const crimild::ImageView > &imageView ) noexcept;
        std::shared_ptr< Sampler > &bind( const std::shared_ptr< const crimild::Sampler > &sampler ) noexcept;

        bool hasShadowMap( const std::shared_ptr< const SharedObject > &obj ) const noexcept;
        void setShadowMap( const std::shared_ptr< const SharedObject > &obj, std::shared_ptr< ShadowMap > const &shadowMap ) noexcept;
        std::shared_ptr< ShadowMap > &getShadowMap( const std::shared_ptr< const SharedObject > &obj ) noexcept;

        bool hasUniforms( const std::shared_ptr< const SharedObject > &obj ) const noexcept;
        void setUniforms( const std::shared_ptr< const SharedObject > &obj, std::shared_ptr< UniformBuffer > const &uniforms ) noexcept;
        std::shared_ptr< UniformBuffer > &getUniforms( const std::shared_ptr< const SharedObject > &obj ) noexcept;

        bool hasDescriptorSet( const std::shared_ptr< const SharedObject > &obj ) const noexcept;
        void setDescriptorSet( const std::shared_ptr< const SharedObject > &obj, std::shared_ptr< vulkan::DescriptorSet > const &descriptorSet ) noexcept;
        std::shared_ptr< vulkan::DescriptorSet > &getDescriptorSet( const std::shared_ptr< const SharedObject > &obj ) noexcept;

    private:
        size_t getObjectId( const std::shared_ptr< const SharedObject > &obj ) const noexcept
        {
            return reinterpret_cast< size_t >( obj.get() );
        }

        size_t getNewObjectIndex( void ) noexcept;

        size_t addBoundObject( const std::shared_ptr< const SharedObject > &obj ) noexcept;

    private:
        // Small optimization: keep an index of positions into bound objects
        // to use when checking if a given object is already bound. This avoid
        // doing a linear search.
        std::unordered_map< size_t, size_t > m_index;
        std::unordered_map< size_t, size_t > m_reverseIndex;

        std::vector< std::weak_ptr< const SharedObject > > m_boundObjects;

        std::unordered_map< size_t, std::shared_ptr< vulkan::Buffer > > m_buffers;
        std::unordered_map< size_t, std::shared_ptr< vulkan::Image > > m_images;
        std::unordered_map< size_t, std::shared_ptr< vulkan::ImageView > > m_imageViews;
        std::unordered_map< size_t, std::shared_ptr< vulkan::Sampler > > m_samplers;
        std::unordered_map< size_t, std::shared_ptr< vulkan::ShadowMap > > m_shadowMaps;
        std::unordered_map< size_t, std::shared_ptr< UniformBuffer > > m_uniforms;
        std::unordered_map< size_t, std::shared_ptr< vulkan::DescriptorSet > > m_descriptorSets;
    };

}

#endif
