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

#include "TimeParticleUpdater.hpp"

using namespace crimild;

TimeParticleUpdater::TimeParticleUpdater( void )
{

}

TimeParticleUpdater::~TimeParticleUpdater( void )
{

}

void TimeParticleUpdater::configure( Node *node, ParticleData *particles )
{
	_times = particles->createAttribArray< crimild::Real32 >( ParticleAttrib::TIME );
	assert( _times != nullptr );
}

void TimeParticleUpdater::update( Node *node, double dt, ParticleData *particles )
{
	const auto count = particles->getAliveCount();

	auto ts = _times->getData< crimild::Real32 >();
	assert( ts != nullptr );

	for ( int i = 0; i < count; i++ ) {
		ts[ i ] -= dt;
		if ( ts[ i ] <= 0.0f ) {
			particles->kill( i );
		}
	}
}

void TimeParticleUpdater::encode( coding::Encoder &encoder ) 
{
	ParticleSystemComponent::ParticleUpdater::encode( encoder );
}

void TimeParticleUpdater::decode( coding::Decoder &decoder )
{
	ParticleSystemComponent::ParticleUpdater::decode( decoder );
}

