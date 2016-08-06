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

#include "ArcPrimitive.hpp"

#include <vector>

using namespace crimild;

ArcPrimitive::ArcPrimitive( float radius, float angle, float thickness, int segments )
    : Primitive( Primitive::Type::TRIANGLES )
{
    float theta = angle / segments;
    float c = crimild::Numericf::cos( -theta );
    float s = crimild::Numericf::sin( -theta );
    float t = 0.0f;

    float y0 = radius + thickness;
    float y1 = radius;
    float x0 = 0;
    float x1 = 0;

    std::vector< float > positions;

    for( int ii = 0; ii < segments + 1; ii++ ) {
        positions.push_back( x0 );
        positions.push_back( y0 );
        positions.push_back( 0.0f );

        positions.push_back( x1 );
        positions.push_back( y1 );
        positions.push_back( 0.0f );

        t = x0;
        x0 = c * x0 - s * y0;
        y0 = s * t + c * y0;
    
        t = x1;
        x1 = c * x1 - s * y1;
        y1 = s * t + c * y1;
    }

    auto vbo = crimild::alloc< crimild::VertexBufferObject >( crimild::VertexFormat::VF_P3, positions.size() / 3, &positions[ 0 ] );
    setVertexBuffer( vbo );
    
    std::vector< IndexPrecision > indices;
    for ( int i = 0; i < segments; i++ ) {
        auto i0 = i * 2;
        auto i1 = i * 2 + 1;
        auto i2 = ( i + 1 ) * 2 + 1;
        auto i3 = ( i + 1 ) * 2 + 0;

        indices.push_back( i0 );
        indices.push_back( i1 );
        indices.push_back( i2 );

        indices.push_back( i0 );
        indices.push_back( i2 );
        indices.push_back( i3 );
    }
    auto ibo = crimild::alloc< crimild::IndexBufferObject >( indices.size(), &indices[ 0 ] );
    setIndexBuffer( ibo );
}

ArcPrimitive::~ArcPrimitive( void )
{

}

