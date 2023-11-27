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

#ifndef CRIMILD_RENDERING_VERTEX_
#define CRIMILD_RENDERING_VERTEX_

#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/isEqual.hpp"
#include "Rendering/VertexLayout.hpp"

namespace crimild {

    struct VertexP2 {
        static VertexLayout getLayout( void ) noexcept
        {
            return VertexLayout()
                .withAttribute< Vector3f >( VertexAttribute::Name::POSITION );
        };

        Vector2f position;
    };

    struct VertexP2C3 {
        static VertexLayout getLayout( void ) noexcept
        {
            return VertexLayout()
                .withAttribute< Vector2f >( VertexAttribute::Name::POSITION )
                .withAttribute< ColorRGB >( VertexAttribute::Name::COLOR );
        };

        Vector2f position;
        ColorRGB color;
    };

    struct VertexP2C3TC2 {
        static VertexLayout getLayout( void ) noexcept
        {
            return VertexLayout()
                .withAttribute< Vector2f >( VertexAttribute::Name::POSITION )
                .withAttribute< ColorRGB >( VertexAttribute::Name::COLOR )
                .withAttribute< Vector2f >( VertexAttribute::Name::TEX_COORD );
        };

        crimild::Vector2f position;
        crimild::ColorRGB color;
        crimild::Vector2f texCoord;
    };

    struct VertexP2TC2C4 {
        static VertexLayout getLayout( void ) noexcept
        {
            return VertexLayout()
                .withAttribute< Vector2f >( VertexAttribute::Name::POSITION )
                .withAttribute< Vector2f >( VertexAttribute::Name::TEX_COORD )
                .withAttribute< ColorRGBA >( VertexAttribute::Name::COLOR );
        };

        crimild::Vector2f position;
        crimild::Vector2f texCoord;
        crimild::ColorRGBA color;
    };

    struct VertexP3 {
        static VertexLayout getLayout( void ) noexcept
        {
            return VertexLayout()
                .withAttribute< Vector3f >( VertexAttribute::Name::POSITION );
        }

        crimild::Vector3f position;
    };

    struct VertexP3C3 {
        static VertexLayout getLayout( void ) noexcept
        {
            return VertexLayout()
                .withAttribute< Vector3f >( VertexAttribute::Name::POSITION )
                .withAttribute< ColorRGB >( VertexAttribute::Name::COLOR );
        }

        Vector3f position;
        ColorRGB color;
    };

    struct VertexP3C3TC2 {
        static VertexLayout getLayout( void ) noexcept
        {
            return VertexLayout()
                .withAttribute< Vector3f >( VertexAttribute::Name::POSITION )
                .withAttribute< ColorRGB >( VertexAttribute::Name::COLOR )
                .withAttribute< Vector3f >( VertexAttribute::Name::TEX_COORD );
        }

        crimild::Vector3f position;
        crimild::ColorRGB color;
        crimild::Vector2f texCoord;
    };

    struct VertexP3N3 {
        static VertexLayout getLayout( void ) noexcept
        {
            return VertexLayout()
                .withAttribute< Vector3f >( VertexAttribute::Name::POSITION )
                .withAttribute< Vector3f >( VertexAttribute::Name::NORMAL );
        }

        crimild::Vector3f position;
        crimild::Vector3f normal;
    };

    struct VertexP3N3TC2 {
        static VertexLayout getLayout( void ) noexcept
        {
            return VertexLayout()
                .withAttribute< Vector3f >( VertexAttribute::Name::POSITION )
                .withAttribute< Vector3f >( VertexAttribute::Name::NORMAL )
                .withAttribute< Vector2f >( VertexAttribute::Name::TEX_COORD );
        }

        crimild::Vector3f position;
        crimild::Vector3f normal;
        crimild::Vector2f texCoord;

        // This is requried to be able to use this vertex type in some containers, like maps
        // See OBJLoader.cpp for a use example
        friend bool operator==( const VertexP3N3TC2 &lhs, const VertexP3N3TC2 &rhs ) noexcept
        {
            return isEqual( lhs.position, rhs.position ) && isEqual( lhs.normal, rhs.normal ) && isEqual( lhs.texCoord, rhs.texCoord );
        }

        // This is requried to be able to use this vertex type in some containers, like maps
        // See OBJLoader.cpp for a use example
        friend bool operator<( const VertexP3N3TC2 &lhs, const VertexP3N3TC2 &rhs ) noexcept
        {
            return lhs.position.x < rhs.position.x
                   && lhs.position.y < rhs.position.y
                   && lhs.position.z < rhs.position.z;
        }
    };

    struct VertexP3TC2 {
        static VertexLayout getLayout( void ) noexcept
        {
            return VertexLayout()
                .withAttribute< Vector3f >( VertexAttribute::Name::POSITION )
                .withAttribute< Vector2f >( VertexAttribute::Name::TEX_COORD );
        }

        crimild::Vector3f position;
        crimild::Vector2f texCoord;
    };

}

#endif
