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

#include "ParticleSystemComponent.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

using namespace crimild;

ParticleSystemComponent::ParticleSystemComponent( void )
{

}

ParticleSystemComponent::ParticleSystemComponent( ParticleDataPtr const &particles )
	: _particles( particles )
{

}

ParticleSystemComponent::ParticleSystemComponent( crimild::Size maxParticles )
	: _particles( crimild::alloc< ParticleData >( maxParticles ) )
{

}

ParticleSystemComponent::~ParticleSystemComponent( void )
{

}

void ParticleSystemComponent::start( void )
{
	auto particles = getParticles();
	assert( particles != nullptr );
	
	auto node = getNode();

	configureGenerators( node, particles );
	configureUpdaters( node, particles );
	configureRenderers( node, particles );

	_particles->generate();

	auto warmUp = _preWarmTime;
	while ( warmUp > 0.0 ) {
		updateGenerators( node, Clock::DEFAULT_TICK_TIME, particles );
		updateUpdaters( node, Clock::DEFAULT_TICK_TIME, particles );
		warmUp -= Clock::DEFAULT_TICK_TIME;
	}
}

void ParticleSystemComponent::configureGenerators( Node *node, ParticleData *particles )
{
	_generators.each( [ node, particles ]( SharedPointer< ParticleGenerator > &g, crimild::Size ) {
		g->configure( node, particles );
	});
}

void ParticleSystemComponent::configureUpdaters( Node *node, ParticleData *particles )
{
	_updaters.each( [ node, particles ]( SharedPointer< ParticleUpdater > &u, crimild::Size ) {
		u->configure( node, particles );
	});
}

void ParticleSystemComponent::configureRenderers( Node *node, ParticleData *particles )
{
	_renderers.each( [ node, particles ]( SharedPointer< ParticleRenderer > &r, crimild::Size ) {
		if ( r != nullptr ) {
			r->configure( node, particles );
		}
	});
}

void ParticleSystemComponent::update( const Clock &c )
{
    const auto dt = c.getDeltaTime();

	auto node = getNode();
	auto particles = getParticles();

	if ( isAnimationEnabled() ) {
		updateGenerators( node, dt, particles );
		updateUpdaters( node, dt, particles );
	}
	
	updateRenderers( node, dt, particles );
}

void ParticleSystemComponent::updateGenerators( Node *node, crimild::Real64 dt, ParticleData *particles )
{
	_emitAccum += _burst ? _emitRate : dt * _emitRate;
	if ( _emitAccum < 1.0 ) {
		return;
	}
	
    const ParticleId maxNewParticles = ( int ) _emitAccum;//_burst ? _emitRate : Numeric< ParticleId >::max( 1, dt * _emitRate );
	_emitAccum -= maxNewParticles;
	
    const ParticleId startId = particles->getAliveCount();            
    const ParticleId endId = Numeric< ParticleId >::min( startId + maxNewParticles, particles->getParticleCount() - 1 );

	_generators.each( [ node, dt, particles, startId, endId ]( SharedPointer< ParticleGenerator > &g ) {
		g->generate( node, dt, particles, startId, endId );
	});

    for ( ParticleId i = startId; i < endId; i++ ) {
        particles->wake( i );
    }
}

void ParticleSystemComponent::updateUpdaters( Node *node, crimild::Real64 dt, ParticleData *particles )
{
	_updaters.each( [ node, dt, particles ]( SharedPointer< ParticleUpdater > &u ) {
		u->update( node, dt, particles );
	});
}

void ParticleSystemComponent::updateRenderers( Node *node, crimild::Real64 dt, ParticleData *particles )
{
	_renderers.each( [ node, dt, particles ]( SharedPointer< ParticleRenderer > &r ) {
		r->update( node, dt, particles );
	});
}

void ParticleSystemComponent::encode( coding::Encoder &encoder ) 
{
	NodeComponent::encode( encoder );

	encoder.encode( "particles", _particles );
	encoder.encode( "emitRate", _emitRate );
	encoder.encode( "preWarmTime", _preWarmTime );
	encoder.encode( "burst", _burst );
	encoder.encode( "generators", _generators );
	encoder.encode( "updaters", _updaters );
	encoder.encode( "renderers", _renderers );
}

void ParticleSystemComponent::decode( coding::Decoder &decoder )
{
	NodeComponent::decode( decoder );

	decoder.decode( "particles", _particles );

	_emitRate = _particles->getParticleCount();
	decoder.decode( "emitRate", _emitRate );

	_preWarmTime = 0;
	decoder.decode( "preWarmTime", _preWarmTime );

	_burst = false;
	decoder.decode( "burst", _burst );

	decoder.decode( "generators", _generators );
	decoder.decode( "updaters", _updaters );
	decoder.decode( "renderers", _renderers );
}

