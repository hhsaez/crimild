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

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/RenderPass.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool CommandBufferManager::bind( CommandBuffer *commandBuffer ) noexcept
{
    if ( validate( commandBuffer ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan Command Buffer" );

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        return nullptr;
    }

    auto commandPool = renderDevice->getCommandPool();
    auto swapchain = renderDevice->getSwapchain();
    auto imageCount = swapchain->images.size();

    auto allocInfo = VkCommandBufferAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = commandPool->handler,
        .commandBufferCount = static_cast< crimild::UInt32 >( imageCount ),
    };

    containers::Array< VkCommandBuffer > handlers( imageCount );
    CRIMILD_VULKAN_CHECK(
         vkAllocateCommandBuffers(
            renderDevice->handler,
            &allocInfo,
            &handlers[ 0 ]
        )
    );

    setHandlers( commandBuffer, handlers );

    for ( auto i = 0l; i < imageCount; i++ ) {
        recordCommands( renderDevice, nullptr, commandBuffer, i );
    }

    return ManagerImpl::bind( commandBuffer );
}

crimild::Bool CommandBufferManager::unbind( CommandBuffer *commandBuffer ) noexcept
{
    if ( !validate( commandBuffer ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbinding Vulkan commandBuffer" );

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid render device instance" );
        return false;
    }

    auto commandPool = renderDevice->getCommandPool();
    if ( commandPool == nullptr ) {
        CRIMILD_LOG_ERROR( "No valid command pool instance" );
        return false;
    }

    renderDevice->waitIdle();

    eachHandler( commandBuffer, [ renderDevice, commandPool ]( VkCommandBuffer handler ) {
        vkFreeCommandBuffers(
            renderDevice->handler,
            commandPool->handler,
            1,
            &handler
        );
    });
    removeHandlers( commandBuffer );

    return ManagerImpl::unbind( commandBuffer );
}

void CommandBufferManager::recordCommands( RenderDevice *renderDevice, CommandBuffer *parent, CommandBuffer *commandBuffer, crimild::Size index ) noexcept
{
    auto handler = renderDevice->getHandler( parent != nullptr ? parent : commandBuffer, index );
    if ( handler == VK_NULL_HANDLE ) {
        CRIMILD_LOG_ERROR( "Invalid command buffer" );
        return;
    }

    commandBuffer->each(
        [&]( CommandBuffer::Command &cmd ) {
        	switch ( cmd.type ) {
                case CommandBuffer::Command::Type::BEGIN: {
                    auto beginInfo = VkCommandBufferBeginInfo {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                        .flags = vulkan::utils::VULKAN_COMMAND_BUFFER_USAGE[ static_cast< uint32_t >( cmd.usage ) ],
                        .pInheritanceInfo = nullptr, // optional
                    };

                    CRIMILD_VULKAN_CHECK(
                        vkBeginCommandBuffer(
                            handler,
                            &beginInfo
                        )
                    );
                    break;
                }

                case CommandBuffer::Command::Type::BEGIN_RENDER_PASS: {
                    m_currentRenderPass = crimild::cast_ptr< RenderPass >( cmd.obj );
                    renderDevice->setCurrentRenderPass( crimild::get_ptr( m_currentRenderPass ) );

                    // Bind renderpass first
                    auto renderPass = renderDevice->getBindInfo( crimild::get_ptr( m_currentRenderPass ) );
                    auto framebuffer = renderDevice->getHandler( crimild::get_ptr( m_currentFramebuffer ), index );

                    auto clearColor = RGBAColorf::ZERO;
                    auto clearDepth = Vector2f( 1, 0 );
                    auto clearValues = std::vector< VkClearValue > {
                        {
                            .color = {
                                .float32 = {
                                	clearColor.r(),
                                	clearColor.g(),
                                	clearColor.b(),
                                	clearColor.a(),
                                },
                            },
                        },
                        {
                            .depthStencil = {
                                clearDepth.r(),
                                static_cast< crimild::UInt32 >( clearDepth.g() ),
                            },
                        },
                    };

                    auto renderPassInfo = VkRenderPassBeginInfo {
                        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                        .renderPass = renderPass,
                        .framebuffer = framebuffer,
                        .renderArea.offset = { 0, 0 },
                        .renderArea.extent = utils::getExtent( m_currentFramebuffer->extent, renderDevice ),
                        .clearValueCount = static_cast< crimild::UInt32 >( clearValues.size() ),
                        .pClearValues = clearValues.data(),
                    };

                    vkCmdBeginRenderPass( handler, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
                    break;
                }

                case CommandBuffer::Command::Type::SET_VIEWPORT: {
                    auto viewport = utils::getViewport( &cmd.viewportDimensions, renderDevice );
                    vkCmdSetViewport( handler, 0, 1, &viewport );
                    break;
                }

                case CommandBuffer::Command::Type::SET_SCISSOR: {
                    auto scissor = utils::getScissor( &cmd.viewportDimensions, renderDevice );
                    vkCmdSetScissor( handler, 0, 1, &scissor );
                    break;
                }

                case CommandBuffer::Command::Type::SET_FRAMEBUFFER: {
                    m_currentFramebuffer = crimild::cast_ptr< Framebuffer >( cmd.obj );
                    break;
                }

                case CommandBuffer::Command::Type::SET_INDEX_OFFSET: {
                    m_indexOffset = cmd.size;
                    break;
                }

                case CommandBuffer::Command::Type::SET_VERTEX_OFFSET: {
                    m_vertexOffset = cmd.size;
                    break;
                }

                case CommandBuffer::Command::Type::BIND_COMMAND_BUFFER: {
                    if ( auto child = cmd.commandBuffer ) {
                        recordCommands( renderDevice, parent != nullptr ? parent : commandBuffer, child, index );
                    }
                    break;
                }

                case CommandBuffer::Command::Type::BIND_GRAPHICS_PIPELINE: {
                    auto pipeline = cmd.pipeline;
                    auto pipelineBindInfo = renderDevice->getBindInfo( pipeline );

                    m_currentPipeline = pipeline;

                    vkCmdBindPipeline(
                        handler,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        pipelineBindInfo.pipelineHandler
                    );
                    break;
                }

                case CommandBuffer::Command::Type::BIND_VERTEX_BUFFER: {
                    auto vbo = crimild::cast_ptr< VertexBuffer >( cmd.obj );
                    auto bindInfo = renderDevice->getBindInfo( crimild::get_ptr( vbo ) );

                    // reset vertex offset
                    m_vertexOffset = 0;

                    VkBuffer vertexBuffers[] = {
                        bindInfo.bufferHandlers[ 0 ],
                    };
                    VkDeviceSize offsets[] = {
                        0
                    };
                    vkCmdBindVertexBuffers(
                        handler,
                        0,
                        1,
                        vertexBuffers,
                        offsets
                    );
                    break;
                }

                case CommandBuffer::Command::Type::BIND_INDEX_BUFFER: {
                    auto ibo = crimild::cast_ptr< IndexBuffer >( cmd.obj );
                    auto bindInfo = renderDevice->getBindInfo( crimild::get_ptr( ibo ) );

                    // reset index offset
                    m_indexOffset = 0;

                    vkCmdBindIndexBuffer(
                        handler,
                        bindInfo.bufferHandlers[ 0 ],
                        0,
                        utils::getIndexType( crimild::get_ptr( ibo ) )
                    );
                    break;
                }

                case CommandBuffer::Command::Type::BIND_DESCRIPTOR_SET: {
                    auto descriptorSet = crimild::cast_ptr< DescriptorSet >( cmd.obj );
                    auto descriptorSetHandler = renderDevice->getHandler( crimild::get_ptr( descriptorSet ), index );

                    auto pipeline = m_currentPipeline;
                    auto pipelineBindInfo = renderDevice->getBindInfo( pipeline );

                    VkDescriptorSet descriptorSets[] = {
                        descriptorSetHandler,
                    };

                    vkCmdBindDescriptorSets(
                        handler,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        pipelineBindInfo.pipelineLayout->handler,
                        0,
                        1,
                        descriptorSets,
                        0,
                        nullptr
                    );
                    break;
                }

                case CommandBuffer::Command::Type::DRAW: {
                    auto count = cmd.count;
                    vkCmdDraw( handler, count, 1, 0, 0 );
                    break;
                }

                case CommandBuffer::Command::Type::DRAW_INDEXED: {
                    auto count = cmd.count;
                    vkCmdDrawIndexed( handler, count, 1, m_indexOffset, m_vertexOffset, 0 );
                    break;
                }

                case CommandBuffer::Command::Type::END_RENDER_PASS: {
                    vkCmdEndRenderPass( handler );
                    renderDevice->setCurrentRenderPass( nullptr );
                    m_currentRenderPass = nullptr;
                    m_currentFramebuffer = nullptr;
                    break;
                }

                case CommandBuffer::Command::Type::END: {
                    CRIMILD_VULKAN_CHECK(
                        vkEndCommandBuffer(
                            handler
                        )
                    );
                    break;
                }

            	default:
                	CRIMILD_LOG_DEBUG( "Ignoring command of type ", static_cast< uint32_t >( cmd.type ) );
                    break;
        	}
    	}
    );
}
