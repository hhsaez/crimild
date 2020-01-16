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

void CommandBuffer::begin( CommandBuffer::Usage usage ) noexcept
{
    m_commands.push_back(
         Command {
            .type = Command::Type::BEGIN,
            .usage = usage,
        }
    );
}

void CommandBuffer::beginRenderPass( RenderPass *renderPass ) noexcept
{
    m_commands.push_back(
         Command {
            .type = Command::Type::BEGIN_RENDER_PASS,
            .renderPass = renderPass,
        }
    );
}

void CommandBuffer::bindGraphicsPipeline( Pipeline *pipeline ) noexcept
{
    m_commands.push_back(
 		Command {
			.type = Command::Type::BIND_GRAPHICS_PIPELINE,
            .pipeline = pipeline,
    	}
	);
}

void CommandBuffer::bindPrimitive( Primitive *primitive ) noexcept
{
	m_commands.push_back(
        Command {
			.type = Command::Type::BIND_PRIMITIVE,
        	.primitive = primitive,
    	}
    );
}

void CommandBuffer::bindIndexBuffer( Buffer *indexBuffer ) noexcept
{
    m_commands.push_back(
        Command {
            .type = Command::Type::BIND_INDEX_BUFFER,
            .buffer = indexBuffer,
        }
    );
}

void CommandBuffer::bindVertexBuffer( VertexBuffer *vertexBuffer ) noexcept
{
    m_commands.push_back(
        Command {
            .type = Command::Type::BIND_VERTEX_BUFFER,
            .vertexBuffer = vertexBuffer,
        }
    );
}

void CommandBuffer::bindUniformBuffer( UniformBuffer *uniformBuffer ) noexcept
{
    m_commands.push_back(
        Command {
            .type = Command::Type::BIND_UNIFORM_BUFFER,
            .uniformBuffer = uniformBuffer,
        }
    );
}

void CommandBuffer::bindDescriptorSet( DescriptorSet *descriptorSet ) noexcept
{
    m_commands.push_back(
    	Command {
        	.type = Command::Type::BIND_DESCRIPTOR_SET,
        	.descriptorSet = descriptorSet,
    	}
    );
}

void CommandBuffer::bindCommandBuffer( CommandBuffer *commandBuffer ) noexcept
{
    m_commands.push_back(
        Command {
            .type = Command::Type::BIND_COMMAND_BUFFER,
            .commandBuffer = commandBuffer,
        }
    );
}

void CommandBuffer::drawIndexed( crimild::UInt32 count ) noexcept
{
    m_commands.push_back(
        Command {
            .type = Command::Type::DRAW_INDEXED,
            .count = count,
        }
    );
}

void CommandBuffer::endRenderPass( RenderPass *renderPass ) noexcept
{
    m_commands.push_back(
         Command {
            .type = Command::Type::END_RENDER_PASS,
            .renderPass = renderPass,
        }
    );
}

void CommandBuffer::end( void ) noexcept
{
    m_commands.push_back(
         Command {
            .type = Command::Type::END,
        }
    );
}

