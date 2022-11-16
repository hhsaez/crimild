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

#include "Rendering/VulkanImageView.hpp"

#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;

vulkan::ImageView::ImageView( const vulkan::RenderDevice *rd, const SharedPointer< vulkan::Image > &image ) noexcept
    : vulkan::ImageView(
        rd,
        [ & ] {
            auto info = vulkan::initializers::imageViewCreateInfo();
            info.image = *image;
            info.subresourceRange.aspectMask =
                rd->formatIsColor( image->getFormat() )
                    ? VK_IMAGE_ASPECT_COLOR_BIT
                : rd->formatHasStencilComponent( image->getFormat() )
                    ? VK_IMAGE_ASPECT_STENCIL_BIT
                    : VK_IMAGE_ASPECT_DEPTH_BIT;
            info.format = image->getFormat();
            return info;
        }()
    )
{
}

vulkan::ImageView::ImageView( const vulkan::RenderDevice *rd, const VkImageViewCreateInfo &createInfo ) noexcept
    : WithConstRenderDevice( rd )
{
    CRIMILD_VULKAN_CHECK(
        vkCreateImageView(
            getRenderDevice()->getHandle(),
            &createInfo,
            nullptr,
            &m_imageView
        )
    );
}

vulkan::ImageView::~ImageView( void ) noexcept
{
    vkDestroyImageView( getRenderDevice()->getHandle(), m_imageView, nullptr );
    m_imageView = VK_NULL_HANDLE;
}

void vulkan::ImageView::setName( std::string_view name ) noexcept
{
    getRenderDevice()->setObjectName( m_imageView, name );
}
