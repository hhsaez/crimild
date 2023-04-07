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

#include "Rendering/VulkanBuffer.hpp"

#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

// TODO: For Static buffers, we should use a staging buffer to transfer data to GPU. For dynamic buffers,
// we might use a transfer queue...
vulkan::Buffer::Buffer( RenderDevice *device, std::string name, const BufferView *bufferView ) noexcept
    : Named( name ),
      WithRenderDevice( device ),
      m_bufferView( retain( bufferView ) )
{
    auto size = bufferView->getLength();

    VkBufferUsageFlags usage = [ & ]() -> VkBufferUsageFlags {
        switch ( bufferView->getTarget() ) {
            case BufferView::Target::VERTEX:
                return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            case BufferView::Target::INDEX:
                return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            case BufferView::Target::UNIFORM:
                return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            case BufferView::Target::STAGING:
                return VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            case BufferView::Target::STORAGE:
                return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            case BufferView::Target::IMAGE:
                return VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
            default:
                CRIMILD_LOG_ERROR( "Unsupported buffer target " );
                return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
        }
    }();

    VkBuffer buffer;
    VkDeviceMemory memory;

    auto createInfo = VkBufferCreateInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateBuffer(
            getRenderDevice()->getHandle(),
            &createInfo,
            getRenderDevice()->getAllocator(),
            &buffer
        )
    );

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements( getRenderDevice()->getHandle(), buffer, &memRequirements );

    auto allocInfo = VkMemoryAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = getRenderDevice()->getPhysicalDevice()->findMemoryType(
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateMemory(
            getRenderDevice()->getHandle(),
            &allocInfo,
            getRenderDevice()->getAllocator(),
            &memory
        )
    );

    CRIMILD_VULKAN_CHECK(
        vkBindBufferMemory(
            getRenderDevice()->getHandle(),
            buffer,
            memory,
            0
        )
    );

    setHandle( buffer );
    m_memory = memory;

    if ( bufferView->getData() != nullptr ) {
        // Force copy data to GPU since this is the first initialization
        update( true );
    }

    getRenderDevice()->setObjectName( ( uint64_t ) getHandle(), VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, name );
}

vulkan::Buffer::~Buffer( void ) noexcept
{
    vkDestroyBuffer( getRenderDevice()->getHandle(), getHandle(), getRenderDevice()->getAllocator() );
    setHandle( VK_NULL_HANDLE );

    vkFreeMemory( getRenderDevice()->getHandle(), m_memory, getRenderDevice()->getAllocator() );
    m_memory = VK_NULL_HANDLE;
}

void vulkan::Buffer::update( bool force ) noexcept
{
    if ( !force && m_bufferView->getUsage() != BufferView::Usage::DYNAMIC ) {
        return;
    }

    const auto size = m_bufferView->getLength();

    void *dstData = nullptr;

    CRIMILD_VULKAN_CHECK(
        vkMapMemory(
            getRenderDevice()->getHandle(),
            m_memory,
            0,
            size,
            0,
            &dstData
        )
    );

    memcpy( dstData, m_bufferView->getData(), ( size_t ) size );

    vkUnmapMemory( getRenderDevice()->getHandle(), m_memory );
}
