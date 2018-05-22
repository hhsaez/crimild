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

#ifndef CRIMILD_PARTICLE_UPDATER_SET_ATTRIB_VALUE_
#define CRIMILD_PARTICLE_UPDATER_SET_ATTRIB_VALUE_

#include "../ParticleSystemComponent.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

namespace crimild {

	/**
	   \brief Set a constant value for an attribute whenver its updated

	   \remarks Useful for reseting values
	 */
	template< typename T >
    class SetAttribValueParticleUpdater : public ParticleSystemComponent::ParticleUpdater {
    public:
        SetAttribValueParticleUpdater( void )
		{
			
		}
		
        virtual ~SetAttribValueParticleUpdater( void )
		{

		}

		inline void setAttribType( const ParticleAttribType &type ) { _attribType = type; }
		inline const ParticleAttribType &getAttribType( void ) { return _attribType; }

		inline void setValue( const T &value ) { _value = value; }
		inline const T &getValue( void ) const { return _value; }

		virtual void configure( Node *node, ParticleData *particles ) override
		{
			_attribData = particles->createAttribArray< T >( _attribType );
		}
		
        virtual void update( Node *node, crimild::Real64 dt, ParticleData *particles ) override
		{
			const auto count = particles->getAliveCount();

			auto as = _attribData->getData< T >();

			for ( int i = 0; i < count; i++ ) {
				as[ i ] = _value;
			}
		}

	private:
		ParticleAttribType _attribType;
		T _value;
		
		ParticleAttribArray *_attribData = nullptr;

		/** 
		 	\name Coding support
		*/
		//@{

	public:
		virtual void encode( coding::Encoder &encoder ) override
		{
			ParticleSystemComponent::ParticleUpdater::encode( encoder );

			std::string attribType;
			switch ( _attribType ) {
				case ParticleAttrib::UNIFORM_SCALE:
					attribType = "uniformScale";
					break;

				case ParticleAttrib::POSITION:
					attribType = "position";
					break;

				case ParticleAttrib::VELOCITY:
					attribType = "velocity";
					break;

				case ParticleAttrib::ACCELERATION:
					attribType = "acceleration";
					break;
					
				default:
					break;
			}
			encoder.encode( "attrib", attribType );

			encoder.encode( "value", _value );
		}

		virtual void decode( coding::Decoder &decoder ) override
		{
			ParticleSystemComponent::ParticleUpdater::decode( decoder );

			std::string attribType;
			decoder.decode( "attrib", attribType );
			if ( attribType == "uniformScale" ) {
				_attribType = ParticleAttrib::UNIFORM_SCALE;
			}
			else if ( attribType == "position" ) {
				_attribType = ParticleAttrib::POSITION;
			}
			else if ( attribType == "velocity" ) {
				_attribType = ParticleAttrib::VELOCITY;
			}
			else if ( attribType == "acceleration" ) {
				_attribType = ParticleAttrib::ACCELERATION;
			}
			
			decoder.decode( "value", _value );
		}

		//@}
        
    };

	class SetVector3fValueParticleUpdater : public SetAttribValueParticleUpdater< Vector3f > {
		CRIMILD_IMPLEMENT_RTTI( crimild::SetVector3fValueParticleUpdater )

	public:
		SetVector3fValueParticleUpdater( void ) { }
		virtual ~SetVector3fValueParticleUpdater( void ) { }
	};

	class SetRGBAColorValueParticleUpdater : public SetAttribValueParticleUpdater< RGBAColorf > {
		CRIMILD_IMPLEMENT_RTTI( crimild::SetRGBAColorValueParticleUpdater )

	public:
		SetRGBAColorValueParticleUpdater( void ) { }
		virtual ~SetRGBAColorValueParticleUpdater( void ) { }
	};

	class SetReal32ValueParticleUpdater : public SetAttribValueParticleUpdater< crimild::Real32 > {
		CRIMILD_IMPLEMENT_RTTI( crimild::SetReal32ValueParticleUpdater )

	public:
		SetReal32ValueParticleUpdater( void ) { }
		virtual ~SetReal32ValueParticleUpdater( void ) { }
	};

}

#endif

