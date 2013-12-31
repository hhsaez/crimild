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

#ifndef CRIMILD_CORE_COMPONENTS_PARTICLE_SYSTEM_
#define CRIMILD_CORE_COMPONENTS_PARTICLE_SYSTEM_

#include "NodeComponent.hpp"
#include "Foundation/Macros.hpp"
#include "Mathematics/Vector.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/ShaderUniform.hpp"
#include "Rendering/ShaderUniformImpl.hpp"
#include "Boundings/BoundingVolume.hpp"

namespace crimild {

	class ParticleSystemComponent : public NodeComponent {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( ParticleSystemComponent );

	public:
		static const char *COMPONENT_NAME;

	public:
		ParticleSystemComponent( void );

		virtual ~ParticleSystemComponent( void );

		virtual void onAttach( void ) override;

		virtual void update( const Time &t ) override;

		void setParticleCount( unsigned short value ) { _particleCount = value; }
		unsigned short getParticleCount( void ) const { return _particleCount; }

		void setParticleSize( float value ) { _particleSize = value; }
		float getParticleSize( void ) const { return _particleSize; }

		void setParticleDuration( float value ) { _particleDuration = value; }
		float getParticleDuration( void ) const { return _particleDuration; }

		void setGravity( const Vector3f &value ) { _gravity = value; }
		const Vector3f &getGravity( void ) const { return _gravity; }

		void setSpread( const Vector3f &value ) { _spread = value; }
		const Vector3f &getSpread( void ) const { return _spread; }

		void setVelocity( const Vector3f &value ) { _velocity = value; }
		const Vector3f &getVelocity( void ) const { return _velocity; }

		void setLooping( float value ) { _looping = value; }
		float getLooping( void ) const { return _looping; }

		Material *getParticleMaterial( void ) { return _material; }

		void setShape( BoundingVolume *volume ) { _shape = volume; }
		BoundingVolume *getShape( void ) { return _shape; }

		void generateParticles( void );

	private:
		Pointer< Primitive > _primitive;
		Pointer< Material > _material;

		unsigned short _particleCount;
		float _particleSize;
		float _particleDuration;
		Vector3f _gravity;
		Vector3f _velocity;
		Vector3f _spread;
		bool _looping;
		Pointer< BoundingVolume > _shape;

		Pointer< Vector3fUniform > _gravityUniform;
		Pointer< FloatUniform > _timeUniform;
		Pointer< FloatUniform > _durationUniform;
		Pointer< FloatUniform > _shapeRadiusUniform;
		Pointer< Vector3fUniform > _shapeCenterUniform;
	};

}

#endif

