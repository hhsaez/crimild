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

using namespace crimild;

ParametricPrimitive::ParametricPrimitive( Primitive::Type type, const VertexFormat &format )
    : Primitive( type ),
      _format( format )
{
}

ParametricPrimitive::~ParametricPrimitive( void )
{
    
}

void ParametricPrimitive::setInterval( const ParametricInterval &interval )
{
    _upperBound = interval.upperBound;
    _divisions = interval.divisions;
    _slices = _divisions - Vector2i( 1, 1 );
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
    return Vector2f( x * _upperBound[ 0 ] / _slices[ 0 ], y * _upperBound[ 1 ] / _slices[ 1 ] );
}

void ParametricPrimitive::generate( void )
{
    generateVertexBuffer();
    if ( getType() == Primitive::Type::LINES ) {
        generateLineIndexBuffer();
    }
    else {
        generateTriangleIndexBuffer();
    }
}

void ParametricPrimitive::generateVertexBuffer( void )
{
    auto vbo = crimild::alloc< VertexBufferObject >( _format, getVertexCount(), nullptr );
    VertexPrecision *vertex = vbo->data();
    for ( int i = 0; i < _divisions[ 1 ]; i++ ) {
        for ( int j = 0; j < _divisions[ 0 ]; j++ ) {
            // compute position
            Vector2f domain = computeDomain( j, i );
            Vector3f range = evaluate( domain );
            vertex[ _format.getPositionsOffset() + 0 ] = range[ 0 ];
            vertex[ _format.getPositionsOffset() + 1 ] = range[ 1 ];
            vertex[ _format.getPositionsOffset() + 2 ] = range[ 2 ];
            
            // compute normal
            if ( _format.hasNormals() ) {
                float s = j;
                float t = i;
                
                // nudge the point if the normal is indeterminate
                if ( j == 0 ) s += 0.01f;
                if ( j == _divisions[ 0 ] - 1 ) s -= 0.01f;
                if ( i == 0 ) t += 0.01f;
                if ( i == _divisions[ 1 ] - 1 ) t -= 0.01f;
                
                // compute the tangents and their cross product
                Vector3f p = evaluate( computeDomain( s, t ) );
                Vector3f u = evaluate( computeDomain( s + 0.01f, t ) ) - p;
                Vector3f v = evaluate( computeDomain( s, t + 0.01f ) ) - p;
                Vector3f normal = u ^ v;
                normal.normalize();
                if ( invertNormal( domain ) ) {
                    normal -= normal;
                }
                vertex[ _format.getNormalsOffset() + 0 ] = normal[ 0 ];
                vertex[ _format.getNormalsOffset() + 1 ] = normal[ 1 ];
                vertex[ _format.getNormalsOffset() + 2 ] = normal[ 2 ];
            }
            
            // compute texture coordinates
            if ( _format.hasTextureCoords() ) {
                float s = ( float ) _textureCount[ 0 ] * ( float ) j / ( float ) _slices[ 0 ];
                float t = ( float ) _textureCount[ 1 ] * ( float ) i / ( float ) _slices[ 1 ];
                vertex[ _format.getTextureCoordsOffset() + 0 ] = s;
                vertex[ _format.getTextureCoordsOffset() + 1 ] = t;
            }
            vertex += _format.getVertexSize();
        }
    }
    setVertexBuffer( vbo );
}

void ParametricPrimitive::generateLineIndexBuffer( void )
{
    auto ibo = crimild::alloc< IndexBufferObject >( getLineIndexCount(), nullptr );
    IndexPrecision *index = ibo->data();
    for ( int i = 0, vertex = 0; i < _slices[ 1 ]; i++ ) {
        for ( int j = 0; j < _slices[ 0 ]; j++ ) {
            int next = ( j + 1 ) % _divisions[ 0 ];
            *index++ = vertex + j;
            *index++ = vertex + next;
            *index++ = vertex + j;
            *index++ = vertex + j + _divisions[ 0 ];
        }
        vertex += _divisions[ 0 ];
    }
    setIndexBuffer( ibo );
}

void ParametricPrimitive::generateTriangleIndexBuffer( void )
{
    auto ibo = crimild::alloc< IndexBufferObject >( getTriangleIndexCount(), nullptr );
    IndexPrecision *index = ibo->data();
    for ( int i = 0, vertex = 0; i < _slices[ 1 ]; i++ ) {
        for ( int j = 0; j < _slices[ 0 ]; j++ ) {
            int next = ( j + 1 ) % _divisions[ 0 ];
            *index++ = vertex + j;
            *index++ = vertex + next;
            *index++ = vertex + j + _divisions[ 0 ];
            *index++ = vertex + next;
            *index++ = vertex + next + _divisions[ 0 ];
            *index++ = vertex + j + _divisions[ 0 ];
        }
        vertex += _divisions[ 0 ];
    }
    
    setIndexBuffer( ibo );
}

