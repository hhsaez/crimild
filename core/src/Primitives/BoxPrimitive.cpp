/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "BoxPrimitive.hpp"
#include "Rendering/Vertex.hpp"

#include <vector>

using namespace crimild;

BoxPrimitive::BoxPrimitive( void ) noexcept
    : BoxPrimitive( Params { } )
{
    // no-op
}

BoxPrimitive::BoxPrimitive( const Params &params ) noexcept
    : Primitive( params.type )
{
    auto layout = params.layout;

    auto vertices = crimild::alloc< VertexBuffer >( layout, 24 );

    auto w = params.size.x();
    auto h = params.size.y();
    auto d = params.size.z();

    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    positions->set(
        Array< Vector3f > {
            // top
            Vector3f( -w, +h, -d ),
            Vector3f( -w, +h, +d ),
            Vector3f( +w, +h, +d ),
            Vector3f( +w, +h, -d ),

            // front
            Vector3f( -w, +h, +d ),
            Vector3f( -w, -h, +d ),
            Vector3f( +w, -h, +d ),
            Vector3f( +w, +h, +d ),

            // back
            Vector3f( +w, +h, -d ),
            Vector3f( +w, -h, -d ),
            Vector3f( -w, -h, -d ),
            Vector3f( -w, +h, -d ),

            // left
            Vector3f( -w, +h, -d ),
            Vector3f( -w, -h, -d ),
            Vector3f( -w, -h, +d ),
            Vector3f( -w, +h, +d ),

            // right
            Vector3f( +w, +h, +d ),
            Vector3f( +w, -h, +d ),
            Vector3f( +w, -h, -d ),
            Vector3f( +w, +h, -d ),

            // bottom
            Vector3f( +w, -h, -d ),
            Vector3f( +w, -h, +d ),
            Vector3f( -w, -h, +d ),
            Vector3f( -w, -h, -d ),
        }
    );

    if ( layout.hasAttribute( VertexAttribute::Name::NORMAL ) ) {
        auto normals = vertices->get( VertexAttribute::Name::NORMAL );
        normals->set(
            Array< Vector3f > {
                // top
                Vector3f::UNIT_Y,
                Vector3f::UNIT_Y,
                Vector3f::UNIT_Y,
                Vector3f::UNIT_Y,

                // front
                Vector3f::UNIT_Z,
                Vector3f::UNIT_Z,
                Vector3f::UNIT_Z,
                Vector3f::UNIT_Z,

                // back
                -Vector3f::UNIT_Z,
                -Vector3f::UNIT_Z,
                -Vector3f::UNIT_Z,
                -Vector3f::UNIT_Z,

                // left
                -Vector3f::UNIT_X,
                -Vector3f::UNIT_X,
                -Vector3f::UNIT_X,
                -Vector3f::UNIT_X,

                // right
                Vector3f::UNIT_X,
                Vector3f::UNIT_X,
                Vector3f::UNIT_X,
                Vector3f::UNIT_X,

                // bottom
                -Vector3f::UNIT_Y,
                -Vector3f::UNIT_Y,
                -Vector3f::UNIT_Y,
                -Vector3f::UNIT_Y,
            }
        );
    }

    if ( layout.hasAttribute( VertexAttribute::Name::COLOR ) ) {
        auto colors = vertices->get( VertexAttribute::Name::COLOR );
        auto r = RGBColorf( 1.0f, 0.0f, 0.0f );
        auto g = RGBColorf( 0.0f, 1.0f, 0.0f );
        auto b = RGBColorf( 0.0f, 0.0f, 1.0f );
        auto c = RGBColorf( 0.0f, 1.0f, 1.0f );
        auto m = RGBColorf( 1.0f, 0.0f, 1.0f );
        auto y = RGBColorf( 1.0f, 1.0f, 0.0f );
        colors->set(
            Array< Vector3f > {
                // top
                g,
                g,
                g,
                g,

                // front
                b,
                b,
                b,
                b,

                // back
                c,
                c,
                c,
                c,

                // left
                y,
                y,
                y,
                y,

                // right
                r,
                r,
                r,
                r,

                // bottom
                m,
                m,
                m,
                m,
            }
        );
    }

    if ( layout.hasAttribute( VertexAttribute::Name::TEX_COORD ) ) {
        auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );
        texCoords->set(
            Array< Vector2f > {
                // top
                Vector2f( 0.0f, 0.0f ),
                Vector2f( 0.0f, 1.0f ),
                Vector2f( 1.0f, 1.0f ),
                Vector2f( 1.0f, 0.0f ),

                // front
                Vector2f( 0.0f, 0.0f ),
                Vector2f( 0.0f, 1.0f ),
                Vector2f( 1.0f, 1.0f ),
                Vector2f( 1.0f, 0.0f ),

                // back
                Vector2f( 0.0f, 0.0f ),
                Vector2f( 0.0f, 1.0f ),
                Vector2f( 1.0f, 1.0f ),
                Vector2f( 1.0f, 0.0f ),

                // left
                Vector2f( 0.0f, 0.0f ),
                Vector2f( 0.0f, 1.0f ),
                Vector2f( 1.0f, 1.0f ),
                Vector2f( 1.0f, 0.0f ),

                // right
                Vector2f( 0.0f, 0.0f ),
                Vector2f( 0.0f, 1.0f ),
                Vector2f( 1.0f, 1.0f ),
                Vector2f( 1.0f, 0.0f ),

                // bottom
                Vector2f( 0.0f, 0.0f ),
                Vector2f( 0.0f, 1.0f ),
                Vector2f( 1.0f, 1.0f ),
                Vector2f( 1.0f, 0.0f ),
            }
        );
    }
    setVertexData( { vertices } );

    setIndices(
        crimild::alloc< IndexBuffer >(
            Format::INDEX_32_UINT,
            Array< UInt32 > {
                0, 1, 2, 0, 2, 3,
                4, 5, 6, 4, 6, 7,
                8, 9, 10, 8, 10, 11,
                12, 13, 14, 12, 14, 15,
                16, 17, 18, 16, 18, 19,
                20, 21, 22, 20, 22, 23,
            }
        )
    );
}
