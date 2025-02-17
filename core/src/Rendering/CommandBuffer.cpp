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

#include "Primitives/BoxPrimitive.hpp"
#include "Primitives/CylinderPrimitive.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/VertexBuffer.hpp"

using namespace crimild;

CommandBuffer::Command::Command( void ) noexcept
    : obj {}
{
}

CommandBuffer::Command::Command( const Command &other ) noexcept
    : type( other.type ),
      obj {}
{
    switch ( type ) {
        case Command::Type::BEGIN:
            usage = other.usage;
            break;

        case Command::Type::SET_VIEWPORT:
        case Command::Type::SET_SCISSOR:
            viewportDimensions = other.viewportDimensions;
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

        case Command::Type::BIND_VERTEX_BUFFER:
            bindVertexBufferInfo = other.bindVertexBufferInfo;
            break;

        case Command::Type::DRAW:
            count = other.count;
            break;

        case Command::Type::DRAW_INDEXED:
            drawIndexedInfo = other.drawIndexedInfo;
            break;

        case Command::Type::DISPATCH:
            workgroup = other.workgroup;
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
        [ & ] {
            Command cmd;
            cmd.type = Command::Type::SET_FRAMEBUFFER;
            cmd.obj = crimild::retain( framebuffer );
            return cmd;
        }() );
    m_commands.push_back(
        [ & ] {
            Command cmd;
            cmd.type = Command::Type::BEGIN_RENDER_PASS;
            cmd.obj = crimild::retain( renderPass );
            return cmd;
        }() );
}

void CommandBuffer::endRenderPass( RenderPass *renderPass ) noexcept
{
    m_commands.push_back(
        [ & ] {
            Command cmd;
            cmd.type = Command::Type::END_RENDER_PASS;
            cmd.obj = crimild::retain( renderPass );
            return cmd;
        }() );
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

void CommandBuffer::bindGraphicsPipeline( GraphicsPipeline *pipeline ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_GRAPHICS_PIPELINE;
    cmd.obj = crimild::retain( pipeline );
    m_commands.push_back( cmd );
}

void CommandBuffer::bindComputePipeline( ComputePipeline *pipeline ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_COMPUTE_PIPELINE;
    cmd.obj = crimild::retain( pipeline );
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

void CommandBuffer::bindVertexBuffer( VertexBuffer *vertexBuffer, UInt32 index ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::BIND_VERTEX_BUFFER;
    cmd.bindVertexBufferInfo = BindVertexBufferInfo {
        .vertexBuffer = crimild::retain( vertexBuffer ),
        .index = index,
    };
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

void CommandBuffer::drawIndexed( const DrawIndexedInfo &info ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::DRAW_INDEXED;
    cmd.drawIndexedInfo = info;
    m_commands.push_back( cmd );
}

void CommandBuffer::drawPrimitive( Primitive *primitive, SharedPointer< VertexBuffer > const &instanceData ) noexcept
{
    switch ( primitive->getType() ) {
        case Primitive::Type::SPHERE: {
            primitive = crimild::get_ptr( SpherePrimitive::UNIT_SPHERE );
            break;
        }

        case Primitive::Type::BOX: {
            primitive = crimild::get_ptr( BoxPrimitive::UNIT_BOX );
            break;
        }

        case Primitive::Type::OPEN_CYLINDER:
        case Primitive::Type::CYLINDER: {
            primitive = crimild::get_ptr( CylinderPrimitive::UNIT_CYLINDER );
            break;
        }

        default:
            break;
    }

    Index vboIndex = 0;
    primitive->getVertexData().each(
        [ & ]( auto &vertices ) {
            if ( vertices != nullptr ) {
                bindVertexBuffer( get_ptr( vertices ), vboIndex++ );
            }
        } );

    UInt32 instanceCount = 1;
    if ( instanceData != nullptr ) {
        instanceCount = UInt32( instanceData->getVertexCount() );
        bindVertexBuffer( get_ptr( instanceData ), vboIndex++ );
    }

    if ( instanceCount == 0 ) {
        CRIMILD_LOG_WARNING( "Instance count must be greater than zero. Primitive will not be rendered" );
        return;
    }

    auto indices = primitive->getIndices();
    if ( indices != nullptr ) {
        bindIndexBuffer( indices );
        drawIndexed(
            DrawIndexedInfo {
                .indexCount = UInt32( indices->getIndexCount() ),
                .instanceCount = instanceCount } );
    } else if ( primitive->getVertexData().size() > 0 ) {
        auto vertices = primitive->getVertexData()[ 0 ];
        if ( vertices != nullptr && vertices->getVertexCount() > 0 ) {
            draw( vertices->getVertexCount() );
        }
    }
}

void CommandBuffer::dispatch( const DispatchWorkgroup &workgroup ) noexcept
{
    Command cmd;
    cmd.type = Command::Type::DISPATCH;
    cmd.workgroup = workgroup;
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
    m_commands.clear();

    m_cleared = true;

    Command cmd;
    cmd.type = Command::Type::RESET;
    m_commands.push_back( cmd );
}
