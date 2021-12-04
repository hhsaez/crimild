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

#ifndef CRIMILD_PARTICLE_GENERATOR_RANDOM_VALUE_
#define CRIMILD_PARTICLE_GENERATOR_RANDOM_VALUE_

#include "../ParticleSystemComponent.hpp"
#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"

#include <Mathematics/Random.hpp>

namespace crimild {

    template< typename T >
    class RandomValueParticleGenerator : public ParticleSystemComponent::ParticleGenerator {
    public:
        RandomValueParticleGenerator( void )
        {
        }

        explicit RandomValueParticleGenerator( const ParticleAttribType &type, const T &min, const T &max )
            : _attribType( type ),
              _minValue( min ),
              _maxValue( max )
        {
        }

        virtual ~RandomValueParticleGenerator( void )
        {
        }

        inline void setParticleAttribType( const ParticleAttribType &type ) { _attribType = type; }
        inline ParticleAttribType getParticleAttribType( void ) const { return _attribType; }

        inline void setMinValue( const T &value ) { _minValue = value; }
        inline const T &getMinValue( void ) const { return _minValue; }

        inline void setMaxValue( const T &value ) { _maxValue = value; }
        inline const T &getMaxValue( void ) const { return _maxValue; }

        virtual void configure( Node *node, ParticleData *particles ) override
        {
            _attribs = particles->createAttribArray< T >( _attribType );
        }

        virtual void generate( Node *node, crimild::Real64 dt, ParticleData *particles, ParticleId startId, ParticleId endId ) override
        {
            auto as = _attribs->getData< T >();

            for ( ParticleId i = startId; i < endId; i++ ) {
                as[ i ] = Random::generate< T >( _minValue, _maxValue );
            }
        }

    private:
        ::crimild::ParticleAttribType _attribType;
        T _minValue;
        T _maxValue;

        ParticleAttribArray *_attribs;

        /**
		 	\name Coding support
		*/
        //@{

    public:
        virtual void encode( coding::Encoder &encoder ) override
        {
            ParticleSystemComponent::ParticleGenerator::encode( encoder );

            std::string attribType;
            switch ( _attribType ) {
                case ParticleAttrib::UNIFORM_SCALE:
                    attribType = "uniformScale";
                    break;

                case ParticleAttrib::UNIFORM_SCALE_START:
                    attribType = "uniformScaleStart";
                    break;

                case ParticleAttrib::UNIFORM_SCALE_END:
                    attribType = "uniformScaleEnd";
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

            //encoder.encode( "minValue", _minValue );
            //encoder.encode( "maxValue", _maxValue );
        }

        virtual void decode( coding::Decoder &decoder ) override
        {
            ParticleSystemComponent::ParticleGenerator::decode( decoder );

            std::string attribType;
            decoder.decode( "attrib", attribType );
            if ( attribType == "uniformScale" ) {
                _attribType = ParticleAttrib::UNIFORM_SCALE;
            } else if ( attribType == "uniformScaleStart" ) {
                setParticleAttribType( ParticleAttrib::UNIFORM_SCALE_START );
            } else if ( attribType == "uniformScaleEnd" ) {
                setParticleAttribType( ParticleAttrib::UNIFORM_SCALE_END );
            } else if ( attribType == "position" ) {
                setParticleAttribType( ParticleAttrib::POSITION );
            } else if ( attribType == "velocity" ) {
                setParticleAttribType( ParticleAttrib::VELOCITY );
            } else if ( attribType == "acceleration" ) {
                setParticleAttribType( ParticleAttrib::ACCELERATION );
            }

            //decoder.decode( "minValue", _minValue );
            //decoder.decode( "maxValue", _maxValue );
        }

        //@}
    };

    class RandomVector3fParticleGenerator : public RandomValueParticleGenerator< Vector3f > {
        CRIMILD_IMPLEMENT_RTTI( crimild::RandomVector3fParticleGenerator )

    public:
        RandomVector3fParticleGenerator( void ) = default;
        RandomVector3fParticleGenerator( const ::crimild::ParticleAttribType &type, const Vector3f &min, const Vector3f &max )
            : RandomValueParticleGenerator( type, min, max )
        {
        }

        virtual ~RandomVector3fParticleGenerator( void ) = default;
    };

    class RandomColorRGBAParticleGenerator : public RandomValueParticleGenerator< ColorRGBA > {
        CRIMILD_IMPLEMENT_RTTI( crimild::RandomColorRGBAParticleGenerator )

    public:
        RandomColorRGBAParticleGenerator( void ) { }
        virtual ~RandomColorRGBAParticleGenerator( void ) { }
    };

    class RandomReal32ParticleGenerator : public RandomValueParticleGenerator< crimild::Real32 > {
        CRIMILD_IMPLEMENT_RTTI( crimild::RandomReal32ParticleGenerator )

    public:
        RandomReal32ParticleGenerator( void ) { }
        virtual ~RandomReal32ParticleGenerator( void ) { }
    };

}

#endif
