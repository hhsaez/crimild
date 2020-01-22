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
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool BufferManager::bind( Buffer *buffer ) noexcept
{
    if ( validate( buffer ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan Buffer" );

    auto renderDevice = getRenderDevice();
    auto swapchain = renderDevice->getSwapchain();

    VkDeviceSize bufferSize = buffer->getSize();
    crimild::Size count = 1;

    VkBufferUsageFlags usage = 0;
    switch ( buffer->getUsage() ) {
        case Buffer::Usage::VERTEX_BUFFER:
            usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        case Buffer::Usage::INDEX_BUFFER:
            usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;
        case Buffer::Usage::UNIFORM_BUFFER:
            usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            count = swapchain->images.size();
            break;
        default:
            CRIMILD_LOG_ERROR( "Invalid buffer usage value" );
            return false;
    }

    containers::Array< VkBuffer > bufferHandlers( count );
    containers::Array< VkDeviceMemory > bufferMemories( count );

    for ( auto i = 0l; i < count; i++ ) {
        VkBuffer bufferHandler;
        VkDeviceMemory bufferMemory;

        utils::createBuffer(
            renderDevice,
            utils::BufferDescriptor {
            	.size = bufferSize,
            	.usage = usage,
            	.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        	},
            bufferHandler,
            bufferMemory
        );

        if ( buffer->getRawData() != nullptr ) {
            utils::copyToBuffer(
                renderDevice->handler,
                bufferMemory,
                buffer->getRawData(),
                bufferSize
            );
        }

        bufferHandlers[ i ] = bufferHandler;
        bufferMemories[ i ] = bufferMemory;
    }

    setBindInfo(
    	buffer,
        {
        	.bufferHandlers = bufferHandlers,
        	.bufferMemories = bufferMemories,
    	}
    );

    return ManagerImpl::bind( buffer );
}

crimild::Bool BufferManager::unbind( Buffer *buffer ) noexcept
{
    if ( !validate( buffer ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbinding Vulkan buffer" );

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return false;
    }

    auto bindInfo = getBindInfo( buffer );
    auto N = bindInfo.bufferHandlers.size();
    for ( int i = 0; i < N; i++ ) {
        vkDestroyBuffer( renderDevice->handler, bindInfo.bufferHandlers[ i ], nullptr );
        vkFreeMemory( renderDevice->handler, bindInfo.bufferMemories[ i ], nullptr );
    }

    removeBindInfo( buffer );

    return RenderResourceManager< crimild::Buffer >::unbind( buffer );
}

/*

SharedPointer< vulkan::Buffer > vulkan::BufferManager::create( vulkan::Buffer::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan buffer" );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    VkDeviceSize bufferSize = descriptor.size;

    VkBufferUsageFlags usage = 0;
    switch ( descriptor.usage ) {
        case Buffer::Usage::VERTEX_BUFFER:
            usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        case Buffer::Usage::INDEX_BUFFER:
            usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;
        case Buffer::Usage::UNIFORM_BUFFER:
            usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            break;
        default:
            return nullptr;
            break;
    }

    VkBuffer bufferHandler;
    VkDeviceMemory bufferMemory;

    if ( descriptor.data == nullptr ) {
        // No data. No need to create staging buffer
        createBuffer(
            renderDevice,
            bufferSize,
            usage,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            bufferHandler,
            bufferMemory
         );
    }
    else {
        usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VkBuffer stagingBufferHandler;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(
            renderDevice,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBufferHandler,
            stagingBufferMemory
        );

        void *data = nullptr;
        CRIMILD_VULKAN_CHECK(
            vkMapMemory(
                renderDevice->handler,
                stagingBufferMemory,
                0,
                bufferSize,
                0,
                &data
            )
        );

        if ( descriptor.data != nullptr ) {
            memcpy( data, descriptor.data, ( size_t ) bufferSize );
        }

        vkUnmapMemory( renderDevice->handler, stagingBufferMemory );

        createBuffer(
            renderDevice,
            bufferSize,
            usage,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            bufferHandler,
            bufferMemory
         );

        copyBuffer(
            renderDevice,
            descriptor.commandPool,
            stagingBufferHandler,
            bufferHandler,
            bufferSize
        );

        vkDestroyBuffer( renderDevice->handler, stagingBufferHandler, nullptr );
        vkFreeMemory( renderDevice->handler, stagingBufferMemory, nullptr );
    }

    auto buffer = crimild::alloc< vulkan::Buffer >();
    buffer->renderDevice = renderDevice;
    buffer->manager = this;
    buffer->handler = bufferHandler;
    buffer->memory = bufferMemory;
    buffer->size = descriptor.size;
    insert( crimild::get_ptr( buffer ) );
    return buffer;
}

void vulkan::BufferManager::destroy( vulkan::Buffer *buffer ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan buffer" );

    if ( buffer->renderDevice != nullptr ) {
        if ( buffer->handler != VK_NULL_HANDLE ) {
        	vkDestroyBuffer(
            	buffer->renderDevice->handler,
        		buffer->handler,
            	nullptr
        	);
        }
        if ( buffer->memory != VK_NULL_HANDLE ) {
			vkFreeMemory(
         		buffer->renderDevice->handler,
             	buffer->memory,
             	nullptr
         	);
        }
    }

    buffer->renderDevice = nullptr;
    buffer->handler = VK_NULL_HANDLE;
    buffer->memory = VK_NULL_HANDLE;
    buffer->manager = nullptr;
    buffer->size = 0;
    erase( buffer );
}
 */

void BufferManager::copyBuffer( RenderDevice *renderDevice, CommandPool *commandPool, VkBuffer srcBufferHandler, VkBuffer dstBufferHandler, VkDeviceSize size ) const noexcept
{
    /*
	auto commandBuffer = renderDevice->create(
        CommandBuffer::Descriptor {
        	.commandPool = commandPool,
    	}
    );

    commandBuffer->begin( CommandBuffer::Usage::ONE_TIME_SUBMIT );

    auto src = crimild::alloc< Buffer >();
    src->handler = srcBufferHandler;
    auto dst = crimild::alloc< Buffer >();
    dst->handler = dstBufferHandler;
    commandBuffer->copy( crimild::get_ptr( src ), 0, crimild::get_ptr( dst ), 0, size );

    commandBuffer->end();

    renderDevice->submit(
        crimild::get_ptr( commandBuffer ),
    	true
    );
     */
}

void BufferManager::updateUniformBuffers( crimild::Size index ) noexcept
{
    auto renderDevice = getRenderDevice();
    each( [ this, renderDevice, index ]( const Buffer *buffer, const BufferBindInfo &bindInfo ) {
        if ( buffer->getUsage() == Buffer::Usage::UNIFORM_BUFFER ) {
            if ( bindInfo.bufferMemories.size() > index ) {
            	updateBuffer( renderDevice, bindInfo.bufferMemories[ index ], buffer->getRawData(), buffer->getSize() );
            }
        }
    });
}

void BufferManager::updateBuffer( RenderDevice *renderDevice, VkDeviceMemory bufferMemory, const void *srcData, crimild::Size size ) noexcept
{
    void *dstData = nullptr;
    CRIMILD_VULKAN_CHECK(
         vkMapMemory(
            renderDevice->handler,
            bufferMemory,
            0,
            size,
            0,
            &dstData
        )
    );

    memcpy( dstData, srcData, size );

    vkUnmapMemory( renderDevice->handler, bufferMemory );
}
