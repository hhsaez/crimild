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

#include "VulkanCommandPool.hpp"
#include "VulkanRenderDevice.hpp"
#include "VulkanCommandBuffer.hpp"
#include "Exceptions/VulkanException.hpp"
#include "Foundation/Log.hpp"

using namespace crimild;
using namespace crimild::vulkan;

CommandPool::~CommandPool( void )
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

//crimild::SharedPointer< CommandBuffer > CommandPool::createCommandBuffer( void ) const
//{
//	return crimild::alloc< CommandBuffer >(
//		m_renderDevice,
//		CommandBuffer::Descriptor {
//			.commandPool = this,
//		}
//	);
//    return nullptr;
//}

SharedPointer< CommandPool > CommandPoolManager::create( CommandPool::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan Command Pool" );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    auto createInfo = VkCommandPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = descriptor.queueFamilyIndex,
        .flags = 0,
    };

    VkCommandPool commandPoolHandler;
    if ( vkCreateCommandPool( renderDevice->handler, &createInfo, nullptr, &commandPoolHandler ) != VK_SUCCESS ) {
        CRIMILD_LOG_ERROR( "Failed to create command pool" );
        return nullptr;
    }

    auto commandPool = crimild::alloc< CommandPool >();
    commandPool->handler = commandPoolHandler;
    commandPool->manager = this;
    commandPool->renderDevice = renderDevice;
    insert( crimild::get_ptr( commandPool ) );
    return commandPool;
}

void CommandPoolManager::destroy( CommandPool *commandPool ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan Command Pool" );

    if ( commandPool->renderDevice != nullptr && commandPool->handler != VK_NULL_HANDLE ) {
        vkDestroyCommandPool(
            commandPool->renderDevice->handler,
            commandPool->handler,
            nullptr
        );
    }

    commandPool->renderDevice = nullptr;
    commandPool->handler = VK_NULL_HANDLE;
    commandPool->manager = nullptr;
    erase( commandPool );
}

