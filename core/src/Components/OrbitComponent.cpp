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

#include "OrbitComponent.hpp"

#include "Mathematics/Numeric.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;

OrbitComponent::OrbitComponent( float x0, float y0, float major, float minor, float speed, float gamma )
{
	_t = 0.0f;
	_x0 = x0;
	_y0 = y0;
	_major = major;
	_minor = minor;
	_speed = speed;
	_gamma = gamma;
}

OrbitComponent::~OrbitComponent( void )
{

}

void OrbitComponent::update( const Clock &c )
{
    getNode()->local().translate()[0] = _x0 + _major * std::cos( _t ) * std::cos( _gamma ) - _minor * std::sin( _t ) * std::sin( _gamma );
    getNode()->local().translate()[1] = _y0 + _major * std::cos( _t ) * std::sin( _gamma ) + _minor * std::sin( _t ) * std::cos( _gamma );
	
	_t += _speed * c.getDeltaTime();
}

void OrbitComponent::encode( coding::Encoder &encoder )
{
	NodeComponent::encode( encoder );

	encoder.encode( "x0", _x0 );
	encoder.encode( "y0", _y0 );
	encoder.encode( "major", _major );
	encoder.encode( "minor", _minor );
	encoder.encode( "speed", _speed );
	encoder.encode( "gamma", _gamma );
}

void OrbitComponent::decode( coding::Decoder &decoder )
{
	NodeComponent::decode( decoder );

	decoder.decode( "x0", _x0 );
	decoder.decode( "y0", _y0 );
	decoder.decode( "major", _major );
	decoder.decode( "minor", _minor );
	decoder.decode( "speed", _speed );
	decoder.decode( "gamma", _gamma );
}

