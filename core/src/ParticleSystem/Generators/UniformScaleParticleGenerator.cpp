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

#include "UniformScaleParticleGenerator.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Crimild_Mathematics.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;

UniformScaleParticleGenerator::UniformScaleParticleGenerator( crimild::Real32 minScale, crimild::Real32 maxScale )
    : _minScale( minScale ),
      _maxScale( maxScale )
{
}

UniformScaleParticleGenerator::~UniformScaleParticleGenerator( void )
{
}

void UniformScaleParticleGenerator::configure( Node *node, ParticleData *particles )
{
    _scales = particles->createAttribArray< crimild::Real32 >( ParticleAttrib::UNIFORM_SCALE );
}

void UniformScaleParticleGenerator::generate( Node *node, double dt, ParticleData *particles, ParticleId startId, ParticleId endId )
{
    auto ss = _scales->getData< crimild::Real32 >();

    for ( ParticleId i = startId; i < endId; i++ ) {
        ss[ i ] = Random::generate< crimild::Real32 >( _minScale, _maxScale );
    }
}

void UniformScaleParticleGenerator::encode( coding::Encoder &encoder )
{
    ParticleSystemComponent::ParticleGenerator::encode( encoder );

    encoder.encode( "minScale", _minScale );
    encoder.encode( "maxScale", _maxScale );
}

void UniformScaleParticleGenerator::decode( coding::Decoder &decoder )
{
    ParticleSystemComponent::ParticleGenerator::decode( decoder );

    decoder.decode( "minScale", _minScale );
    decoder.decode( "maxScale", _maxScale );
}
