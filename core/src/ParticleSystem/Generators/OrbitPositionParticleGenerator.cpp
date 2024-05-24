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

#include "OrbitPositionParticleGenerator.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Crimild_Mathematics.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;

OrbitPositionParticleGenerator::OrbitPositionParticleGenerator( void )
    : OrbitPositionParticleGenerator( 1.0f, 0.1f, 0.1f, Vector3f::Constants::ONE )
{
}

OrbitPositionParticleGenerator::OrbitPositionParticleGenerator( crimild::Real32 radius, crimild::Real32 height, crimild::Real32 offset, const Vector3f &scale )
    : m_radius( radius ),
      m_height( height ),
      m_offset( offset ),
      m_scale( scale )
{
}

void OrbitPositionParticleGenerator::configure( Node *node, ParticleData *particles )
{
    m_positions = particles->createAttribArray< Vector3f >( ParticleAttrib::POSITION );
    assert( m_positions != nullptr );
}

void OrbitPositionParticleGenerator::generate( Node *node, crimild::Real64 dt, ParticleData *particles, ParticleId startId, ParticleId endId )
{
    auto ps = m_positions->getData< Vector3f >();
    auto radius = m_radius;
    auto offset = m_offset;
    auto height = m_height;

    auto count = endId - startId;
    if ( count == 0 ) {
        return;
    }

    auto randomOffset = []( crimild::Real32 offset, crimild::Real32 height ) {
        return Vector3f {
            Random::generate< crimild::Real32 >( -offset, offset ),
            height * Random::generate< crimild::Real32 >( -offset, offset ),
            Random::generate< crimild::Real32 >( -offset, offset ),
        };
    };

    for ( auto i = startId; i < endId; ++i ) {
        const auto theta = ( crimild::Real32 )( i - startId ) / ( crimild::Real32 ) count * Numericf::TWO_PI;
        const auto r = randomOffset( offset, height );
        const auto p = Vector3f {
            m_scale.x * Numericf::sin( theta ) * radius,
            0.0f,
            m_scale.z * Numericf::cos( theta ) * radius,
        };
        ps[ i ] = p + r;
    }
}

void OrbitPositionParticleGenerator::encode( coding::Encoder &encoder )
{
    ParticleSystemComponent::ParticleGenerator::encode( encoder );

    m_scale = Vector3f::Constants::ONE;

    encoder.encode( "radius", m_radius );
    encoder.encode( "height", m_height );
    encoder.encode( "offset", m_offset );
    encoder.encode( "scale", m_scale );
}

void OrbitPositionParticleGenerator::decode( coding::Decoder &decoder )
{
    ParticleSystemComponent::ParticleGenerator::decode( decoder );

    decoder.decode( "radius", m_radius );
    decoder.decode( "height", m_height );
    decoder.decode( "offset", m_offset );
    decoder.decode( "scale", m_scale );
}
