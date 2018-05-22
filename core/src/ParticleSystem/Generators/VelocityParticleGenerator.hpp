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

#ifndef CRIMILD_PARTICLE_GENERATOR_VELOCITY_
#define CRIMILD_PARTICLE_GENERATOR_VELOCITY_

#include "../ParticleSystemComponent.hpp"

namespace crimild {

    class VelocityParticleGenerator : public ParticleSystemComponent::ParticleGenerator {
        CRIMILD_IMPLEMENT_RTTI( crimild::VelocityParticleGenerator )

    public:
        VelocityParticleGenerator( void );
        virtual ~VelocityParticleGenerator( void );

        inline void setMinVelocity( const Vector3f &value ) { _minVelocity = value; }
        inline const Vector3f &getMinVelocity( void ) const { return _minVelocity; }

        inline void setMaxVelocity( const Vector3f &value ) { _maxVelocity = value; }
        inline const Vector3f &getMaxVelocity( void ) const { return _maxVelocity; }

		virtual void configure( Node *node, ParticleData *particles ) override;
        virtual void generate( Node *node, double dt, ParticleData *particles, ParticleId startId, ParticleId endId ) override;

    private:
        Vector3f _minVelocity;
        Vector3f _maxVelocity;

		ParticleAttribArray *_velocities = nullptr;

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

