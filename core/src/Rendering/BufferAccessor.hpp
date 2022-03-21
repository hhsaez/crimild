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

#ifndef CRIMILD_RENDERING_BUFFER_ACCESSOR_
#define CRIMILD_RENDERING_BUFFER_ACCESSOR_

#include "Rendering/BufferView.hpp"

namespace crimild {

    /**
       \brief Define methods for retrieving data from a buffer view.
     */
    class BufferAccessor : public coding::Codable {
        CRIMILD_IMPLEMENT_RTTI( crimild::BufferAccessor )

    public:
        /**
         * \brief Default constructor
         *
         * \remarks This is only used in order to comply with the Codable interface.
         */
        BufferAccessor( void ) = default;

        template< typename BufferViewPtr >
        BufferAccessor( BufferViewPtr bufferView, crimild::Size offset = 0, crimild::Size size = 0 ) noexcept
            : m_bufferView( crimild::retain( bufferView ) ),
              m_offset( offset ),
              m_size( size > 0 ? size : bufferView->getStride() )
        {
        }

        virtual ~BufferAccessor( void ) = default;

        inline BufferView *getBufferView( void ) noexcept { return crimild::get_ptr( m_bufferView ); }
        inline const BufferView *getBufferView( void ) const noexcept { return crimild::get_ptr( m_bufferView ); }

        inline crimild::Size getOffset( void ) const noexcept { return m_offset; }

        inline crimild::Size getSize( void ) const noexcept { return m_size; }

        template< typename T >
        T &get( crimild::Size index = 0 ) noexcept
        {
            assert( sizeof( T ) == getSize() && "Invalid template type argument" );
            auto data = getBufferView()->getData();
            auto offset = getOffset();
            auto stride = getBufferView()->getStride();
            return *reinterpret_cast< T * >( &data[ index * stride + offset ] );
        }

        template< typename T >
        const T &get( crimild::Size index = 0 ) const noexcept
        {
            assert( sizeof( T ) == getSize() && "Invalid template type argument" );
            auto data = getBufferView()->getData();
            auto offset = getOffset();
            auto stride = getBufferView()->getStride();
            return *reinterpret_cast< T * >( &data[ index * stride + offset ] );
        }

        template< typename T, typename Fn >
        void each( Fn fn ) const noexcept
        {
            assert( sizeof( T ) == getSize() && "Invalid template type argument" );

            auto data = getBufferView()->getData();
            auto offset = getOffset();
            auto stride = getBufferView()->getStride();
            auto N = getBufferView()->getCount();

            for ( auto index = 0l; index < N; index++ ) {
                fn( *reinterpret_cast< const T * >( &data[ index * stride + offset ] ), index );
            }
        }

        template< typename T >
        void set( crimild::Size index, const T &value ) noexcept
        {
            assert( sizeof( T ) == getSize() && "Invalid template type argument" );
            auto data = getBufferView()->getData();
            auto offset = getOffset();
            auto stride = getBufferView()->getStride();
            memcpy( &data[ index * stride + offset ], &value, getSize() );
        }

        template< typename T >
        void set( Array< T > const &src ) noexcept
        {
            auto data = getBufferView()->getData();
            auto O = getOffset();
            auto S = getBufferView()->getStride();
            auto N = getBufferView()->getCount();
            auto size = getSize();
            auto srcData = src.getData();
            auto srcStride = size / sizeof( T );

            assert( src.size() * sizeof( T ) / size == N && "Invalid data size" );

            for ( auto i = 0l; i < N; i++ ) {
                memcpy( &data[ i * S + O ], &srcData[ i * srcStride ], size );
            }
        }

    private:
        SharedPointer< BufferView > m_bufferView;
        crimild::Size m_offset;
        crimild::Size m_size;

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
