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

#ifndef CRIMILD_RENDERING_COMMAND_BUFFER_
#define CRIMILD_RENDERING_COMMAND_BUFFER_

#include "Rendering/RenderResource.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/Types.hpp"

namespace crimild {

    class Buffer;
    class Pipeline;
    class Primitive;
    class RenderPass;
    class UniformBuffer;
    class DescriptorSet;

    class CommandBuffer :
    	public SharedObject,
    	public RenderResourceImpl< CommandBuffer > {
    public:
        enum class Usage {
            DEFAULT,
            ONE_TIME_SUBMIT,
            RENDER_PASS_CONTINUE,
            SIMULTANEOUS_USE,
        };

        struct Command {
            enum class Type {
				BEGIN,
                BEGIN_RENDER_PASS,
                BIND_GRAPHICS_PIPELINE,
                BIND_PRIMITIVE,
                BIND_INDEX_BUFFER,
                BIND_UNIFORM_BUFFER,
                BIND_VERTEX_BUFFER,
                BIND_COMMAND_BUFFER,
                BIND_DESCRIPTOR_SET,
                DRAW,
                DRAW_INDEXED,
                END_RENDER_PASS,
                END,
            };

            Type type;

            union {
                Usage usage;
                CommandBuffer *commandBuffer;
                Pipeline *pipeline;
                Primitive *primitive;
                RenderPass *renderPass;
                Buffer *buffer;
                UniformBuffer *uniformBuffer;
                DescriptorSet *descriptorSet;
                crimild::UInt32 count;
            };
        };

    public:
        void begin( Usage usage ) noexcept;
        void beginRenderPass( RenderPass *renderPass ) noexcept;
        void bindCommandBuffer( CommandBuffer *commandBuffer ) noexcept;
        void bindGraphicsPipeline( Pipeline *pipeline ) noexcept;
        void bindPrimitive( Primitive *primitive ) noexcept;
        void bindIndexBuffer( Buffer *indexBuffer ) noexcept;
        void bindVertexBuffer( Buffer *vertexBuffer ) noexcept;
        void bindUniformBuffer( UniformBuffer *uniformBuffer ) noexcept;
        void bindDescriptorSet( DescriptorSet *descriptorSet ) noexcept;
        void drawIndexed( crimild::UInt32 count ) noexcept;
        void endRenderPass( RenderPass *renderPass ) noexcept;
        void end( void ) noexcept;

        void clear( void ) noexcept;

        template< typename CallbackType >
        void each( CallbackType &&callback ) noexcept {
            for ( auto &cmd : m_commands ) {
                callback( cmd );
            }
        }

    private:
        std::vector< Command > m_commands;
    };

}

#endif

