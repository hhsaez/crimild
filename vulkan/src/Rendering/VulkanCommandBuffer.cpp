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

using namespace crimild;
using namespace crimild::vulkan;

CommandBuffer::~CommandBuffer( void ) noexcept
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

void CommandBuffer::begin( Usage usage ) const noexcept
{
	auto beginInfo = VkCommandBufferBeginInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = vulkan::utils::VULKAN_COMMAND_BUFFER_USAGE[ static_cast< uint32_t >( usage ) ],
		.pInheritanceInfo = nullptr, // optional
	};

    CRIMILD_VULKAN_CHECK(
     	vkBeginCommandBuffer(
     		handler,
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
	};
	
	auto renderPassInfo = VkRenderPassBeginInfo {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass->handler,
		.framebuffer = framebuffer->handler,
		.renderArea.offset = { 0, 0 },
		.renderArea.extent = framebuffer->extent,
		.clearValueCount = 1,
		.pClearValues = &clearValue,
	};

	vkCmdBeginRenderPass( handler, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
}

void CommandBuffer::bindGraphicsPipeline( const Pipeline *pipeline ) const noexcept
{
	vkCmdBindPipeline(
		handler,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline->handler
	);
}

void CommandBuffer::draw( void ) const noexcept
{
	vkCmdDraw( handler, 3, 1, 0, 0 );
}

void CommandBuffer::endRenderPass( void ) const noexcept
{
	vkCmdEndRenderPass( handler );
}

void CommandBuffer::end( void ) const
{
    CRIMILD_VULKAN_CHECK(
     	vkEndCommandBuffer(
       		handler
        )
    );
}

SharedPointer< CommandBuffer > CommandBufferManager::create( CommandBuffer::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan Command Buffer" );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    auto allocInfo = VkCommandBufferAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = descriptor.commandPool->handler,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBufferHandler;
    CRIMILD_VULKAN_CHECK(
 		vkAllocateCommandBuffers(
            renderDevice->handler,
            &allocInfo,
            &commandBufferHandler
        )
 	);

    auto commandBuffer = crimild::alloc< CommandBuffer >();
    commandBuffer->handler = commandBufferHandler;
    commandBuffer->renderDevice = renderDevice;
    commandBuffer->commandPool = descriptor.commandPool;
    commandBuffer->manager = this;
    insert( crimild::get_ptr( commandBuffer ) );
    return commandBuffer;
}

void CommandBufferManager::destroy( CommandBuffer *commandBuffer ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan commandBuffer" );

    if ( commandBuffer->renderDevice != nullptr && commandBuffer->handler != VK_NULL_HANDLE ) {
        vkFreeCommandBuffers(
            commandBuffer->renderDevice->handler,
            commandBuffer->commandPool->handler,
            1,
            &commandBuffer->handler
        );
    };

    commandBuffer->handler = nullptr;
    commandBuffer->renderDevice = nullptr;
    commandBuffer->commandPool = nullptr;
    commandBuffer->manager = nullptr;
    erase( commandBuffer );
}
