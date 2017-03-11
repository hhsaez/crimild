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

#ifndef CRIMILD_PARTICLE_GENERATOR_DEFAULT_VALUE_
#define CRIMILD_PARTICLE_GENERATOR_DEFAULT_VALUE_

#include "../ParticleSystemComponent.hpp"

namespace crimild {

	template< typename T >
    class DefaultValueParticleGenerator : public ParticleSystemComponent::ParticleGenerator {
    public:
        DefaultValueParticleGenerator( void )
		{

		}

		explicit DefaultValueParticleGenerator( const ParticleAttribType &type, const T &value )
			: _attribType( type ),
			  _value( value )
		{

		}
		
        virtual ~DefaultValueParticleGenerator( void )
		{

		}

		inline void setParticleAttribType( const ParticleAttribType &type ) { _attribType = type; }
		inline const ParticleAttribType &ParticleAttribType( void ) const { return _attribType; }

		inline void setValue( const T &value ) { _value = value; }
		inline const T &getValue( void ) { return _value; }

		virtual void configure( Node *node, ParticleData *particles ) override
		{
			_attribs = particles->createAttribArray< T >( _attribType );
		}

        virtual void generate( Node *node, crimild::Real64 dt, ParticleData *particles, ParticleId startId, ParticleId endId ) override
		{
			auto as = _attribs->getData< T >();

			const auto v = _value;
			
			for ( ParticleId i = startId; i < endId; i++ ) {
				as[ i ] = v;
			}
		}

    private:
		crimild::ParticleAttribType _attribType;
		T _value;

		ParticleAttribArray *_attribs;
    };

	using DefaultVector3fParticleGenerator = DefaultValueParticleGenerator< Vector3f >;
	using DefaultRGBAColorfParticleGenerator = DefaultValueParticleGenerator< RGBAColorf >;
	using DefaultReal32ParticleGenerator = DefaultValueParticleGenerator< crimild::Real32 >;

}

#endif

