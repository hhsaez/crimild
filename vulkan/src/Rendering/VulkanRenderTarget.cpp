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

#include "Rendering/VulkanRenderTarget.hpp"

#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanSampler.hpp"

using namespace crimild;
using namespace crimild::vulkan;

RenderTarget::RenderTarget(
    RenderDevice *device,
    std::string name,
    std::shared_ptr< vulkan::ImageView > const &imageView
) noexcept
    : WithRenderDevice( device ),
      Named( name ),
      m_extent(
          VkExtent2D {
              imageView->getExtent().width,
              imageView->getExtent().height,
          }
      ),
      m_image( imageView->getImage() ),
      m_imageView( imageView ),
      m_format( imageView->getImage()->getFormat() ),
      m_formatIsColor( device->formatIsColor( m_format ) ),
      m_formatIsDepthStencil( device->formatIsDepthStencil( m_format ) )
{
    if ( !isColor() && !isDepthStencil() ) {
        CRIMILD_LOG_ERROR( "Invalid render target format: ", m_format );
        return;
    }

    m_sampler = crimild::alloc< Sampler >(
        device,
        getName() + "/Sampler",
        VK_FILTER_LINEAR,
        VK_SAMPLER_MIPMAP_MODE_LINEAR,
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
    );

    m_descriptorSet = crimild::alloc< DescriptorSet >(
        device,
        getName() + "/DescriptorSet",
        std::vector< Descriptor > {
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = m_imageView,
                .sampler = m_sampler,
            },
        }
    );

    if ( isColor() ) {
        m_clearValue = VkClearValue {
            .color = {
                .float32 { 0, 0, 0, 0 },
            },
        };
    } else if ( isDepthStencil() ) {
        m_clearValue = VkClearValue {
            .depthStencil = { 1, 0 },
        };
    }
}

RenderTarget::RenderTarget(
    RenderDevice *device,
    std::string name,
    VkFormat format,
    const VkExtent2D &extent
) noexcept
    : WithRenderDevice( device ),
      Named( name ),
      m_extent( extent ),
      m_format( format ),
      m_formatIsColor( device->formatIsColor( format ) ),
      m_formatIsDepthStencil( device->formatIsDepthStencil( format ) )
{
    init();
}

RenderTarget::~RenderTarget( void ) noexcept
{
    cleanup();
}

void RenderTarget::resize( const VkExtent2D &extent ) noexcept
{
    cleanup();
    init();
}

void RenderTarget::init( void ) noexcept
{
    auto device = getRenderDevice();

    if ( !isColor() && !isDepthStencil() ) {
        CRIMILD_LOG_ERROR( "Invalid render target format: ", m_format );
        return;
    }

    m_image = crimild::alloc< vulkan::Image >(
        device,
        m_extent,
        m_format,
        isColor()
            ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT
            : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        getName() + "/Image"
    );
    m_image->allocateMemory();

    m_imageView = crimild::alloc< ImageView >(
        device,
        getName() + "/ImageView",
        m_image
    );

    m_sampler = crimild::alloc< Sampler >(
        device,
        getName() + "/Sampler",
        VK_FILTER_LINEAR,
        VK_SAMPLER_MIPMAP_MODE_LINEAR,
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
    );

    m_descriptorSet = crimild::alloc< DescriptorSet >(
        device,
        getName() + "/DescriptorSet",
        std::vector< Descriptor > {
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = m_imageView,
                .sampler = m_sampler,
            },
        }
    );

    if ( isColor() ) {
        m_clearValue = VkClearValue {
            .color = {
                .float32 { 0, 0, 0, 0 },
            },
        };
    } else if ( isDepthStencil() ) {
        m_clearValue = VkClearValue {
            .depthStencil = { 1, 0 },
        };
    }
}

void RenderTarget::cleanup( void ) noexcept
{
    m_descriptorSet = nullptr;
    m_sampler = nullptr;
    m_imageView = nullptr;
    m_image = nullptr;
}
