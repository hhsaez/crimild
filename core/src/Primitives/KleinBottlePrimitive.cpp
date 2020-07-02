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

#include "KleinBottlePrimitive.hpp"

using namespace crimild;

KleinBottlePrimitive::KleinBottlePrimitive( const Params &params ) noexcept
    : ParametricPrimitive( { params.type, params.layout, params.colorMode } )
{
    _scale = params.scale;

    ParametricInterval interval = { params.divisions, Vector2f( Numericf::TWO_PI, Numericf::TWO_PI ), Vector2f( 15, 50 ) };
    setInterval( interval );
    generate();
}

Vector3f KleinBottlePrimitive::evaluate( const Vector2f &domain ) const
{
    float v = 1.0f - domain[ 0 ];
    float u = domain[ 1 ];
    float x0 = 3 * cos( u ) * ( 1 + sin( u ) ) + ( 2 * ( 1 - cos( u ) / 2 ) ) * cos( u ) * cos( v );
    float y0 = 8 * sin( u ) + ( 2 * ( 1 - cos( u ) / 2 ) ) * sin( u ) * cos( v );
    float x1 = 3 * cos( u ) * ( 1 + sin( u ) ) + ( 2 * ( 1 - cos( u ) / 2 ) ) * cos( v + Numericf::PI );
    float y1 = 8 * sin( u );

    Vector3f range;
    range[ 0 ] = u < Numericf::PI ? x0 : x1;
    range[ 1 ] = u < Numericf::PI ? -y0 : -y1;
    range[ 2 ] = ( -2 * ( 1 - cos( u ) / 2 ) ) * sin( v );
    return .1f * range * _scale;
}

bool KleinBottlePrimitive::InvertNormal( const Vector2f &domain ) const
{
    return domain[ 1 ] > 3 * Numericf::PI / 2;
}
