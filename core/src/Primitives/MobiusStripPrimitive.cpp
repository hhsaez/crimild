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

#include "MobiusStripPrimitive.hpp"

using namespace crimild;

MobiusStripPrimitive::MobiusStripPrimitive( Primitive::Type type, float scale, const VertexFormat &format, Vector2i divisions )
    : ParametricPrimitive( type, format )
{
    _scale = scale;
    
    ParametricInterval interval = { divisions, Vector2f( Numericf::TWO_PI, Numericf::TWO_PI ), Vector2f( 40, 15 ) };
    setInterval( interval );
    generate();
}

MobiusStripPrimitive::~MobiusStripPrimitive( void )
{
    
}

Vector3f MobiusStripPrimitive::evaluate( const Vector2f &domain ) const
{
    float u = domain[ 0 ];
    float t = domain[ 1 ];
    float major = 1.25f;
    float a = 0.125f;
    float b = 0.5f;
    float phi = u / 2.0f;
    
    // general equation for an ellipse where phi is the angle
    // between the major axis and the x axis
    float x = a * cos( t ) * cos( phi ) - b * sin( t ) * sin( phi );
    float y = a * cos( t ) * sin( phi ) + b * sin( t ) * cos( phi );
    
    // sweep the ellipse along a circle, like a torus
    Vector3f range;
    range[ 0 ] = ( major + x ) * cos( u );
    range[ 1 ] = ( major + x ) * sin( u );
    range[ 2 ] = y;
    return range * _scale;
}

