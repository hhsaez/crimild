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
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/VertexBuffer.hpp"

using namespace crimild;

CommandBuffer::Command::Command( void ) noexcept
    : obj { }
{
    
}

CommandBuffer::Command::Command( const Command &other ) noexcept
    : type( other.type ),
	  obj { }
{
    switch ( type ) {
        case Command::Type::BEGIN:
            usage = other.usage;
            break;

        case Command::Type::SET_VIEWPORT:
        case Command::Type::SET_SCISSOR:
            viewportDimensions = other.viewportDimensions;
            break;

        case Command::Type::BIND_GRAPHICS_PIPELINE:
            pipeline = other.pipeline;
            break;

        case Command::Type::BIND_PRIMITIVE:
            primitive = other.primitive;
            break;

        case Command::Type::BIND_UNIFORM_BUFFER:
            uniformBuffer = other.uniformBuffer;
            break;

        case Command::Type::BIND_COMMAND_BUFFER:
            commandBuffer = other.commandBuffer;
            break;

        case Command::Type::DRAW:
            count = other.count;
            break;

        case Command::Type::DRAW_INDEXED:
            count = other.count;
            break;

        default:
            obj = other.obj;
            break;
    }
}

CommandBuffer::Command::~Command( void ) noexcept
{
    switch ( type ) {
        case Command::Type::SET_VIEWPORT:
        case Command::Type::SET_SCISSOR:
            viewportDimensions.~ViewportDimensions();
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

void CommandBuffer::beginRenderPass( RenderPass *renderPass, Framebuffer *framebuffer ) noexcept
{
    m_commands.push_back(
        [&] {
        	Command cmd;
        	cmd.type = Command::Type::SET_FRAMEBUFFER;
        	cmd.obj = crimild::retain( framebuffer );
            return cmd;
        }()
    );
    m_commands.push_back(
		[&] {
            Command cmd;
            cmd.type = Command::Type::BEGIN_RENDER_PASS;
            cmd.obj = crimild::retain( renderPass );
            return cmd;
    	}()
    );
}

void CommandBuffer::endRenderPass( RenderPass *renderPass ) noexcept
{
    m_commands.push_back(
        [&] {
            Command cmd;
            cmd.type = Command::Type::END_RENDER_PASS;
            cmd.obj = crimild::retain( renderPass );
            return cmd;
        }()
    );
}

void CommandBuffer::setViewport( const ViewportDimensions &viewport ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::SET_VIEWPORT;
    cmd.viewportDimensions = viewport;
    m_commands.push_back( cmd );
}

void CommandBuffer::setScissor( const ViewportDimensions &scissor ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::SET_SCISSOR;
    cmd.viewportDimensions = scissor;
    m_commands.push_back( cmd );
}

void CommandBuffer::setIndexOffset( crimild::Size offset ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::SET_INDEX_OFFSET;
    cmd.size = offset;
    m_commands.push_back( cmd );
}

void CommandBuffer::setVertexOffset( crimild::Size offset ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::SET_VERTEX_OFFSET;
    cmd.size = offset;
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

void CommandBuffer::bindIndexBuffer( IndexBuffer *indexBuffer ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_INDEX_BUFFER;
    cmd.obj = crimild::retain( indexBuffer );
    m_commands.push_back( cmd );
}

void CommandBuffer::bindVertexBuffer( VertexBuffer *vertexBuffer ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_VERTEX_BUFFER;
    cmd.obj = crimild::retain( vertexBuffer );
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
    cmd.obj = crimild::retain( descriptorSet ),
    m_commands.push_back( cmd );
}

void CommandBuffer::bindCommandBuffer( CommandBuffer *commandBuffer ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_COMMAND_BUFFER;
    cmd.commandBuffer = commandBuffer;
    m_commands.push_back( cmd );
}

void CommandBuffer::draw( crimild::UInt32 count ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::DRAW;
    cmd.count = count;
    m_commands.push_back( cmd );
}

void CommandBuffer::drawIndexed( crimild::UInt32 count ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::DRAW_INDEXED;
    cmd.count = count;
    m_commands.push_back( cmd );
}

void CommandBuffer::drawIndexed( crimild::UInt32 count, crimild::Size indexOffset, crimild::Size vertexOffset ) noexcept
{
    setIndexOffset( indexOffset );
    setVertexOffset( vertexOffset );

    Command cmd;
    cmd.type = Command::Type::DRAW_INDEXED;
    cmd.count = count;
    m_commands.push_back( cmd );
}

void CommandBuffer::end( void ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::END,
    m_commands.push_back( cmd );
}

void CommandBuffer::clear( void ) noexcept
{
    if ( manager != nullptr ) {
        manager->unbind( this );
    }
    m_commands.clear();
}
