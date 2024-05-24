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

#ifndef CRIMILD_RENDERING_UNIFORM_BUFFER_
#define CRIMILD_RENDERING_UNIFORM_BUFFER_

#include "Common/Observable.hpp"
#include "Crimild_Foundation.hpp"
#include "Rendering/Buffer.hpp"
#include "Rendering/BufferView.hpp"

namespace crimild {

    class UniformBuffer
        : public coding::Codable,
          public Observable< UniformBuffer >,
          public RenderResourceImpl< UniformBuffer > {
        CRIMILD_IMPLEMENT_RTTI( crimild::UniformBuffer )

    public:
        template< typename T >
        UniformBuffer( const T &value ) noexcept
        {
            m_bufferView = crimild::alloc< BufferView >(
                BufferView::Target::UNIFORM,
                crimild::alloc< Buffer >( value )
            );
        }

        virtual ~UniformBuffer( void ) = default;

        inline BufferView *getBufferView( void ) noexcept { return crimild::get_ptr( m_bufferView ); }
        inline const BufferView *getBufferView( void ) const noexcept { return crimild::get_ptr( m_bufferView ); }

        template< typename T >
        T &getValue( void ) noexcept
        {
            return *reinterpret_cast< T * >( getBufferView()->getData() );
        }

        template< typename T >
        const T &getValue( void ) const noexcept
        {
            return *reinterpret_cast< const T * >( getBufferView()->getData() );
        }

        template< typename T >
        void setValue( const T &value ) noexcept
        {
            assert( sizeof( T ) == getBufferView()->getLength() && "Invalid data type" );
            getValue< T >() = value;
        }

        /**
           \brief Invoked when a frame is about to be rendered

           Derived classes can use this function to update the state of the uniform buffer if needed
         */
        virtual void onPreRender( void ) noexcept { }

    private:
        SharedPointer< BufferView > m_bufferView;
    };

}

#endif
