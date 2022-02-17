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

#include "Rendering/VulkanIndexBuffer.hpp"

#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDeviceOLD.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool IndexBufferManager::bind( IndexBuffer *indexBuffer ) noexcept
{
    if ( validate( indexBuffer ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan IndexBuffer" );

    auto renderDevice = getRenderDevice();
    auto bufferView = indexBuffer->getBufferView();
    auto bufferSize = bufferView->getLength();

    VkDeviceSize indexBufferSize = bufferView->getLength();

    VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

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

    setBindInfo(
        indexBuffer,
        {
            .bufferHandler = bufferHandler,
            .bufferMemory = bufferMemory,
        } );

    return ManagerImpl::bind( indexBuffer );
}

crimild::Bool IndexBufferManager::unbind( IndexBuffer *indexBuffer ) noexcept
{
    if ( !validate( indexBuffer ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbinding Vulkan indexBuffer" );

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return false;
    }

    auto bindInfo = getBindInfo( indexBuffer );
    vkDestroyBuffer( renderDevice->handler, bindInfo.bufferHandler, nullptr );
    vkFreeMemory( renderDevice->handler, bindInfo.bufferMemory, nullptr );

    removeBindInfo( indexBuffer );

    return ManagerImpl::unbind( indexBuffer );
}

void IndexBufferManager::updateIndexBuffers( void ) noexcept
{
    // TODO: This is a slow operation. Not only it needs to iterate over all
    // available index buffers to get the ones that actually need updating, but
    // also it's copying data to the device in a synchronous way.
    auto renderDevice = getRenderDevice();
    each( [ this, renderDevice ]( IndexBuffer *ibo, IndexBufferBindInfo &bindInfo ) {
        if ( ibo->getBufferView()->getUsage() == BufferView::Usage::DYNAMIC ) {
            utils::copyToBuffer(
                renderDevice->handler,
                bindInfo.bufferMemory,
                ibo->getBufferView()->getData(),
                ibo->getBufferView()->getLength() );
        }
    } );
}
