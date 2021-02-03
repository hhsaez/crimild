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

#include "Foundation/SharedObject.hpp"
#include "Foundation/Types.hpp"
#include "Rendering/RenderResource.hpp"
#include "Rendering/ViewportDimensions.hpp"

namespace crimild {

    class DescriptorSet;
    class Framebuffer;
    class IndexBuffer;
    class GraphicsPipeline;
    class ComputePipeline;
    class Primitive;
    class RenderPass;
    class UniformBuffer;
    class VertexBuffer;

    struct DispatchWorkgroup {
        static constexpr UInt32 DEFAULT_WORGROUP_SIZE = 32;

        UInt32 x;
        UInt32 y;
        UInt32 z;
    };

    class CommandBuffer
        : public SharedObject,
          public RenderResourceImpl< CommandBuffer > {
    public:
        enum class Usage {
            DEFAULT,
            ONE_TIME_SUBMIT,
            RENDER_PASS_CONTINUE,
            SIMULTANEOUS_USE,
        };

        struct Command {
            Command( void ) noexcept;
            Command( const Command &other ) noexcept;
            ~Command( void ) noexcept;

            enum class Type {
                BEGIN,
                RESET,
                BEGIN_RENDER_PASS,
                END_RENDER_PASS,
                BIND_GRAPHICS_PIPELINE,
                BIND_COMPUTE_PIPELINE,
                BIND_PRIMITIVE,
                BIND_INDEX_BUFFER,
                BIND_UNIFORM_BUFFER,
                BIND_VERTEX_BUFFER,
                BIND_COMMAND_BUFFER,
                BIND_DESCRIPTOR_SET,
                DRAW,
                DRAW_INDEXED,
                DISPATCH,
                SET_FRAMEBUFFER,
                SET_INDEX_OFFSET,
                SET_SCISSOR,
                SET_VERTEX_OFFSET,
                SET_VIEWPORT,
                END,
            };

            Type type;

            union {
                Usage usage;
                CommandBuffer *commandBuffer;
                ViewportDimensions viewportDimensions;
                Primitive *primitive;
                UniformBuffer *uniformBuffer;
                SharedPointer< SharedObject > obj;
                crimild::UInt32 count;
                crimild::Size size;
                DispatchWorkgroup workgroup;
            };

            template< typename T >
            inline T *get( void ) noexcept
            {
                return static_cast< T * >( crimild::get_ptr( obj ) );
            }

            template< typename T >
            inline const T *get( void ) const noexcept
            {
                return static_cast< T * >( crimild::get_ptr( obj ) );
            }
        };

    public:
        void begin( Usage usage ) noexcept;
        void end( void ) noexcept;

        void beginRenderPass( RenderPass *renderPass, Framebuffer *framebuffer ) noexcept;
        void endRenderPass( RenderPass *renderPass ) noexcept;

        void bindCommandBuffer( CommandBuffer *commandBuffer ) noexcept;
        void bindGraphicsPipeline( GraphicsPipeline *pipeline ) noexcept;
        void bindComputePipeline( ComputePipeline *pipeline ) noexcept;
        void bindPrimitive( Primitive *primitive ) noexcept;
        void bindIndexBuffer( IndexBuffer *indexBuffer ) noexcept;
        void bindVertexBuffer( VertexBuffer *vertexBuffer ) noexcept;
        void bindUniformBuffer( UniformBuffer *uniformBuffer ) noexcept;
        void bindDescriptorSet( DescriptorSet *descriptorSet ) noexcept;

        void setIndexOffset( crimild::Size offset ) noexcept;
        void setScissor( const ViewportDimensions &scissor ) noexcept;
        void setVertexOffset( crimild::Size offset ) noexcept;
        void setViewport( const ViewportDimensions &viewport ) noexcept;

        void draw( crimild::UInt32 count ) noexcept;
        void drawIndexed( crimild::UInt32 count ) noexcept;
        void drawIndexed( crimild::UInt32 count, crimild::Size indexOffset, crimild::Size vertexOffset ) noexcept;
        void drawPrimitive( Primitive *primitive ) noexcept;

        void dispatch( const DispatchWorkgroup &workgroup ) noexcept;

        void clear( void ) noexcept;

		inline Bool cleared( void ) const noexcept { return m_cleared; }
  		inline void resetCleared( void ) noexcept { m_cleared = false; }

        template< typename CallbackType >
        void each( CallbackType &&callback ) noexcept
        {
            for ( auto &cmd : m_commands ) {
                callback( cmd );
            }
        }

    private:
        std::vector< Command > m_commands;
        Bool m_cleared = false;
    };

}

#endif
