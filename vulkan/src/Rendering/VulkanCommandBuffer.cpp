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

#include "VulkanCommandBuffer.hpp"
#include "VulkanRenderDevice.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanPipeline.hpp"

using namespace crimild::vulkan;

CommandBuffer::CommandBuffer( const VulkanRenderDevice *device, const Descriptor &descriptor )
	: m_renderDevice( device ),
	  m_commandPool( descriptor.commandPool )
{
	CRIMILD_LOG_TRACE( "Creating Vulkan Command Buffer" );

	auto allocInfo = VkCommandBufferAllocateInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = descriptor.commandPool->getCommandPoolHandler(),
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	CRIMILD_VULKAN_CHECK(
		vkAllocateCommandBuffers(
			m_renderDevice->getDeviceHandler(),
			&allocInfo,
			&m_commandBufferHandler
		)
	);
}

CommandBuffer::~CommandBuffer( void ) noexcept
{
	CRIMILD_LOG_TRACE( "Destroying Vulkan commandBuffer" );

	if ( m_renderDevice != nullptr && m_commandPool != nullptr && m_commandBufferHandler != VK_NULL_HANDLE ) {
		vkFreeCommandBuffers(
			m_renderDevice->getDeviceHandler(),
			m_commandPool->getCommandPoolHandler(),
			1,
			&m_commandBufferHandler
		);

		m_commandBufferHandler = VK_NULL_HANDLE;
	};
}

void CommandBuffer::begin( Usage usage ) const
{
	auto beginInfo = VkCommandBufferBeginInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = vulkan::utils::VULKAN_COMMAND_BUFFER_USAGE[ static_cast< uint32_t >( usage ) ],
		.pInheritanceInfo = nullptr, // optional
	};

	CRIMILD_VULKAN_CHECK(
		vkBeginCommandBuffer(
			m_commandBufferHandler,
			&beginInfo
		)
	);
}

void CommandBuffer::beginRenderPass( const RenderPass *renderPass, const Framebuffer *framebuffer, const RGBAColorf &clearColor ) const noexcept
{
	auto clearValue = VkClearValue {
		clearColor.r(),
		clearColor.g(),
		clearColor.b(),
		clearColor.a(),
	} ;
	
	auto renderPassInfo = VkRenderPassBeginInfo {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass->getRenderPassHandler(),
		.framebuffer = framebuffer->getFramebufferHandler(),
		.renderArea.offset = { 0, 0 },
		.renderArea.extent = framebuffer->getExtent(),
		.clearValueCount = 1,
		.pClearValues = &clearValue,
	};

	vkCmdBeginRenderPass( m_commandBufferHandler, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
}

void CommandBuffer::bindGraphicsPipeline( const Pipeline *pipeline ) const noexcept
{
	vkCmdBindPipeline(
		m_commandBufferHandler,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline->handler
	);
}

void CommandBuffer::draw( void ) const noexcept
{
	vkCmdDraw( m_commandBufferHandler, 3, 1, 0, 0 );
}

void CommandBuffer::endRenderPass( void ) const noexcept
{
	vkCmdEndRenderPass( m_commandBufferHandler );
}

void CommandBuffer::end( void ) const
{
	CRIMILD_VULKAN_CHECK(
		vkEndCommandBuffer(
			m_commandBufferHandler
		)
	);
}

