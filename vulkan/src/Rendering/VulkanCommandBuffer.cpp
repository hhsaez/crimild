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

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanRenderDevice.hpp"
#include "VulkanRenderPass.hpp"

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
        return false;
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

    Array< VkCommandBuffer > handlers( imageCount );
    CRIMILD_VULKAN_CHECK(
        vkAllocateCommandBuffers(
            renderDevice->handler,
            &allocInfo,
            &handlers[ 0 ] ) );

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
            &handler );
    } );
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
        [ & ]( CommandBuffer::Command &cmd ) {
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
                            &beginInfo ) );
                    break;
                }

                case CommandBuffer::Command::Type::BEGIN_RENDER_PASS: {
                    m_currentRenderPass = crimild::cast_ptr< RenderPass >( cmd.obj );
                    renderDevice->setCurrentRenderPass( crimild::get_ptr( m_currentRenderPass ) );

                    // Bind renderpass first
                    auto bindInfo = renderDevice->getBindInfo( crimild::get_ptr( m_currentRenderPass ) );
                    auto renderPass = bindInfo.handler;
                    auto framebuffer = bindInfo.framebuffers[ index ];

                    auto clearColor = m_currentRenderPass->clearValue.color;
                    auto clearDepth = m_currentRenderPass->clearValue.depthStencil;
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
                        .renderArea = {
                            // Render passes always render to the full extent of
                            // the associated framebuffer
                            .offset = { 0, 0 },
                            .extent = utils::getExtent(
                                m_currentRenderPass->extent,
                                renderDevice ),
                        },
                        .clearValueCount = static_cast< crimild::UInt32 >( clearValues.size() ),
                        .pClearValues = clearValues.data(),
                    };

                    vkCmdBeginRenderPass( handler, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
                    break;
                }

                case CommandBuffer::Command::Type::SET_VIEWPORT: {
                    auto viewport = [ & ] {
                        auto viewport = cmd.viewportDimensions;
                        if ( viewport.scalingMode == ScalingMode::RELATIVE ) {
                            viewport.scalingMode = m_currentRenderPass->extent.scalingMode;
                            auto w = m_currentRenderPass->extent.width;
                            auto h = m_currentRenderPass->extent.height;
                            viewport.dimensions = Rectf(
                                viewport.dimensions.getX() * w,
                                viewport.dimensions.getY() * h,
                                viewport.dimensions.getWidth() * w,
                                viewport.dimensions.getHeight() * h );
                        }
                        return utils::getViewport( &viewport, renderDevice );
                    }();
                    vkCmdSetViewport( handler, 0, 1, &viewport );
                    break;
                }

                case CommandBuffer::Command::Type::SET_SCISSOR: {
                    auto scissor = [ & ] {
                        auto viewport = cmd.viewportDimensions;
                        if ( viewport.scalingMode == ScalingMode::RELATIVE ) {
                            viewport.scalingMode = m_currentRenderPass->extent.scalingMode;
                            auto w = m_currentRenderPass->extent.width;
                            auto h = m_currentRenderPass->extent.height;
                            viewport.dimensions = Rectf(
                                viewport.dimensions.getX() * w,
                                viewport.dimensions.getY() * h,
                                viewport.dimensions.getWidth() * w,
                                viewport.dimensions.getHeight() * h );
                        }
                        return utils::getScissor( &viewport, renderDevice );
                    }();
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
                    auto pipeline = cmd.get< GraphicsPipeline >();
                    auto pipelineBindInfo = renderDevice->getBindInfo( pipeline );

                    m_currentGraphicsPipeline = pipeline;
                    m_boundDescriptorSets = 0;

                    vkCmdBindPipeline(
                        handler,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        pipelineBindInfo.pipelineHandler );
                    break;
                }

                case CommandBuffer::Command::Type::BIND_COMPUTE_PIPELINE: {
                    auto pipeline = cmd.get< ComputePipeline >();
                    auto pipelineBindInfo = renderDevice->getBindInfo( pipeline );

                    m_currentComputePipeline = pipeline;
                    m_boundDescriptorSets = 0;

                    vkCmdBindPipeline(
                        handler,
                        VK_PIPELINE_BIND_POINT_COMPUTE,
                        pipelineBindInfo.pipelineHandler );
                    break;
                }

                case CommandBuffer::Command::Type::BIND_VERTEX_BUFFER: {
                    auto vbo = crimild::cast_ptr< VertexBuffer >( cmd.obj );
                    auto bindInfo = renderDevice->getBindInfo( crimild::get_ptr( vbo ) );

                    // reset vertex offset
                    m_vertexOffset = 0;

                    VkBuffer vertexBuffers[] = {
                        bindInfo.bufferHandler,
                    };
                    VkDeviceSize offsets[] = {
                        0
                    };
                    vkCmdBindVertexBuffers(
                        handler,
                        0,
                        1,
                        vertexBuffers,
                        offsets );
                    break;
                }

                case CommandBuffer::Command::Type::BIND_INDEX_BUFFER: {
                    auto ibo = crimild::cast_ptr< IndexBuffer >( cmd.obj );
                    auto bindInfo = renderDevice->getBindInfo( crimild::get_ptr( ibo ) );

                    // reset index offset
                    m_indexOffset = 0;

                    vkCmdBindIndexBuffer(
                        handler,
                        bindInfo.bufferHandler,
                        0,
                        utils::getIndexType( crimild::get_ptr( ibo ) ) );
                    break;
                }

                case CommandBuffer::Command::Type::BIND_DESCRIPTOR_SET: {
                    PipelineBase *pipeline = nullptr;
                    PipelineLayout *pipelineLayout = nullptr;
                    if ( m_currentGraphicsPipeline != nullptr ) {
                        pipeline = m_currentGraphicsPipeline;
                        pipelineLayout = crimild::get_ptr( renderDevice->getBindInfo( m_currentGraphicsPipeline ).pipelineLayout );
                    } else if ( m_currentComputePipeline != nullptr ) {
                        pipeline = m_currentComputePipeline;
                        pipelineLayout = crimild::get_ptr( renderDevice->getBindInfo( m_currentComputePipeline ).pipelineLayout );
                    }

                    auto descriptorSet = crimild::cast_ptr< DescriptorSet >( cmd.obj );
                    if ( descriptorSet->layout == nullptr ) {
                        // Get a layout corresponding to the same binding point
                        // If there's a mismatch, Vulkan validation layers will complain about it
                        // TODO: Maybe we can do this in an early stage, like the frame graph?
                        descriptorSet->layout = pipeline->getProgram()->descriptorSetLayouts[ m_boundDescriptorSets ];
                    }
                    auto descriptorSetHandler = renderDevice->getHandler( crimild::get_ptr( descriptorSet ) );

                    VkDescriptorSet descriptorSets[] = {
                        descriptorSetHandler,
                    };

                    vkCmdBindDescriptorSets(
                        handler,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        pipelineLayout->handler,
                        m_boundDescriptorSets++,
                        1,
                        descriptorSets,
                        0,
                        nullptr );
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

                case CommandBuffer::Command::Type::DISPATCH: {
                    auto workgroup = cmd.workgroup;
                    vkCmdDispatch( handler, workgroup.x, workgroup.y, workgroup.z );
                    break;
                }

                case CommandBuffer::Command::Type::END: {
                    CRIMILD_VULKAN_CHECK(
                        vkEndCommandBuffer(
                            handler ) );
                    break;
                }

                default:
                    CRIMILD_LOG_DEBUG( "Ignoring command of type ", static_cast< uint32_t >( cmd.type ) );
                    break;
            }
        } );
}
