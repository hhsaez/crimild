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

#ifndef CRIMILD_RENDERING_BUFFER_VIEW_
#define CRIMILD_RENDERING_BUFFER_VIEW_

#include "Mathematics/Numeric.hpp"
#include "Rendering/Buffer.hpp"

namespace crimild {

    /**
       \brief A subset of data in a buffer
     */
    class BufferView
        : public coding::Codable,
          public RenderResourceImpl< BufferView > {
        CRIMILD_IMPLEMENT_RTTI( crimild::BufferView )

    public:
        enum class Target {
            GENERAL, //< Any target
            VERTEX,
            INDEX,
            UNIFORM,
            STORAGE,
            USER_DEFINED, //< custom target
        };

        // This is a hint to render devices so device memory
        // is optimized accordingly
        enum class Usage {
            STATIC,
            DYNAMIC,
        };

    public:
        /**
           \brief Constructs a buffer view from an existing buffer

           \param offset (optional) Offset to the first element, in bytes
           \param stride (optional) Stride in bytes between elements
           \param length (optional) Total number of bytes in this view

           If length is 0, it is assumed the view will corresponds to the segment starting
           at 'offset' and ending at the end of the input buffer itself.
         */
        template< typename BufferImpl >
        BufferView( Target target, BufferImpl buffer, crimild::Size offset = 0, crimild::Size stride = 0, crimild::Size length = 0 ) noexcept
            : m_target( target ),
              m_buffer( crimild::retain( buffer ) ),
              m_offset( offset ),
              m_stride( Numeric< crimild::Size >::max( sizeof( crimild::Byte ), stride ) ),
              m_length( length )
        {
            if ( m_length == 0 ) {
                m_length = buffer->getSize() - m_offset;
            }
        }

        virtual ~BufferView( void ) = default;

        inline Buffer *getBuffer( void ) noexcept { return crimild::get_ptr( m_buffer ); }
        inline const Buffer *getBuffer( void ) const noexcept { return crimild::get_ptr( m_buffer ); }

        /**
           \brief Get target usage for this view
         */
        inline Target getTarget( void ) const noexcept { return m_target; }

        inline void setUsage( Usage usage ) noexcept { m_usage = usage; }
        inline Usage getUsage( void ) const noexcept { return m_usage; }

        /**
           \brief Get number of bytes to the first element in the data buffer
         */
        inline crimild::Size getOffset( void ) const noexcept { return m_offset; }

        /**
           \brief Get stride between elements in the data buffer
         */
        inline crimild::Size getStride( void ) const noexcept { return m_stride; }

        /**
           \brief Get length in bytes for this view
         */
        inline crimild::Size getLength( void ) const noexcept { return m_length; }

        /**
           \brief Set length of the view

           The length value must be larger than the actual length of the buffer
         */
        inline void setLength( Size length ) noexcept { m_length = length; }

        /**
           \brief Get count of elements
         */
        inline crimild::Size getCount( void ) const noexcept { return m_length / m_stride; }

        inline crimild::Byte *getData( void ) noexcept { return m_buffer->getData() + m_offset; }

        inline const crimild::Byte *getData( void ) const noexcept { return m_buffer->getData() + m_offset; }

    private:
        Target m_target;
        SharedPointer< Buffer > m_buffer;
        crimild::Size m_offset;
        crimild::Size m_stride;
        crimild::Size m_length;
        Usage m_usage = Usage::STATIC;
    };

}

#endif
