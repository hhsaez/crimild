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

#include "RotationComponent.hpp"

#include "Mathematics/Numeric.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/UpdateWorldState.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

using namespace crimild;

RotationComponent::RotationComponent( void )
{

}

RotationComponent::RotationComponent( const Vector3f &axis, float speed )
	: _axis( axis.getNormalized() ),
	  _speed( speed ),
	  _time( 0 )
{

}

RotationComponent::~RotationComponent( void )
{

}

void RotationComponent::update( const Clock &c )
{
	getNode()->local().rotate().fromAxisAngle( _axis, _time * 2.0f * Numericf::PI );
	_time += _speed * c.getDeltaTime();
}

void RotationComponent::encode( coding::Encoder &encoder )
{
    NodeComponent::encode( encoder );

    encoder.encode( "axis", _axis );
    encoder.encode( "speed", _speed );
}

void RotationComponent::decode( coding::Decoder &decoder )
{
    NodeComponent::decode( decoder );

    decoder.decode( "axis", _axis );
    decoder.decode( "speed", _speed );
}
