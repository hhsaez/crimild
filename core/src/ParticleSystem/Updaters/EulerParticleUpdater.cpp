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

#include "EulerParticleUpdater.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

using namespace crimild;

EulerParticleUpdater::EulerParticleUpdater( void )
	: _globalAcceleration( Vector3f::ZERO )
{

}

EulerParticleUpdater::~EulerParticleUpdater( void )
{

}

void EulerParticleUpdater::configure( Node *node, ParticleData *particles )
{
	_positions = particles->createAttribArray< Vector3f >( ParticleAttrib::POSITION );
	_velocities = particles->createAttribArray< Vector3f >( ParticleAttrib::VELOCITY );
	_accelerations = particles->createAttribArray< Vector3f >( ParticleAttrib::ACCELERATION );
}

void EulerParticleUpdater::update( Node *node, crimild::Real64 dt, ParticleData *particles )
{
	const auto count = particles->getAliveCount();

	const auto g = dt * _globalAcceleration;

	auto as = _accelerations->getData< Vector3f >();
	auto vs = _velocities->getData< Vector3f >();
	auto ps = _positions->getData< Vector3f >();

    crimild::Size itemsPerCacheLine = 64 / sizeof( crimild::Real32 );

	// TODO: all the accelerations are the same value
	// I think this could be optimized, but other
	// updaters may need separated values
	// Also, accelerations are handled in the same way
	// regardless of the computation space (world or local)
    for ( crimild::Size i = 0; i < count; i += itemsPerCacheLine ) {
        for ( crimild::Size j = i; j < std::min( count, i + itemsPerCacheLine ); j++ ) {
            as[ j ] += g;
        }
	}

	// Velocities are handled in the same way
	// regardless of the computation space (world or local)
    for ( crimild::Size i = 0; i < count; i += itemsPerCacheLine ) {
        for ( crimild::Size j = i; j < std::min( count, i + itemsPerCacheLine ); j++ ) {
            vs[ j ] += dt * as[ j ];
        }
	}
	
    for ( crimild::Size i = 0; i < count; i += itemsPerCacheLine ) {
        for ( crimild::Size j = i; j < std::min( count, i + itemsPerCacheLine ); j++ ) {
            ps[ j ] += dt * vs[ j ];
        }
	}
}

void EulerParticleUpdater::encode( coding::Encoder &encoder ) 
{
	ParticleSystemComponent::ParticleUpdater::encode( encoder );

	encoder.encode( "globalAcceleration", _globalAcceleration );
}

void EulerParticleUpdater::decode( coding::Decoder &decoder )
{
	ParticleSystemComponent::ParticleUpdater::decode( decoder );

	decoder.decode( "globalAcceleration", _globalAcceleration );
}

