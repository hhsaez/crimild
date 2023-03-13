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

#ifndef CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_TARGET
#define CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_TARGET

#include "Foundation/Named.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"

namespace crimild::vulkan {

    class Image;
    class ImageView;
    class DescriptorSet;
    class Sampler;

    class RenderTarget
        : public SharedObject,
          public Named,
          public WithRenderDevice {
    public:
        RenderTarget(
            RenderDevice *device,
            std::string name,
            VkFormat format,
            const VkExtent2D &extent
        ) noexcept;
        virtual ~RenderTarget( void ) noexcept;

        inline const VkExtent2D &getExtent( void ) const noexcept { return m_extent; }
        inline VkFormat getFormat( void ) const noexcept { return m_format; }

        inline bool isColor( void ) const noexcept { return m_formatIsColor; }
        inline bool isDepthStencil( void ) const noexcept { return m_formatIsDepthStencil; }

        inline void setClearValue( const VkClearValue &clearValue ) noexcept { m_clearValue = clearValue; }
        inline const VkClearValue getClearValue( void ) const noexcept { return m_clearValue; }

        inline std::shared_ptr< Image > &getImage( void ) noexcept { return m_image; }
        inline std::shared_ptr< ImageView > &getImageView( void ) noexcept { return m_imageView; }
        inline std::shared_ptr< Sampler > &getSampler( void ) noexcept { return m_sampler; }

        void resize( const VkExtent2D &extent ) noexcept;

    private:
        void init( void ) noexcept;
        void cleanup( void ) noexcept;

    private:
        VkExtent2D m_extent;
        VkFormat m_format = VK_FORMAT_UNDEFINED;
        bool m_formatIsColor = false;
        bool m_formatIsDepthStencil = false;
        std::shared_ptr< vulkan::Image > m_image;
        std::shared_ptr< vulkan::ImageView > m_imageView;
        std::shared_ptr< vulkan::Sampler > m_sampler;
        std::shared_ptr< vulkan::DescriptorSet > m_descriptorSet;
        VkClearValue m_clearValue;
    };

}

#endif
