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
*     * Neither the name of the copyright holder nor the
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

#include "Rendering/CommandBuffer.hpp"

using namespace crimild;

CommandBuffer::Command::Command( const Command &other ) noexcept
    : type( other.type )
{
    switch ( type ) {
        case Command::Type::BEGIN:
            usage = other.usage;
            break;

        case Command::Type::BEGIN_RENDER_PASS:
            renderPass = other.renderPass;
            break;

        case Command::Type::SET_VIEWPORT:
            viewport = other.viewport;
            break;

        case Command::Type::BIND_GRAPHICS_PIPELINE:
            pipeline = other.pipeline;
            break;

        case Command::Type::BIND_PRIMITIVE:
            primitive = other.primitive;
            break;

        case Command::Type::BIND_INDEX_BUFFER:
            buffer = other.buffer;
            break;

        case Command::Type::BIND_UNIFORM_BUFFER:
            uniformBuffer = other.uniformBuffer;
            break;

        case Command::Type::BIND_VERTEX_BUFFER:
            buffer = other.buffer;
            break;

        case Command::Type::BIND_COMMAND_BUFFER:
            commandBuffer = other.commandBuffer;
            break;

        case Command::Type::BIND_DESCRIPTOR_SET:
            descriptorSet = other.descriptorSet;
            break;

        case Command::Type::DRAW:
            count = other.count;
            break;

        case Command::Type::DRAW_INDEXED:
            count = other.count;
            break;

        case Command::Type::END_RENDER_PASS:
            renderPass = other.renderPass;
            break;

        case Command::Type::END:
            break;

        default:
            break;
    }
}

CommandBuffer::Command::~Command( void ) noexcept
{
    switch ( type ) {
        case Command::Type::SET_VIEWPORT:
            viewport.~ViewportDimensions();
            break;
        case Command::Type::BIND_GRAPHICS_PIPELINE:
            pipeline = nullptr;
            break;

        default:
            break;
    }
}

void CommandBuffer::begin( CommandBuffer::Usage usage ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BEGIN,
    cmd.usage = usage;
    m_commands.push_back( cmd );
}

void CommandBuffer::beginRenderPass( RenderPass *renderPass ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BEGIN_RENDER_PASS;
    cmd.renderPass = renderPass;
    m_commands.push_back( cmd );
}

void CommandBuffer::setViewport( const ViewportDimensions &viewport ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::SET_VIEWPORT;
    cmd.viewport = viewport;
    m_commands.push_back( cmd );
}

void CommandBuffer::bindGraphicsPipeline( Pipeline *pipeline ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_GRAPHICS_PIPELINE;
    cmd.pipeline = pipeline;
    m_commands.push_back( cmd );
}

void CommandBuffer::bindPrimitive( Primitive *primitive ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_PRIMITIVE;
    cmd.primitive = primitive;
    m_commands.push_back( cmd );
}

void CommandBuffer::bindIndexBuffer( Buffer *indexBuffer ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_INDEX_BUFFER;
    cmd.buffer = indexBuffer;
    m_commands.push_back( cmd );
}

void CommandBuffer::bindVertexBuffer( VertexBuffer *vertexBuffer ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_VERTEX_BUFFER;
    cmd.vertexBuffer = vertexBuffer;
    m_commands.push_back( cmd );
}

void CommandBuffer::bindUniformBuffer( UniformBuffer *uniformBuffer ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_UNIFORM_BUFFER,
    cmd.uniformBuffer = uniformBuffer,
    m_commands.push_back( cmd );
}

void CommandBuffer::bindDescriptorSet( DescriptorSet *descriptorSet ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_DESCRIPTOR_SET,
    cmd.descriptorSet = descriptorSet,
    m_commands.push_back( cmd );
}

void CommandBuffer::bindCommandBuffer( CommandBuffer *commandBuffer ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_COMMAND_BUFFER;
    cmd.commandBuffer = commandBuffer;
    m_commands.push_back( cmd );
}

void CommandBuffer::drawIndexed( crimild::UInt32 count ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::DRAW_INDEXED;
    cmd.count = count;
    m_commands.push_back( cmd );
}

void CommandBuffer::endRenderPass( RenderPass *renderPass ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::END_RENDER_PASS,
    cmd.renderPass = renderPass,
    m_commands.push_back( cmd );
}

void CommandBuffer::end( void ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::END,
    m_commands.push_back( cmd );
}

