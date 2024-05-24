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

#include "Crimild_Mathematics.hpp"
#include "Rendering/Vertex.hpp"

#include <vector>

using namespace crimild;

SharedPointer< Primitive > BoxPrimitive::UNIT_BOX = crimild::alloc< BoxPrimitive >( BoxPrimitive::Params {} );

BoxPrimitive::BoxPrimitive( void ) noexcept
    : BoxPrimitive( Params {} )
{
    // no-op
}

BoxPrimitive::BoxPrimitive( const Params &params ) noexcept
    : Primitive( params.type )
{
    auto layout = params.layout;

    auto vertices = crimild::alloc< VertexBuffer >( layout, 24 );

    auto w = ( params.invertFaces ? -1.0f : 1.0f ) * params.size.x;
    auto h = ( params.invertFaces ? -1.0f : 1.0f ) * params.size.y;
    auto d = ( params.invertFaces ? -1.0f : 1.0f ) * params.size.z;

    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    positions->set(
        Array< Vector3f > {
            // top
            Vector3f { -w, +h, -d },
            Vector3f { -w, +h, +d },
            Vector3f { +w, +h, +d },
            Vector3f { +w, +h, -d },

            // front
            Vector3f { -w, +h, +d },
            Vector3f { -w, -h, +d },
            Vector3f { +w, -h, +d },
            Vector3f { +w, +h, +d },

            // back
            Vector3f { +w, +h, -d },
            Vector3f { +w, -h, -d },
            Vector3f { -w, -h, -d },
            Vector3f { -w, +h, -d },

            // left
            Vector3f { -w, +h, -d },
            Vector3f { -w, -h, -d },
            Vector3f { -w, -h, +d },
            Vector3f { -w, +h, +d },

            // right
            Vector3f { +w, +h, +d },
            Vector3f { +w, -h, +d },
            Vector3f { +w, -h, -d },
            Vector3f { +w, +h, -d },

            // bottom
            Vector3f { +w, -h, -d },
            Vector3f { +w, -h, +d },
            Vector3f { -w, -h, +d },
            Vector3f { -w, -h, -d },
        }
    );

    if ( layout.hasAttribute( VertexAttribute::Name::NORMAL ) ) {
        auto normals = vertices->get( VertexAttribute::Name::NORMAL );
        auto R = Vector3f::Constants::UNIT_X;
        auto U = Vector3f::Constants::UNIT_Y;
        auto F = Vector3f::Constants::UNIT_Z;
        normals->set(
            Array< Vector3f > {
                // top
                U,
                U,
                U,
                U,

                // front
                F,
                F,
                F,
                F,

                // back
                -F,
                -F,
                -F,
                -F,

                // left
                -R,
                -R,
                -R,
                -R,

                // right
                R,
                R,
                R,
                R,

                // bottom
                -U,
                -U,
                -U,
                -U,
            }
        );
    }

    if ( layout.hasAttribute( VertexAttribute::Name::COLOR ) ) {
        auto colors = vertices->get( VertexAttribute::Name::COLOR );
        auto r = ColorRGB { 1.0f, 0.0f, 0.0f };
        auto g = ColorRGB { 0.0f, 1.0f, 0.0f };
        auto b = ColorRGB { 0.0f, 0.0f, 1.0f };
        auto c = ColorRGB { 0.0f, 1.0f, 1.0f };
        auto m = ColorRGB { 1.0f, 0.0f, 1.0f };
        auto y = ColorRGB { 1.0f, 1.0f, 0.0f };
        colors->set(
            Array< ColorRGB > {
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
        Real s0 = params.invertFaces ? 1.0 : 0.0;
        Real s1 = params.invertFaces ? 0.0 : 1.0;
        Real t0 = params.invertFaces ? 1.0 : 0.0;
        Real t1 = params.invertFaces ? 0.0 : 1.0;
        texCoords->set(
            Array< Vector2f > {
                // top
                Vector2f { s0, t0 },
                Vector2f { s0, t1 },
                Vector2f { s1, t1 },
                Vector2f { s1, t0 },

                // front
                Vector2f { s0, t0 },
                Vector2f { s0, t1 },
                Vector2f { s1, t1 },
                Vector2f { s1, t0 },

                // back
                Vector2f { s0, t0 },
                Vector2f { s0, t1 },
                Vector2f { s1, t1 },
                Vector2f { s1, t0 },

                // left
                Vector2f { s0, t0 },
                Vector2f { s0, t1 },
                Vector2f { s1, t1 },
                Vector2f { s1, t0 },

                // right
                Vector2f { s0, t0 },
                Vector2f { s0, t1 },
                Vector2f { s1, t1 },
                Vector2f { s1, t0 },

                // bottom
                Vector2f { s0, t0 },
                Vector2f { s0, t1 },
                Vector2f { s1, t1 },
                Vector2f { s1, t0 },
            }
        );
    }
    setVertexData( { vertices } );

    setIndices(
        crimild::alloc< IndexBuffer >(
            Format::INDEX_32_UINT,
            Array< UInt32 > {
                0,
                1,
                2,

                0,
                2,
                3,

                4,
                5,
                6,

                4,
                6,
                7,

                8,
                9,
                10,

                8,
                10,
                11,

                12,
                13,
                14,

                12,
                14,
                15,

                16,
                17,
                18,

                16,
                18,
                19,

                20,
                21,
                22,

                20,
                22,
                23,
            }
        )
    );
}
