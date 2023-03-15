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

#include "Rendering/Image.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;

vulkan::Image::Image(
    const vulkan::RenderDevice *device,
    const VkImageCreateInfo &info,
    std::string name
) noexcept
    : WithConstRenderDevice( device ),
      Named( name )
{
    m_format = info.format;
    m_extent = info.extent;
    m_layout = info.initialLayout;
    m_mipLevels = info.mipLevels;
    m_arrayLayers = info.arrayLayers;
    m_aspectFlags =
        device->formatIsColor( getFormat() )
            ? VK_IMAGE_ASPECT_COLOR_BIT
        : device->formatHasStencilComponent( getFormat() )
            ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT
            : VK_IMAGE_ASPECT_DEPTH_BIT;

    CRIMILD_VULKAN_CHECK(
        vkCreateImage(
            getRenderDevice()->getHandle(),
            &info,
            getRenderDevice()->getAllocator(),
            &m_handle
        )
    );

    device->setObjectName( getHandle(), name );
}

vulkan::Image::Image(
    const RenderDevice *device,
    const VkExtent2D &extent,
    VkFormat format,
    VkImageUsageFlags usage,
    std::string name
) noexcept
    : vulkan::Image::Image(
        device,
        [ & ] {
            auto info = createInfo();
            info.extent = { extent.width, extent.height, 1 };
            info.format = format;
            info.usage = usage;
            return info;
        }(),
        name
    )
{
    // no-op
}

vulkan::Image::Image( const vulkan::RenderDevice *device, const crimild::Image *image ) noexcept
    : WithConstRenderDevice( device ),
      Named( image->getName() )
{
    uint32_t width = image->extent.width;
    uint32_t height = image->extent.height;
    auto mipLevels = image->getMipLevels();
    auto arrayLayers = image->getLayerCount();
    auto type = image->type;

    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    if ( image->getBufferView() != nullptr ) {
        // If image has data, it will be used for transfer operations
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    } else {
        // If image has no data, then it's used as an attachment
        if ( utils::formatIsColor( image->format ) ) {
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        } else if ( utils::formatIsDepthStencil( image->format ) ) {
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    auto createInfo = VkImageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = utils::getFormat( image->format ),
        .extent = {
            .width = width,
            .height = height,
            .depth = 1,
        },
        .mipLevels = mipLevels,
        .arrayLayers = arrayLayers,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    m_handle = VK_NULL_HANDLE;
    m_memory = VK_NULL_HANDLE;

    CRIMILD_VULKAN_CHECK(
        vkCreateImage(
            getRenderDevice()->getHandle(),
            &createInfo,
            getRenderDevice()->getAllocator(),
            &m_handle
        )
    );

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements( getRenderDevice()->getHandle(), getHandle(), &memRequirements );

    auto allocInfo = VkMemoryAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = utils::findMemoryType(
            getRenderDevice()->getPhysicalDevice()->getHandle(),
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        ),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateMemory(
            getRenderDevice()->getHandle(),
            &allocInfo,
            getRenderDevice()->getAllocator(),
            &m_memory
        )
    );

    CRIMILD_VULKAN_CHECK(
        vkBindImageMemory(
            getRenderDevice()->getHandle(),
            m_handle,
            m_memory,
            0
        )
    );

    if ( image->getBufferView() != nullptr ) {
        // Image has pixel data. Upload it

        VkDeviceSize imageSize = image->getBufferView()->getLength();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

        getRenderDevice()->createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory
        );

        getRenderDevice()->copyToBuffer( stagingBufferMemory, image->getBufferView()->getData(), imageSize );

        getRenderDevice()->transitionImageLayout(
            getHandle(),
            utils::getFormat( image->format ),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            mipLevels,
            arrayLayers
        );

        getRenderDevice()->copyBufferToImage(
            stagingBuffer,
            getHandle(),
            width,
            height,
            arrayLayers
        );

        if ( type == crimild::Image::Type::IMAGE_2D_CUBEMAP ) {
            // No mipmaps. Transition to SHADER_READ_OPTIMAL
            getRenderDevice()->transitionImageLayout(
                getHandle(),
                utils::getFormat( image->format ),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                mipLevels,
                arrayLayers
            );
        } else {
            // Automatically transitions to SHADER_READ_OPTIMAL layout
            getRenderDevice()->generateMipmaps(
                getHandle(),
                utils::getFormat( image->format ),
                width,
                height,
                mipLevels
            );
        };

        // TODO: Support dynamic images!!
        // if ( image->getBufferView() == nullptr || image->getBufferView()->getUsage() == BufferView::Usage::STATIC ) {
        // We won't be using the staging buffers for static images anymore
        vkDestroyBuffer(
            getRenderDevice()->getHandle(),
            stagingBuffer,
            getRenderDevice()->getAllocator()
        );

        vkFreeMemory(
            getRenderDevice()->getHandle(),
            stagingBufferMemory,
            getRenderDevice()->getAllocator()
        );
    }
}

vulkan::Image::Image( const vulkan::RenderDevice *rd, VkImage image, const VkExtent3D &extent, std::string name ) noexcept
    : WithConstRenderDevice( rd ),
      Named( name )
{
    m_handle = image;
    m_extent = extent;
    m_readonly = true;
}

vulkan::Image::~Image( void ) noexcept
{
    if ( m_readonly ) {
        m_handle = VK_NULL_HANDLE;
        return;
    }

    if ( m_memory != VK_NULL_HANDLE ) {
        vkFreeMemory(
            getRenderDevice()->getHandle(),
            m_memory,
            getRenderDevice()->getAllocator()
        );
        m_memory = VK_NULL_HANDLE;
    }

    if ( m_handle != VK_NULL_HANDLE ) {
        vkDestroyImage(
            getRenderDevice()->getHandle(),
            m_handle,
            getRenderDevice()->getAllocator()
        );
        m_handle = VK_NULL_HANDLE;
    }
}

void vulkan::Image::allocateMemory( void ) noexcept
{
    assert( !m_readonly && "Attempting to allocate memory for a read-only image" );

    // TODO: Add a new method getMemoryRequirements(properties). That way it can be customized if needed
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements( getRenderDevice()->getHandle(), m_handle, &memRequirements );

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
            m_handle,
            m_memory,
            0
        )
    );
}

void vulkan::Image::transitionLayout( VkImageLayout newLayout ) const noexcept
{
    assert( !m_readonly && "Attempting to transition layout of a read-only image" );

    getRenderDevice()->transitionImageLayout(
        m_handle,
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
        m_handle,
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
            m_handle,
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
            m_handle,
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
        src->getHandle(),
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        this->getHandle(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copyRegion
    );
}
