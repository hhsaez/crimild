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

#include "UniformScaleParticleUpdater.hpp"

#include "Crimild_Mathematics.hpp"

using namespace crimild;

UniformScaleParticleUpdater::UniformScaleParticleUpdater( void )
{
}

UniformScaleParticleUpdater::~UniformScaleParticleUpdater( void )
{
}

void UniformScaleParticleUpdater::configure( Node *node, ParticleData *particles )
{
    _startScales = particles->createAttribArray< crimild::Real32 >( ParticleAttrib::UNIFORM_SCALE_START );
    _endScales = particles->createAttribArray< crimild::Real32 >( ParticleAttrib::UNIFORM_SCALE_END );
    _scales = particles->createAttribArray< crimild::Real32 >( ParticleAttrib::UNIFORM_SCALE );
    _times = particles->createAttribArray< crimild::Real32 >( ParticleAttrib::TIME );
    _lifetimes = particles->createAttribArray< crimild::Real32 >( ParticleAttrib::LIFE_TIME );
}

void UniformScaleParticleUpdater::update( Node *node, crimild::Real64 dt, ParticleData *particles )
{
    const auto count = particles->getAliveCount();

    auto startData = _startScales->getData< crimild::Real32 >();
    auto endData = _endScales->getData< crimild::Real32 >();
    auto scaleData = _scales->getData< crimild::Real32 >();
    auto timeData = _times->getData< crimild::Real32 >();
    auto lifetimeData = _lifetimes->getData< crimild::Real32 >();

    for ( crimild::Size i = 0; i < count; i++ ) {
        const auto s0 = startData[ i ];
        const auto s1 = endData[ i ];

        const auto t = 1.0f - ( timeData[ i ] / lifetimeData[ i ] );

        crimild::Real32 s;
        Interpolation::linear( s0, s1, t, s );
        scaleData[ i ] = s;
    }
}

void UniformScaleParticleUpdater::encode( coding::Encoder &encoder )
{
    ParticleSystemComponent::ParticleUpdater::encode( encoder );
}

void UniformScaleParticleUpdater::decode( coding::Decoder &decoder )
{
    ParticleSystemComponent::ParticleUpdater::decode( decoder );
}
