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

#include "Rendering/VulkanUniformBuffer.hpp"

#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VulkanPhysicalDevice.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool UniformBufferManager::bind( UniformBuffer *uniformBuffer ) noexcept
{
    if ( validate( uniformBuffer ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan UniformBuffer" );

    auto renderDevice = getRenderDevice();
    auto bufferView = uniformBuffer->getBufferView();
    auto bufferSize = bufferView->getLength();

    VkDeviceSize uniformBufferSize = bufferView->getLength();

    VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

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
        uniformBuffer,
        {
            .bufferHandler = bufferHandler,
            .bufferMemory = bufferMemory,
        } );

    return ManagerImpl::bind( uniformBuffer );
}

crimild::Bool UniformBufferManager::unbind( UniformBuffer *uniformBuffer ) noexcept
{
    if ( !validate( uniformBuffer ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbinding Vulkan uniformBuffer" );

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return false;
    }

    auto bindInfo = getBindInfo( uniformBuffer );
    vkDestroyBuffer( renderDevice->handler, bindInfo.bufferHandler, nullptr );
    vkFreeMemory( renderDevice->handler, bindInfo.bufferMemory, nullptr );

    removeBindInfo( uniformBuffer );

    return ManagerImpl::unbind( uniformBuffer );
}

void UniformBufferManager::updateUniformBuffers( crimild::Size index ) noexcept
{
    // This is a slow operation. It's copying data synchronously and also it's assuming
    // all UBOs need updating. Instead, we could use BufferView::Usage to find out which
    // ubos actually need updating.
    auto renderDevice = getRenderDevice();
    each( [ this, renderDevice ]( UniformBuffer *ubo, UniformBufferBindInfo &bindInfo ) {
        ubo->onPreRender();
        utils::copyToBuffer(
            renderDevice->handler,
            bindInfo.bufferMemory,
            ubo->getBufferView()->getData(),
            ubo->getBufferView()->getLength() );
    } );
}
