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

#ifndef CRIMILD_RENDERING_INDEX_BUFFER_
#define CRIMILD_RENDERING_INDEX_BUFFER_

#include "Rendering/Buffer.hpp"
#include "Rendering/BufferView.hpp"
#include "Rendering/BufferAccessor.hpp"
#include "Rendering/Format.hpp"

namespace crimild {

#if 0
    class IndexBuffer : public Buffer, public RTTI {
    public:
        enum class IndexType {
            UINT_16,
            UINT_32,
        };

    public:
        virtual ~IndexBuffer( void ) noexcept = default;

        Buffer::Usage getUsage( void ) const noexcept { return Buffer::Usage::INDEX_BUFFER; }

        virtual crimild::Size getCount( void ) const noexcept = 0;

        virtual IndexType getIndexType( void ) const noexcept = 0;
    };

    template< typename Index >
    class IndexBufferImpl : public IndexBuffer {
    public:
        explicit IndexBufferImpl( crimild::Size count ) noexcept
            : m_indices( count )
        {
            // nothing to do
        }

        explicit IndexBufferImpl( const containers::Array< Index > &indices ) noexcept
            : m_indices( indices )
        {
            // nothing to do
        }

        /**
            \remarks When implementing custom vertex formats, don't
             forget to implement a scecialization for getClassName()
             so RTTI works.
         */
        virtual const char *getClassName( void ) const override;

        virtual ~IndexBufferImpl( void ) noexcept = default;

        crimild::Size getSize( void ) const noexcept override { return m_indices.size() * sizeof( Index ); }
        crimild::Size getStride( void ) const noexcept override { return sizeof( Index ); }

        void *getRawData( void ) noexcept override { return ( void * ) m_indices.getData(); }
        const void *getRawData( void ) const noexcept override { return ( void * ) m_indices.getData(); }

        crimild::Size getCount( void ) const noexcept override { return getSize() / getStride(); }

        Index *getData( void ) noexcept { return m_indices.getData(); }
        const Index *getData( void ) const noexcept { return m_indices.getData(); }

        IndexBuffer::IndexType getIndexType( void ) const noexcept override;

    private:
        containers::Array< Index > m_indices;
    };

    using IndexUInt16 = crimild::UInt16;
    using IndexUInt16Buffer = IndexBufferImpl< crimild::UInt16 >;

    using IndexUInt32 = crimild::UInt32;
    using IndexUInt32Buffer = IndexBufferImpl< crimild::UInt32 >;

#endif

    class IndexBuffer
        : public coding::Codable,
          public RenderResourceImpl< IndexBuffer > {
        CRIMILD_IMPLEMENT_RTTI( crimild::IndexBuffer )
        
    public:
        IndexBuffer( Format format, crimild::Size count ) noexcept;

        template< typename T >
        IndexBuffer( Format format, containers::Array< T > const &data ) noexcept
            : m_format( format )
        {
            auto stride = utils::getFormatSize( format );

            auto buffer = crimild::alloc< Buffer >( data );
    
            m_bufferView = crimild::alloc< BufferView >(
                BufferView::Target::INDEX,
                buffer,
                0,
                stride
            );
            
            m_accessor = crimild::alloc< BufferAccessor >(
                m_bufferView,
                0,
                stride
            );
        }
        
        virtual ~IndexBuffer( void ) = default;

        inline Format getFormat( void ) const noexcept
        {
            return m_format;
        }

        inline crimild::Size getIndexCount( void ) const noexcept
        {
            return m_bufferView->getCount();
        }

        inline BufferView *getBufferView( void ) const noexcept
        {
            return crimild::get_ptr( m_bufferView );
        }
        
        crimild::UInt32 getIndex( crimild::Size i ) const noexcept
        {
            if ( m_format == Format::INDEX_16_UINT ) {
                return m_accessor->get< crimild::UInt16 >( i );
            }
            return m_accessor->get< crimild::UInt32 >( i );
        }

        template< typename Fn >
        void each( Fn fn ) const noexcept
        {
            if ( m_format == Format::INDEX_16_UINT ) {
                m_accessor->each< crimild::UInt16 >( fn );
            }
            else {
                m_accessor->each< crimild::UInt32 >( fn );
            }
        }

        template< typename T >
        void setIndex( crimild::Size i, T value ) noexcept
        {
            m_accessor->set< T >( i, value );
        }

        template< typename T >
        void setIndices( containers::Array< T > const &data ) noexcept
        {
            m_accessor->set( data );
        }
        
    private:
        Format m_format;
        SharedPointer< BufferView > m_bufferView;
        SharedPointer< BufferAccessor > m_accessor;
    };

}

#endif


