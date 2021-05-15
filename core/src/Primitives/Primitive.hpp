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
 *     * Neither the name of the copyright holders nor the
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

#ifndef CRIMILD_PRIMITIVES_PRIMITIVE_
#define CRIMILD_PRIMITIVES_PRIMITIVE_

#include "Coding/Codable.hpp"
#include "Rendering/Catalog.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/VertexBuffer.hpp"

#include <functional>

namespace crimild {

    class Primitive
        : public coding::Codable,
          public Catalog< Primitive >::Resource {
        CRIMILD_IMPLEMENT_RTTI( crimild::Primitive )

    private:
        using VertexData = Array< SharedPointer< VertexBuffer > >;
        using IndexData = SharedPointer< IndexBuffer >;

    public:
        enum class Type : uint8_t {
            POINTS,
            LINES,
            LINE_LOOP,
            LINE_STRIP,
            TRIANGLES,
            TRIANGLE_STRIP,
            TRIANGLE_FAN
        };

    public:
        explicit Primitive( Primitive::Type type = Primitive::Type::TRIANGLES );
        virtual ~Primitive( void ) = default;

        Primitive::Type getType( void ) const { return _type; }

        inline void setVertexData( VertexData const vertexData ) noexcept { m_vertexData = vertexData; }
        inline VertexData &getVertexData( void ) noexcept { return m_vertexData; }
        inline const VertexData &getVertexData( void ) const noexcept { return m_vertexData; }

        inline void setIndices( SharedPointer< IndexBuffer > const &indices ) noexcept { m_indices = indices; }
        inline IndexBuffer *getIndices( void ) noexcept { return get_ptr( m_indices ); }
        inline const IndexBuffer *getIndices( void ) const noexcept { return get_ptr( m_indices ); }

    private:
        Primitive::Type _type;
        VertexData m_vertexData;
        IndexData m_indices;

        /**
         */
        //@{

    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
    };

    using PrimitivePtr = SharedPointer< Primitive >;

}

#endif
