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
#include "VulkanGraphicsPipelineOLD.hpp"
#include "VulkanRenderDeviceOLD.hpp"
#include "VulkanRenderPass.hpp"

using namespace crimild;
using namespace crimild::vulkan;

crimild::Bool CommandBufferManager::bind( CommandBuffer *commandBuffer ) noexcept
{
    if ( validate( commandBuffer ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE();

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        return false;
    }

    auto commandPool = renderDevice->getCommandPool();

    auto allocInfo = VkCommandBufferAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool->handler,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer handler;
    CRIMILD_VULKAN_CHECK(
        vkAllocateCommandBuffers(
            renderDevice->handler,
            &allocInfo,
            &handler ) );

    if ( !commandBuffer->getName().empty() ) {
        utils::setObjectName(
            renderDevice->handler,
            UInt64( handler ),
            VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT,
            commandBuffer->getName().c_str() );
    }

    setBindInfo( commandBuffer, CommandBufferBindInfo { .handler = handler } );
    recordCommands( renderDevice, nullptr, commandBuffer );

    return ManagerImpl::bind( commandBuffer );
}

crimild::Bool CommandBufferManager::unbind( CommandBuffer *commandBuffer ) noexcept
{
    if ( !validate( commandBuffer ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE();

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

    auto bindInfo = getBindInfo( commandBuffer );
    auto handler = bindInfo.handler;
    vkFreeCommandBuffers(
        renderDevice->handler,
        commandPool->handler,
        1,
        &handler );

    removeBindInfo( commandBuffer );

    return ManagerImpl::unbind( commandBuffer );
}

void CommandBufferManager::recordCommands( RenderDeviceOLD *renderDevice, CommandBuffer *parent, CommandBuffer *commandBuffer ) noexcept
{
    auto handler = getBindInfo( commandBuffer ).handler;
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
                        .flags = static_cast< VkCommandBufferUsageFlags >( vulkan::utils::VULKAN_COMMAND_BUFFER_USAGE[ static_cast< uint32_t >( cmd.usage ) ] ),
                        .pInheritanceInfo = nullptr, // optional
                    };

                    CRIMILD_VULKAN_CHECK(
                        vkBeginCommandBuffer(
                            handler,
                            &beginInfo ) );
                    break;
                }

                case CommandBuffer::Command::Type::RESET: {
                    CRIMILD_VULKAN_CHECK(
                        vkResetCommandBuffer(
                            handler,
                            VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT ) );
                    break;
                }

                case CommandBuffer::Command::Type::BEGIN_RENDER_PASS: {
                    m_currentRenderPass = crimild::cast_ptr< RenderPass >( cmd.obj );
                    renderDevice->setCurrentRenderPass( crimild::get_ptr( m_currentRenderPass ) );

                    // Bind renderpass first
                    auto bindInfo = renderDevice->getBindInfo( crimild::get_ptr( m_currentRenderPass ) );
                    auto renderPass = bindInfo.handler;
                    auto framebuffer = bindInfo.framebuffers[ commandBuffer->getFrameIndex() ];

                    m_currentRenderPass->eachRead(
                        [ & ]( auto resource ) {
                            if ( resource == nullptr ) {
                                return;
                            }

                            switch ( resource->getType() ) {
                                case FrameGraphResource::Type::IMAGE_VIEW: {
                                    break;
                                }
                                case FrameGraphResource::Type::IMAGE: {
                                    break;
                                }
                                case FrameGraphResource::Type::ATTACHMENT: {
                                    break;
                                }
                                case FrameGraphResource::Type::TEXTURE: {
                                    auto texture = crimild::cast_ptr< Texture >( resource );
                                    auto image = texture->imageView->image;
                                    auto imageBindInfo = renderDevice->getBindInfo( crimild::get_ptr( image ) );

                                    auto needsBarrier = false;

                                    texture->eachWrittenBy( [ & ]( auto op ) {
                                        if ( op->getType() == FrameGraphOperation::Type::COMPUTE_PASS ) {
                                            needsBarrier = true;
                                        }
                                    } );

                                    if ( needsBarrier ) {
                                        auto imageMemoryBarrier = VkImageMemoryBarrier {
                                            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                            .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
                                            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                                            .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
                                            .newLayout = VK_IMAGE_LAYOUT_GENERAL,
                                            .image = imageBindInfo.imageHandler,
                                            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
                                        };
                                        vkCmdPipelineBarrier(
                                            handler,
                                            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                            0,
                                            0,
                                            nullptr,
                                            0,
                                            nullptr,
                                            1,
                                            &imageMemoryBarrier );
                                    }

                                    break;
                                }
                                default: {
                                    break;
                                }
                            }
                        } );

                    auto clearColor = m_currentRenderPass->clearValue.color;
                    auto clearDepth = m_currentRenderPass->clearValue.depthStencil;
                    auto clearValues = std::vector< VkClearValue > {};
                    m_currentRenderPass->attachments.each(
                        [ & ]( auto attachment ) {
                            auto format = attachment->format;
                            if ( utils::formatIsDepthStencil( format ) ) {
                                clearValues.push_back(
                                    {
                                        .depthStencil = {
                                            clearDepth.x,
                                            static_cast< crimild::UInt32 >( clearDepth.y ),
                                        },
                                    } );
                            } else {
                                clearValues.push_back(
                                    {
                                        .color = {
                                            .float32 = {
                                                clearColor.r,
                                                clearColor.g,
                                                clearColor.b,
                                                clearColor.a,
                                            },
                                        },
                                    } );
                            }
                        } );

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
                            viewport.dimensions = Rectf {
                                viewport.dimensions.origin.x * w,
                                viewport.dimensions.origin.y * h,
                                viewport.dimensions.size.width * w,
                                viewport.dimensions.size.height * h,
                            };
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
                            viewport.dimensions = Rectf {
                                viewport.dimensions.origin.x * w,
                                viewport.dimensions.origin.y * h,
                                viewport.dimensions.size.width * w,
                                viewport.dimensions.size.height * h,
                            };
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

                case CommandBuffer::Command::Type::BIND_COMMAND_BUFFER: {
                    if ( auto child = cmd.commandBuffer ) {
                        recordCommands( renderDevice, parent != nullptr ? parent : commandBuffer, child );
                    }
                    break;
                }

                case CommandBuffer::Command::Type::BIND_GRAPHICS_PIPELINE: {
                    auto pipeline = cmd.get< GraphicsPipeline >();
                    auto pipelineBindInfo = renderDevice->getBindInfo( pipeline );

                    m_currentGraphicsPipeline = pipeline;
                    m_currentComputePipeline = nullptr;
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

                    m_currentGraphicsPipeline = nullptr;
                    m_currentComputePipeline = pipeline;
                    m_boundDescriptorSets = 0;

                    vkCmdBindPipeline(
                        handler,
                        VK_PIPELINE_BIND_POINT_COMPUTE,
                        pipelineBindInfo.pipelineHandler );
                    break;
                }

                case CommandBuffer::Command::Type::BIND_VERTEX_BUFFER: {
                    auto &info = cmd.bindVertexBufferInfo;
                    auto vbo = info.vertexBuffer;
                    auto index = info.index;
                    auto bindInfo = renderDevice->getBindInfo( crimild::get_ptr( vbo ) );

                    VkBuffer vertexBuffers[] = {
                        bindInfo.bufferHandler,
                    };
                    VkDeviceSize offsets[] = {
                        0
                    };
                    vkCmdBindVertexBuffers(
                        handler,
                        index,
                        1,
                        vertexBuffers,
                        offsets );
                    break;
                }

                case CommandBuffer::Command::Type::BIND_INDEX_BUFFER: {
                    auto ibo = crimild::cast_ptr< IndexBuffer >( cmd.obj );
                    auto bindInfo = renderDevice->getBindInfo( crimild::get_ptr( ibo ) );

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
                    // auto count = cmd.count;
                    const auto &info = cmd.drawIndexedInfo;
                    vkCmdDrawIndexed( handler, info.indexCount, info.instanceCount, info.firstIndex, info.vertexOffset, info.firstInstance );
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

void CommandBufferManager::updateCommandBuffer( CommandBuffer *commandBuffer ) noexcept
{
    if ( !commandBuffer->cleared() ) {
        return;
    }

    auto renderDevice = getRenderDevice();

    recordCommands( renderDevice, nullptr, commandBuffer );

    commandBuffer->resetCleared();
}
