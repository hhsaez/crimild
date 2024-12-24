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

#include "Crimild_Coding.hpp"
#include "Crimild_Foundation.hpp"
#include "Rendering/FrameGraphResource.hpp"
#include "Rendering/RenderResource.hpp"

#include <cstring>

namespace crimild {

    /**
       \brief A buffer of linear data
     */
    class Buffer
        : public coding::Codable,
          public RenderResourceImpl< Buffer >,
          public FrameGraphResource {
        CRIMILD_IMPLEMENT_RTTI( crimild::Buffer )

    public:
        /**
         * \brief Default constructor
         *
         * \remarks This is only used in order to comply with the Codable interface.
         */
        Buffer( void ) = default;

        /**
           \brief Initialize buffer with array of data

           This is usually used for vertex or index buffers
         */
        template< typename DATA_TYPE >
        explicit Buffer( Array< DATA_TYPE > const &data )
            : m_data( sizeof( DATA_TYPE ) * data.size() )
        {
            memcpy( &m_data[ 0 ], &data[ 0 ], m_data.size() );
        }

        /**
           \brief Initialize buffer for a single value

           This is usually used with uniform buffers
         */
        template< typename DATA_TYPE >
        explicit Buffer( DATA_TYPE const &data )
            : m_data( sizeof( DATA_TYPE ) )
        {
            memcpy( &m_data[ 0 ], &data, m_data.size() );
        }

        virtual ~Buffer( void ) = default;

        /**
           \brief Get size of buffer in bytes
         */
        inline crimild::Size getSize( void ) const noexcept { return m_data.size(); }

        /**
           \brief Get raw data
         */
        inline crimild::Byte *getData( void ) noexcept { return m_data.getData(); }

        /**
           \brief Get raw data
         */
        inline const crimild::Byte *getData( void ) const noexcept { return m_data.getData(); }

    private:
        ByteArray m_data;

        /**
         * \name FrameGraphResource impl
         */
        //@{

    public:
        inline FrameGraphResource::Type getType( void ) const noexcept override { return FrameGraphResource::Type::BUFFER; }

        //@}

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
