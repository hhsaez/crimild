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

#ifndef CRIMILD_PARTICLE_GENERATOR_POSITION_BOX_
#define CRIMILD_PARTICLE_GENERATOR_POSITION_BOX_

#include "Crimild_Mathematics.hpp"
#include "ParticleSystem/ParticleSystemComponent.hpp"

namespace crimild {

    class BoxPositionParticleGenerator : public ParticleSystemComponent::ParticleGenerator {
        CRIMILD_IMPLEMENT_RTTI( crimild::BoxPositionParticleGenerator )
    public:
        struct Params {
            Vector3f origin = Vector3f::Constants::ZERO;
            Vector3f size = Vector3f::Constants::ONE;
        };

    public:
        BoxPositionParticleGenerator( void ) = default;

        explicit BoxPositionParticleGenerator( const Vector3f &origin, const Vector3f &size ) noexcept
            : _origin( origin ),
              _size( size )
        {
            // no-op
        }

        virtual ~BoxPositionParticleGenerator( void ) = default;

        inline void setOrigin( const Vector3f &origin ) { _origin = origin; }
        inline const Vector3f &getOrigin( void ) const { return _origin; }

        inline void setSize( const Vector3f &size ) { _size = size; }
        inline const Vector3f &getSize( void ) const { return _size; }

        virtual void configure( Node *node, ParticleData *particles ) override;
        virtual void generate( Node *node, crimild::Real64 dt, ParticleData *particles, ParticleId startId, ParticleId endId ) override;

    private:
        Params m_params;
        Vector3f _origin;
        Vector3f _size;

        ParticleAttribArray *_positions = nullptr;

        /**
                        \name Coding support
                */
        //@{

    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
    };

}

#endif
