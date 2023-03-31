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

vulkan::ImageView::ImageView(
    vulkan::RenderDevice *device,
    std::string name,
    std::shared_ptr< vulkan::Image > const &image,
    uint32_t baseArrayLayer
) noexcept
    : vulkan::ImageView(
        device,
        name,
        image,
        [ & ] {
            auto info = createInfo();
            info.image = image->getHandle();
            info.subresourceRange.aspectMask = image->getAspectFlags();
            info.subresourceRange.baseArrayLayer = baseArrayLayer;
            info.format = image->getFormat();
            return info;
        }()
    )
{
}

vulkan::ImageView::ImageView(
    vulkan::RenderDevice *device,
    std::string name,
    std::shared_ptr< vulkan::Image > const &image,
    const VkImageViewCreateInfo &createInfo
) noexcept
    : WithRenderDevice( device ),
      Named( name ),
      m_image( image )
{
    VkImageView handle;
    CRIMILD_VULKAN_CHECK(
        vkCreateImageView(
            getRenderDevice()->getHandle(),
            &createInfo,
            getRenderDevice()->getAllocator(),
            &handle
        )
    );
    setHandle( handle );

    device->setObjectName( handle, name );

    m_subresourceRange = createInfo.subresourceRange;
}

vulkan::ImageView::~ImageView( void ) noexcept
{
    vkDestroyImageView(
        getRenderDevice()->getHandle(),
        getHandle(),
        getRenderDevice()->getAllocator()
    );
    setHandle( VK_NULL_HANDLE );
}

const VkExtent3D &vulkan::ImageView::getExtent( void ) const noexcept
{
    return m_image->getExtent();
}
