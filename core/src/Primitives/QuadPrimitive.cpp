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

#include "QuadPrimitive.hpp"

using namespace crimild;

QuadPrimitive::QuadPrimitive( void ) noexcept
    : QuadPrimitive( Params { } )
{

}

QuadPrimitive::QuadPrimitive( const Params &params ) noexcept
    : Primitive( params.type )
{
    auto w = params.size.x();
    auto h = params.size.y();
    auto layout = params.layout;
    auto texCoordOffset = params.texCoordOffset;
    auto texCoordScale = params.texCoordScale;

    auto vertices = crimild::alloc< VertexBuffer >( layout, 4 );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    positions->set(
        Array< Vector3f > {
            Vector3f( -w, h, 0.0f ),
            Vector3f( -w, -h, 0.0f ),
            Vector3f( w, -h, 0.0f ),
            Vector3f( w, h, 0.0f ),
        }
    );

    if ( layout.hasAttribute( VertexAttribute::Name::NORMAL ) ) {
        auto normals = vertices->get( VertexAttribute::Name::NORMAL );
        normals->set(
            Array< Vector3f > {
                Vector3f::UNIT_Z,
                Vector3f::UNIT_Z,
                Vector3f::UNIT_Z,
                Vector3f::UNIT_Z,
            }
        );
    }

    if ( layout.hasAttribute( VertexAttribute::Name::TEX_COORD ) ) {
        auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );
        texCoords->set(
            Array< Vector2f > {
                texCoordOffset + Vector2f( 0.0f, 0.0f ).times( texCoordScale ),
                texCoordOffset + Vector2f( 0.0f, 1.0f ).times( texCoordScale ),
                texCoordOffset + Vector2f( 1.0f, 1.0f ).times( texCoordScale ),
                texCoordOffset + Vector2f( 1.0f, 0.0f ).times( texCoordScale ),
            }
        );
    }

    setVertexData( { vertices } );

    if ( getType() == Primitive::Type::LINES ) {
        setIndices(
            crimild::alloc< IndexBuffer >(
                Format::INDEX_32_UINT,
                Array< UInt32 > {
                    0, 1,
                    1, 2,
                    2, 3,
                    3, 0,
                }
            )
        );
    }
    else if ( getType() == Primitive::Type::TRIANGLE_STRIP ) {
        // make sure indices are in the right order
        setIndices(
            crimild::alloc< IndexBuffer >(
                Format::INDEX_32_UINT,
                Array< UInt32 > {
                    1, 2, 0, 3,
                }
            )
        );
    }
    else {
        setIndices(
            crimild::alloc< IndexBuffer >(
                Format::INDEX_32_UINT,
                Array< UInt32 > {
                    0, 1, 2,
                    0, 2, 3,
                }
            )
        );
    }
}
