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

#include "AttractorParticleUpdater.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Crimild_Mathematics.hpp"

using namespace crimild;

AttractorParticleUpdater::AttractorParticleUpdater( void )
    : _attractor { Point3f::Constants::ZERO, 1.0f },
      _strength( 1.0f )
{
}

AttractorParticleUpdater::~AttractorParticleUpdater( void )
{
}

void AttractorParticleUpdater::configure( Node *node, ParticleData *particles )
{
    _positions = particles->createAttribArray< Vector3f >( ParticleAttrib::POSITION );
    _accelerations = particles->createAttribArray< Vector3f >( ParticleAttrib::ACCELERATION );
}

void AttractorParticleUpdater::update( Node *node, crimild::Real64 dt, ParticleData *particles )
{
    /*
    const auto center = crimild::center( _attractor );
    const auto radius = crimild::radius( _attractor );
    const auto count = particles->getAliveCount();

    const auto ps = _positions->getData< Vector3f >();
    auto as = _accelerations->getData< Vector3f >();

    for ( crimild::Size i = 0; i < count; i++ ) {
        const auto p = ps[ i ];

        auto direction = Vector3( center - p );
        auto d = length( direction );
        if ( d > 0.0 && d <= radius ) {
            //direction /= d;
            const auto pct = 1.0 - ( d / radius );

            //as[ i ] += dt * pct * _strength * direction;
        }
    }
    */
    assert( false );
}

void AttractorParticleUpdater::encode( coding::Encoder &encoder )
{
    ParticleSystemComponent::ParticleUpdater::encode( encoder );

    // encoder.encode( "origin", _attractor.getCenter() );
    // encoder.encode( "radius", _attractor.getRadius() );
    // encoder.encode( "strengh", _strength );
}

void AttractorParticleUpdater::decode( coding::Decoder &decoder )
{
    ParticleSystemComponent::ParticleUpdater::decode( decoder );

    Vector3f origin = Vector3f::Constants::ZERO;
    decoder.decode( "origin", origin );

    crimild::Real32 radius = 1.0f;
    decoder.decode( "radius", radius );

    crimild::Real32 strength = 1.0f;
    decoder.decode( "strength", strength );

    setAttractor( Sphere { Point3f( origin ), radius } );
    setStrength( strength );
}
