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

#include "ParametricPrimitive.hpp"

#include "Crimild_Mathematics.hpp"

using namespace crimild;

ParametricPrimitive::ParametricPrimitive( const Params &params ) noexcept
    : Primitive( params.type ),
      _layout( params.layout ),
      _colorMode( params.colorMode )
{
}

void ParametricPrimitive::setInterval( const ParametricInterval &interval )
{
    _upperBound = interval.upperBound;
    _divisions = interval.divisions;
    _slices = _divisions - Vector2i::Constants::ONE;
    _textureCount = interval.textureCount;
}

int ParametricPrimitive::getVertexCount( void ) const
{
    return _divisions[ 0 ] * _divisions[ 1 ];
}

int ParametricPrimitive::getLineIndexCount( void ) const
{
    return 4 * _slices[ 0 ] * _slices[ 1 ];
}

int ParametricPrimitive::getTriangleIndexCount( void ) const
{
    return 6 * _slices[ 0 ] * _slices[ 1 ];
}

Vector2f ParametricPrimitive::computeDomain( float x, float y ) const
{
    return Vector2f {
        x * _upperBound[ 0 ] / _slices[ 0 ],
        y * _upperBound[ 1 ] / _slices[ 1 ],
    };
}

void ParametricPrimitive::generate( void )
{
    generateVertexBuffer();
    if ( getType() == Primitive::Type::LINES ) {
        generateLineIndexBuffer();
    } else {
        generateTriangleIndexBuffer();
    }
}

void ParametricPrimitive::generateVertexBuffer( void )
{
    auto layout = _layout;

    auto vertices = crimild::alloc< VertexBuffer >( layout, getVertexCount() );
    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    auto colors = layout.hasAttribute( VertexAttribute::Name::COLOR ) ? vertices->get( VertexAttribute::Name::COLOR ) : nullptr;
    auto normals = layout.hasAttribute( VertexAttribute::Name::NORMAL ) ? vertices->get( VertexAttribute::Name::NORMAL ) : nullptr;
    auto texCoords = layout.hasAttribute( VertexAttribute::Name::TEX_COORD ) ? vertices->get( VertexAttribute::Name::TEX_COORD ) : nullptr;

    for ( auto i = 0l; i < _divisions[ 1 ]; i++ ) {
        for ( auto j = 0l; j < _divisions[ 0 ]; j++ ) {
            auto domain = computeDomain( j, i );
            auto range = evaluate( domain );

            auto vIdx = i * _divisions[ 0 ] + j;

            positions->set( vIdx, range );

            if ( normals != nullptr ) {
                float s = j;
                float t = i;

                // nudge the point if the normal is indeterminate
                if ( j == 0 )
                    s += 0.01f;
                if ( j == _divisions[ 0 ] - 1 )
                    s -= 0.01f;
                if ( i == 0 )
                    t += 0.01f;
                if ( i == _divisions[ 1 ] - 1 )
                    t -= 0.01f;

                // compute the tangents and their cross product
                Vector3f p = evaluate( computeDomain( s, t ) );
                Vector3f u = evaluate( computeDomain( s + 0.01f, t ) ) - p;
                Vector3f v = evaluate( computeDomain( s, t + 0.01f ) ) - p;
                Vector3f normal = normalize( cross( u, v ) );
                if ( invertNormal( domain ) ) {
                    normal = -normal;
                }

                normals->set( vIdx, normal );
            }

            if ( colors != nullptr ) {
                auto color = ColorRGB::Constants::WHITE;
                if ( _colorMode.type == ColorMode::Type::CONSTANT ) {
                    color = _colorMode.color;
                } else if ( _colorMode.type == ColorMode::Type::POSITIONS ) {
                    color = 0.5f * ( ColorRGB::Constants::WHITE + ColorRGB( normalize( range ) ) );
                }
                colors->set( vIdx, color );
            }

            if ( texCoords != nullptr ) {
                auto s = ( float ) _textureCount[ 0 ] * ( float ) j / ( float ) _slices[ 0 ];
                auto t = ( float ) _textureCount[ 1 ] * ( float ) i / ( float ) _slices[ 1 ];
                texCoords->set( vIdx, Vector2f { s, t } );
            }
        }
    }

    setVertexData( { vertices } );
}

void ParametricPrimitive::generateLineIndexBuffer( void )
{
    auto indices = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, getLineIndexCount() );
    UInt32 index = 0;
    for ( UInt32 i = 0, vertex = 0l; i < _slices[ 1 ]; i++ ) {
        for ( UInt32 j = 0; j < _slices[ 0 ]; j++ ) {
            UInt32 next = ( j + 1 ) % _divisions[ 0 ];
            indices->setIndex( index++, vertex + j );
            indices->setIndex( index++, vertex + next );
            indices->setIndex( index++, vertex + j );
            indices->setIndex( index++, vertex + j + _divisions[ 0 ] );
        }
        vertex += _divisions[ 0 ];
    }

    setIndices( indices );
}

void ParametricPrimitive::generateTriangleIndexBuffer( void )
{
    auto indices = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, getTriangleIndexCount() );
    UInt32 index = 0;
    for ( UInt32 i = 0, vertex = 0l; i < _slices[ 1 ]; i++ ) {
        for ( UInt32 j = 0; j < _slices[ 0 ]; j++ ) {
            UInt32 next = ( j + 1 ) % _divisions[ 0 ];
            indices->setIndex( index++, vertex + j );
            indices->setIndex( index++, vertex + next );
            indices->setIndex( index++, vertex + j + _divisions[ 0 ] );

            indices->setIndex( index++, vertex + next );
            indices->setIndex( index++, vertex + next + _divisions[ 0 ] );
            indices->setIndex( index++, vertex + j + _divisions[ 0 ] );
        }
        vertex += _divisions[ 0 ];
    }

    setIndices( indices );
}
