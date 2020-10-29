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

#include "Rendering/VulkanStorageBuffer.hpp"

#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool StorageBufferManager::bind( StorageBuffer *storageBuffer ) noexcept
{
    if ( validate( storageBuffer ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan StorageBuffer" );

    auto renderDevice = getRenderDevice();
    auto swapchain = renderDevice->getSwapchain();
    auto bufferView = storageBuffer->getBufferView();
    auto bufferSize = bufferView->getLength();

    VkDeviceSize storageBufferSize = bufferView->getLength();
    crimild::Size count = swapchain->images.size();

    VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    Array< VkBuffer > bufferHandlers( count );
    Array< VkDeviceMemory > bufferMemories( count );

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
            bufferMemory );

        if ( bufferView->getData() != nullptr ) {
            utils::copyToBuffer(
                renderDevice->handler,
                bufferMemory,
                bufferView->getData(),
                bufferSize );
        }

        bufferHandlers[ i ] = bufferHandler;
        bufferMemories[ i ] = bufferMemory;
    }

    setBindInfo(
        storageBuffer,
        {
            .bufferHandlers = bufferHandlers,
            .bufferMemories = bufferMemories,
        } );

    return ManagerImpl::bind( storageBuffer );
}

crimild::Bool StorageBufferManager::unbind( StorageBuffer *storageBuffer ) noexcept
{
    if ( !validate( storageBuffer ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbinding Vulkan storageBuffer" );

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return false;
    }

    auto bindInfo = getBindInfo( storageBuffer );
    auto N = bindInfo.bufferHandlers.size();
    for ( int i = 0; i < N; i++ ) {
        vkDestroyBuffer( renderDevice->handler, bindInfo.bufferHandlers[ i ], nullptr );
        vkFreeMemory( renderDevice->handler, bindInfo.bufferMemories[ i ], nullptr );
    }

    removeBindInfo( storageBuffer );

    return ManagerImpl::unbind( storageBuffer );
}

Bool StorageBufferManager::mapFromDevice( StorageBuffer *storageBuffer ) noexcept
{
    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return false;
    }

    auto bindInfo = getBindInfo( storageBuffer );

    void *srcData = nullptr;

    CRIMILD_VULKAN_CHECK(
        vkMapMemory(
            renderDevice->handler,
            bindInfo.bufferMemories[ 0 ],
            0,
            storageBuffer->getBufferView()->getLength(),
            0,
            &srcData ) );

    memcpy( storageBuffer->getBufferView()->getData(), srcData, storageBuffer->getBufferView()->getLength() );

    vkUnmapMemory( renderDevice->handler, bindInfo.bufferMemories[ 0 ] );

	return true;
}
