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

#include "TrefoilKnotPrimitive.hpp"

using namespace crimild;

TrefoilKnotPrimitive::TrefoilKnotPrimitive( Primitive::Type type, float scale, const VertexFormat &format, Vector2i divisions )
    : ParametricPrimitive( type, format )
{
    _scale = scale;
    
    ParametricInterval interval = { divisions, Vector2f( Numericf::TWO_PI, Numericf::TWO_PI ), Vector2f( 100, 8 ) };
    setInterval( interval );
    generate();
}

TrefoilKnotPrimitive::~TrefoilKnotPrimitive( void )
{
    
}

Vector3f TrefoilKnotPrimitive::evaluate( const Vector2f &domain ) const
{
    const float a = 0.5f;
    const float b = 0.3f;
    const float c = 0.5f;
    const float d = 0.1f;
    float u = ( Numericf::TWO_PI - domain[ 0 ] ) * 2;
    float v = domain[ 1 ];
    
    float r = a + b * cos( 1.5f * u );
    float x = r * cos( u );
    float y = r * sin( u );
    float z = c * sin( 1.5f * u );
    
    Vector3f dv;
    dv[ 0 ] = -1.5f * b * sin( 1.5f * u ) * cos( u ) - ( a + b * cos( 1.5f * u ) ) * sin( u );
    dv[ 1 ] = -1.5f * b * sin( 1.5f * u ) * sin( u ) + ( a + b * cos( 1.5f * u ) ) * cos( u );
    dv[ 2 ] = 1.5f * c * cos( 1.5f * u );
    
    Vector3f q = dv.getNormalized();
    Vector3f qvn = Vector3f( q[ 1 ], -q[ 0 ], 0.0f).getNormalized();
    Vector3f ww = q ^ qvn;
    
    Vector3f range;
    range[ 0 ] = x + d * ( qvn[ 0 ] * cos( v ) + ww[ 0 ] * sin( v ) );
    range[ 1 ] = y + d * ( qvn[ 1 ] * cos( v ) + ww[ 1 ] * sin( v ) );
    range[ 2 ] = z + d * ww[ 2 ] * sin( v );
    return range * _scale;
}

