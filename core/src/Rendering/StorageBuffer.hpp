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

#ifndef CRIMILD_RENDERING_STORAGE_BUFFER_
#define CRIMILD_RENDERING_STORAGE_BUFFER_

#include "Rendering/Buffer.hpp"
#include "Rendering/BufferAccessor.hpp"
#include "Rendering/BufferView.hpp"
#include "Rendering/FrameGraphResource.hpp"

namespace crimild {

    class StorageBuffer
        : public coding::Codable,
          public RenderResourceImpl< StorageBuffer >,
          public FrameGraphResource {
        CRIMILD_IMPLEMENT_RTTI( crimild::StorageBuffer )

    public:
        template< typename T >
        explicit StorageBuffer( Array< T > const &data ) noexcept
        {
            auto stride = sizeof( T );

            auto buffer = crimild::alloc< Buffer >( data );

            m_bufferView = crimild::alloc< BufferView >(
                BufferView::Target::STORAGE,
                buffer,
                0,
                stride );

            m_accessor = crimild::alloc< BufferAccessor >(
                m_bufferView,
                0,
                stride );
        }

        virtual ~StorageBuffer( void ) = default;

        inline Size getCount( void ) const noexcept
        {
            return m_bufferView->getCount();
        }

        inline BufferView *getBufferView( void ) noexcept { return crimild::get_ptr( m_bufferView ); }

        inline const BufferView *getBufferView( void ) const noexcept { return crimild::get_ptr( m_bufferView ); }

        template< typename T, typename Fn >
        void each( Fn fn ) const noexcept
        {
            m_accessor->each< T >( fn );
        }

    private:
        SharedPointer< BufferView > m_bufferView;
        SharedPointer< BufferAccessor > m_accessor;

        /**
         * \name FrameGraphResource impl
         */
        //@{

    public:
        inline FrameGraphResource::Type getType( void ) const noexcept override { return FrameGraphResource::Type::STORAGE_BUFFER; }

        virtual void setWrittenBy( FrameGraphOperation *op ) noexcept override;
        virtual void setReadBy( FrameGraphOperation *op ) noexcept override;

        //@}
    };

}

#endif
