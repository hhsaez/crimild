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

#ifndef CRIMILD_PARTICLE_GENERATOR_POSITION_ORBIT_
#define CRIMILD_PARTICLE_GENERATOR_POSITION_ORBIT_

#include "ParticleSystem/ParticleSystemComponent.hpp"

namespace crimild {

    class OrbitPositionParticleGenerator : public ParticleSystemComponent::ParticleGenerator {
        CRIMILD_IMPLEMENT_RTTI( crimild::OrbitPositionParticleGenerator )
		
    public:
        OrbitPositionParticleGenerator( void );
		OrbitPositionParticleGenerator( crimild::Real32 radius, crimild::Real32 height, crimild::Real32 offset, const Vector3f &scale );
		~OrbitPositionParticleGenerator( void ) = default;

		virtual void configure( Node *node, ParticleData *particles ) override;
        virtual void generate( Node *node, crimild::Real64 dt, ParticleData *particles, ParticleId startId, ParticleId endId ) override;

    private:
		crimild::Real32 m_radius;
		crimild::Real32 m_height;
		crimild::Real32 m_offset;
		Vector3f m_scale;

		ParticleAttribArray *m_positions = nullptr;
        
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

