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

#include "ParametricSpherePrimitive.hpp"

using namespace crimild;

ParametricSpherePrimitive::ParametricSpherePrimitive( void ) noexcept
    : ParametricSpherePrimitive( Params {} )
{
}

ParametricSpherePrimitive::ParametricSpherePrimitive( const Params &params ) noexcept
    : ParametricPrimitive(
        {
            .type = params.type,
            .layout = params.layout,
            .colorMode = params.colorMode,
        }
    )
{
    _radius = params.radius;

    ParametricInterval interval = {
        .divisions = params.divisions,
        .upperBound = Vector2f { Numericf::PI, Numericf::TWO_PI },
        .textureCount = Vector2f { 20, 35 },
    };
    setInterval( interval );
    generate();
}

Vector3f ParametricSpherePrimitive::evaluate( const Vector2f &domain ) const
{
    float u = domain[ 0 ];
    float v = domain[ 1 ];
    float x = _radius * std::sin( u ) * std::cos( v );
    float y = _radius * std::cos( u );
    float z = _radius * -std::sin( u ) * std::sin( v );
    return Vector3f { x, y, z };
}
