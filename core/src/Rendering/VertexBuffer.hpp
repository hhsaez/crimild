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

#ifndef CRIMILD_RENDERING_VERTEX_BUFFER_
#define CRIMILD_RENDERING_VERTEX_BUFFER_

#include "Foundation/Observable.hpp"
#include "Foundation/RTTI.hpp"
#include "Rendering/Buffer.hpp"
#include "Rendering/BufferAccessor.hpp"
#include "Rendering/BufferView.hpp"
#include "Rendering/VertexLayout.hpp"

namespace crimild {

    class VertexBuffer
        : public coding::Codable,
          public Observable< VertexBuffer >,
          public RenderResourceImpl< VertexBuffer > {
        CRIMILD_IMPLEMENT_RTTI( crimild::VertexBuffer )

    public:
        /**
         * \brief Default constructor
         *
         * \remarks This is only used in order to comply with the Codable interface.
         */
        VertexBuffer( void ) = default;

        template< typename DATA_TYPE >
        VertexBuffer( const VertexLayout &vertexLayout, const Array< DATA_TYPE > &data ) noexcept
            : VertexBuffer(
                vertexLayout,
                [ & ] {
                    return crimild::alloc< BufferView >(
                        BufferView::Target::VERTEX,
                        crimild::alloc< Buffer >( data ),
                        0,
                        vertexLayout.getSize() );
                }() )
        {
            // no-op
        }

        // Compute data size based on the vertex layout and the count
        VertexBuffer( const VertexLayout &vertexLayout, crimild::Size count ) noexcept;

        /**
           \brief Creates a vertex buffer from a buffer view

           \remarks The buffer view must enclose the full buffer
         */
        VertexBuffer( const VertexLayout &vertexLayout, SharedPointer< BufferView > const &bufferView ) noexcept;

        VertexBuffer( const VertexLayout &vertexLayout, BufferView *bufferView ) noexcept;

        virtual ~VertexBuffer( void ) = default;

        inline const VertexLayout &getVertexLayout( void ) const noexcept { return m_vertexLayout; }

        inline BufferView *getBufferView( void ) noexcept { return crimild::get_ptr( m_bufferView ); }
        inline const BufferView *getBufferView( void ) const noexcept { return crimild::get_ptr( m_bufferView ); }

        inline crimild::Size getVertexCount( void ) const noexcept { return m_bufferView->getCount(); }

        BufferAccessor *get( VertexAttribute::Name name ) noexcept
        {
            return m_accessors.contains( name ) ? crimild::get_ptr( m_accessors[ name ] ) : nullptr;
        }

        const BufferAccessor *get( VertexAttribute::Name name ) const noexcept
        {
            return m_accessors.contains( name ) ? crimild::get_ptr( m_accessors[ name ] ) : nullptr;
        }

    private:
        VertexLayout m_vertexLayout;
        SharedPointer< BufferView > m_bufferView;
        Map< VertexAttribute::Name, SharedPointer< BufferAccessor > > m_accessors;

        /**
            \name Coding
         */
        //@{
    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
    };

}

#endif
