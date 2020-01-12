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

#ifndef CRIMILD_RENDERING_BUFFER_
#define CRIMILD_RENDERING_BUFFER_

#include "Foundation/Types.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/RTTI.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Rendering/RenderResource.hpp"

namespace crimild {

    class Buffer : public RenderResourceImpl< Buffer > {
    public:
        enum class Usage {
            DONT_CARE,
            VERTEX_BUFFER,
            INDEX_BUFFER,
            UNIFORM_BUFFER,
            TRANSFER_SRC,
            TRANSFER_DST,
        };

    public:
        virtual ~Buffer( void ) noexcept = default;

        virtual Usage getUsage( void ) const noexcept = 0;

        virtual crimild::Size getSize( void ) const noexcept = 0;
        virtual crimild::Size getStride( void ) const noexcept = 0;

        virtual void *getRawData( void ) noexcept = 0;
        virtual const void *getRawData( void ) const noexcept = 0;
    };

    template< typename T, Buffer::Usage BUFFER_USAGE >
    class BufferImpl : public Buffer {
    public:
        BufferImpl( crimild::Size count, const T *data )
        {
            if ( count > 0 ) {
                m_data.resize( count );
                if ( data != nullptr ) {
                    memcpy( &m_data[ 0 ], data, count * sizeof( T ) );
                }
            }
        }

        BufferImpl( const containers::Array< T > &data )
            : m_data( data )
        {
            // no-op
        }

        virtual ~BufferImpl( void ) noexcept = default;

        Usage getUsage( void ) const noexcept override { return BUFFER_USAGE; }

        crimild::Size getSize( void ) const noexcept override { return m_data.size() * sizeof( T ); }
        crimild::Size getStride( void ) const noexcept override { return sizeof( T ); }

        void *getRawData( void ) noexcept override { return ( void * ) m_data.getData(); }
        const void *getRawData( void ) const noexcept override { return ( void * ) m_data.getData(); }

        crimild::Size getCount( void ) const noexcept { return getSize() / getStride(); }

        T *getData( void ) noexcept { return m_data.getData(); }
        const T *getData( void ) const noexcept { return m_data.getData(); }

    private:
        containers::Array< T > m_data;
    };

}

#endif
