/*
 * Copyright (c) 2002 - present, H. Hernan Saez
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

#include "EulerAnglesParticleGenerator.hpp"

#include "Mathematics/Random.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;

EulerAnglesParticleGenerator::EulerAnglesParticleGenerator( void )
	: EulerAnglesParticleGenerator( 0.0f, Numericf::TWO_PI, Vector3f::ONE )
{

}

EulerAnglesParticleGenerator::EulerAnglesParticleGenerator( crimild::Real32 min, crimild::Real32 max, const Vector3f &axis )
	: m_min( min ),
	  m_max( max ),
	  m_axis( axis )
{
	
}

void EulerAnglesParticleGenerator::configure( Node *node, ParticleData *particles )
{
	m_angles = particles->createAttribArray< Vector3f >( ParticleAttrib::EULER_ANGLES );
	assert( m_angles != nullptr );
}

void EulerAnglesParticleGenerator::generate( Node *node, crimild::Real64 dt, ParticleData *particles, ParticleId startId, ParticleId endId )
{
	auto count = endId - startId;
	if ( count == 0 ) {
		return;
	}
    
	auto angles = m_angles->getData< Vector3f >();
	for ( auto i = startId; i < endId; ++i ) {
		auto angle = Random::generate< crimild::Real32 >( m_min, m_max );
		angles[ i ] = angle * m_axis;
	}
}

void EulerAnglesParticleGenerator::encode( coding::Encoder &encoder ) 
{
	ParticleSystemComponent::ParticleGenerator::encode( encoder );

	encoder.encode( "min", m_min );
	encoder.encode( "max", m_max );
	encoder.encode( "axis", m_axis );
}

void EulerAnglesParticleGenerator::decode( coding::Decoder &decoder )
{
	ParticleSystemComponent::ParticleGenerator::decode( decoder );

	decoder.decode( "min", m_min );
	decoder.decode( "max", m_max );
	decoder.decode( "axis", m_axis );
}
