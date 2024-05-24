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

#include "SpherePrimitive.hpp"

#include "Crimild_Mathematics.hpp"

#include <vector>

using namespace crimild;

SharedPointer< Primitive > SpherePrimitive::UNIT_SPHERE = crimild::alloc< SpherePrimitive >( SpherePrimitive::Params {} );

SpherePrimitive::SpherePrimitive( void ) noexcept
    : SpherePrimitive( Params {} )
{
}

SpherePrimitive::SpherePrimitive( const Params &params ) noexcept
    : Primitive( Primitive::Type::TRIANGLES )
{
    auto layout = params.layout;
    auto divisions = params.divisions;
    auto radius = params.radius;
    auto center = params.center;

    auto N = ( 1 + divisions[ 0 ] ) * ( 1 + divisions[ 1 ] );

    auto vertices = crimild::alloc< VertexBuffer >( layout, N );
    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    auto colors = layout.hasAttribute( VertexAttribute::Name::COLOR ) ? vertices->get( VertexAttribute::Name::COLOR ) : nullptr;
    auto normals = layout.hasAttribute( VertexAttribute::Name::NORMAL ) ? vertices->get( VertexAttribute::Name::NORMAL ) : nullptr;
    auto texCoords = layout.hasAttribute( VertexAttribute::Name::TEX_COORD ) ? vertices->get( VertexAttribute::Name::TEX_COORD ) : nullptr;

    for ( auto latitude = 0.0f; latitude <= divisions[ 1 ]; latitude += 1.0f ) {
        auto theta = latitude * Numericf::PI / divisions[ 1 ];
        auto sinTheta = Numericf::sin( theta );
        auto cosTheta = Numericf::cos( theta );

        for ( auto longitude = 0.0f; longitude <= divisions[ 0 ]; longitude += 1.0f ) {
            auto phi = longitude * Numericf::TWO_PI / divisions[ 0 ];
            auto sinPhi = Numericf::sin( phi );
            auto cosPhi = Numericf::cos( phi );

            auto x = cosPhi * sinTheta;
            auto y = cosTheta;
            auto z = sinPhi * sinTheta;

            auto vIdx = latitude * ( 1 + divisions[ 0 ] ) + longitude;

            positions->set( vIdx, center + radius * Vector3f { x, y, z } );

            if ( colors != nullptr ) {
                colors->set( vIdx, ColorRGB { 0.5f, 0.5f, 0.5f } + 0.5f * ColorRGB { x, y, z } );
            }

            if ( normals != nullptr ) {
                normals->set( vIdx, Vector3f { x, y, z } );
            }

            if ( texCoords != nullptr ) {
                auto u = 1.0f - ( longitude / divisions[ 0 ] );
                auto v = ( latitude / divisions[ 1 ] );
                texCoords->set( vIdx, Vector2f { u, v } );
            }
        }
    }
    setVertexData( { vertices } );

    if ( getType() == Primitive::Type::POINTS ) {
        auto indices = crimild::alloc< IndexBuffer >(
            Format::INDEX_32_UINT,
            Array< Int32 >( divisions[ 1 ] * divisions[ 0 ] ).fill( []( auto i ) { return i; } )
        );
        setIndices( indices );
    } else {
        auto indices = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, 6 * divisions[ 1 ] * divisions[ 0 ] );
        for ( auto latitude = 0l; latitude < divisions[ 1 ]; latitude++ ) {
            for ( auto longitude = 0l; longitude < divisions[ 0 ]; longitude++ ) {
                UInt32 first = ( latitude * ( divisions[ 1 ] + 1 ) ) + longitude;
                UInt32 second = first + divisions[ 0 ] + 1;

                auto index = latitude * divisions[ 0 ] + longitude;
                indices->setIndex( index * 6 + 0, first );
                indices->setIndex( index * 6 + 1, first + 1 );
                indices->setIndex( index * 6 + 2, second );
                indices->setIndex( index * 6 + 3, second );
                indices->setIndex( index * 6 + 4, first + 1 );
                indices->setIndex( index * 6 + 5, second + 1 );
            }
        }
        setIndices( indices );
    }
}
