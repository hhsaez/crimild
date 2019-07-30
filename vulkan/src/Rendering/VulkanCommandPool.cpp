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

using namespace crimild::vulkan;

CommandPool::CommandPool( const VulkanRenderDevice *device, crimild::UInt32 queueFamilyIndex )
	: m_renderDevice( device )
{
	CRIMILD_LOG_TRACE( "Creating Vulkan Command Pool" );

	auto createInfo = VkCommandPoolCreateInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = queueFamilyIndex,
		.flags = 0,
	};

	if ( vkCreateCommandPool( m_renderDevice->getDeviceHandler(), &createInfo, nullptr, &m_commandPoolHandler ) != VK_SUCCESS ) {
		throw VulkanException( "Failed to create command pool" );
	}
}

CommandPool::~CommandPool( void )
{
	CRIMILD_LOG_TRACE( "Destroying Vulkan Command Pool" );
	
	if ( m_renderDevice != nullptr && m_commandPoolHandler != VK_NULL_HANDLE ) {
		vkDestroyCommandPool(
			m_renderDevice->getDeviceHandler(),
			m_commandPoolHandler,
			nullptr
		);
	}

	m_renderDevice = nullptr;
	m_commandPoolHandler = VK_NULL_HANDLE;
}

crimild::SharedPointer< CommandBuffer > CommandPool::createCommandBuffer( void ) const
{
	return crimild::alloc< CommandBuffer >(
		m_renderDevice,
		CommandBuffer::Descriptor {
			.commandPool = this,
		}
	);
}

