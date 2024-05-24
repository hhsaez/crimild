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

#include "Common/Observable.hpp"
#include "Crimild_Foundation.hpp"
#include "Rendering/Buffer.hpp"
#include "Rendering/BufferAccessor.hpp"
#include "Rendering/BufferView.hpp"
#include "Rendering/Format.hpp"

namespace crimild {

    class IndexBuffer
        : public coding::Codable,
          public Observable< IndexBuffer >,
          public RenderResourceImpl< IndexBuffer > {
        CRIMILD_IMPLEMENT_RTTI( crimild::IndexBuffer )

    public:
        /**
         * \brief Default constructor
         *
         * \remarks This is only used in order to comply with the Codable interface.
         */
        IndexBuffer( void ) = default;

        IndexBuffer( Format format, crimild::Size count ) noexcept;

        template< typename T >
        IndexBuffer( Format format, Array< T > const &data ) noexcept
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
            } else {
                m_accessor->each< crimild::UInt32 >( fn );
            }
        }

        template< typename T >
        void setIndex( crimild::Size i, T value ) noexcept
        {
            m_accessor->set< T >( i, value );
        }

        template< typename T >
        void setIndices( Array< T > const &data ) noexcept
        {
            m_accessor->set( data );
        }

    private:
        Format m_format;
        SharedPointer< BufferView > m_bufferView;
        SharedPointer< BufferAccessor > m_accessor;

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
