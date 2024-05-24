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
#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Crimild_Mathematics.hpp"

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
        inline ParticleAttribType getParticleAttribType( void ) const { return _attribType; }

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

            // encoder.encode( "value", _value );
        }

        virtual void decode( coding::Decoder &decoder ) override
        {
            ParticleSystemComponent::ParticleGenerator::decode( decoder );

            std::string attribType;
            decoder.decode( "attrib", attribType );
            if ( attribType == "uniformScale" ) {
                _attribType = ParticleAttrib::UNIFORM_SCALE;
            } else if ( attribType == "position" ) {
                setParticleAttribType( ParticleAttrib::POSITION );
            } else if ( attribType == "velocity" ) {
                setParticleAttribType( ParticleAttrib::VELOCITY );
            } else if ( attribType == "acceleration" ) {
                setParticleAttribType( ParticleAttrib::ACCELERATION );
            }

            // decoder.decode( "value", _value );
        }

        //@}
    };

    class DefaultVector3fParticleGenerator : public DefaultValueParticleGenerator< Vector3f > {
        CRIMILD_IMPLEMENT_RTTI( crimild::DefaultVector3fParticleGenerator )

    public:
        DefaultVector3fParticleGenerator( void ) { }
        virtual ~DefaultVector3fParticleGenerator( void ) { }
    };

    class DefaultColorRGBAParticleGenerator : public DefaultValueParticleGenerator< ColorRGBA > {
        CRIMILD_IMPLEMENT_RTTI( crimild::DefaultColorRGBAParticleGenerator )

    public:
        DefaultColorRGBAParticleGenerator( void ) { }
        virtual ~DefaultColorRGBAParticleGenerator( void ) { }
    };

    class DefaultColorRGBParticleGenerator : public DefaultValueParticleGenerator< ColorRGB > {
        CRIMILD_IMPLEMENT_RTTI( crimild::DefaultColorRGBAParticleGenerator )
    };

    class DefaultReal32ParticleGenerator : public DefaultValueParticleGenerator< crimild::Real32 > {
        CRIMILD_IMPLEMENT_RTTI( crimild::DefaultReal32ParticleGenerator )

    public:
        DefaultReal32ParticleGenerator( void ) { }
        virtual ~DefaultReal32ParticleGenerator( void ) { }
    };

}

#endif
