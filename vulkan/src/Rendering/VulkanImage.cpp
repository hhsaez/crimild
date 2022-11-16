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

#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;

vulkan::Image::Image( const vulkan::RenderDevice *rd, const VkImageCreateInfo &createInfo ) noexcept
    : WithConstRenderDevice( rd )
{
    m_format = createInfo.format;
    m_extent = createInfo.extent;
    m_layout = createInfo.initialLayout;
    m_mipLevels = createInfo.mipLevels;
    m_arrayLayers = createInfo.arrayLayers;

    CRIMILD_VULKAN_CHECK(
        vkCreateImage(
            getRenderDevice()->getHandle(),
            &createInfo,
            nullptr,
            &m_image
        )
    );
}

vulkan::Image::Image( const vulkan::RenderDevice *rd, VkImage image, const VkExtent3D &extent ) noexcept
    : WithConstRenderDevice( rd )
{
    m_image = image;
    m_extent = extent;
    m_readonly = true;
}

vulkan::Image::~Image( void ) noexcept
{
    if ( m_readonly ) {
        m_image = VK_NULL_HANDLE;
        return;
    }

    if ( m_memory != VK_NULL_HANDLE ) {
        vkFreeMemory( getRenderDevice()->getHandle(), m_memory, nullptr );
        m_memory = VK_NULL_HANDLE;
    }

    if ( m_image != VK_NULL_HANDLE ) {
        vkDestroyImage( getRenderDevice()->getHandle(), m_image, nullptr );
        m_image = VK_NULL_HANDLE;
    }
}

void vulkan::Image::setName( std::string_view name ) noexcept
{
    assert( !m_readonly && "Attempting to set name to a read-only image" );
    getRenderDevice()->setObjectName( m_image, name );
}

void vulkan::Image::allocateMemory( void ) noexcept
{
    assert( !m_readonly && "Attempting to allocate memory for a read-only image" );

    // TODO: Add a new method getMemoryRequirements(properties). That way it can be customized if needed
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements( getRenderDevice()->getHandle(), m_image, &memRequirements );

    auto allocInfo = vulkan::initializers::memoryAllocateInfo();
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = getRenderDevice()->getPhysicalDevice()->findMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    allocateMemory( allocInfo );
}

void vulkan::Image::allocateMemory( const VkMemoryAllocateInfo &allocInfo ) noexcept
{
    assert( !m_readonly && "Attempting to allocate memory for a read-only image" );

    CRIMILD_VULKAN_CHECK(
        vkAllocateMemory(
            getRenderDevice()->getHandle(),
            &allocInfo,
            nullptr,
            &m_memory
        )
    );

    CRIMILD_VULKAN_CHECK(
        vkBindImageMemory(
            getRenderDevice()->getHandle(),
            m_image,
            m_memory,
            0
        )
    );
}

void vulkan::Image::transitionLayout( VkImageLayout newLayout ) const noexcept
{
    assert( !m_readonly && "Attempting to transition layout of a read-only image" );

    getRenderDevice()->transitionImageLayout(
        m_image,
        m_format,
        m_layout,
        newLayout,
        m_mipLevels,
        m_arrayLayers
    );

    m_layout = newLayout;
}

void vulkan::Image::transitionLayout( VkImageLayout oldLayout, VkImageLayout newLayout ) const noexcept
{
    assert( !m_readonly && "Attempting to transition layout of a read-only image" );

    getRenderDevice()->transitionImageLayout(
        m_image,
        m_format,
        oldLayout,
        newLayout,
        m_mipLevels,
        m_arrayLayers
    );

    m_layout = newLayout;
}

void vulkan::Image::transitionLayout( VkCommandBuffer commandBuffer, VkImageLayout newLayout ) const noexcept
{
    assert( !m_readonly && "Attempting to transition layout of a read-only image" );

    // Transition all layers, individually.
    for ( uint32_t baseArrayLayer = 0; baseArrayLayer < m_arrayLayers; ++baseArrayLayer ) {
        getRenderDevice()->transitionImageLayout(
            commandBuffer,
            m_image,
            m_format,
            m_layout,
            newLayout,
            m_mipLevels,
            1,
            baseArrayLayer
        );
    }

    m_layout = newLayout;
}

void vulkan::Image::transitionLayout( VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout ) const noexcept
{
    assert( !m_readonly && "Attempting to transition layout of a read-only image" );

    // Transition all layers, individually.
    for ( uint32_t baseArrayLayer = 0; baseArrayLayer < m_arrayLayers; ++baseArrayLayer ) {
        getRenderDevice()->transitionImageLayout(
            commandBuffer,
            m_image,
            m_format,
            oldLayout,
            newLayout,
            m_mipLevels,
            1,
            baseArrayLayer
        );
    }

    m_layout = newLayout;
}

void vulkan::Image::copy( VkCommandBuffer commandBuffer, SharedPointer< Image > const &src ) noexcept
{
    auto copyRegion = vulkan::initializers::imageCopy();
    copyRegion.srcSubresource.aspectMask = getRenderDevice()->formatIsColor( src->m_format ) ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT;
    copyRegion.dstSubresource.aspectMask = getRenderDevice()->formatIsColor( m_format ) ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT;
    copyRegion.extent = m_extent;
    copy( commandBuffer, src, copyRegion );
}

void vulkan::Image::copy( VkCommandBuffer commandBuffer, SharedPointer< Image > const &src, const VkImageCopy &copyRegion ) noexcept
{
    vkCmdCopyImage(
        commandBuffer,
        *src,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        *this,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copyRegion
    );
}
