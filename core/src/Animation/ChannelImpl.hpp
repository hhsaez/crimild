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

#ifndef CRIMILD_ANIMATION_CHANNEL_IMPL_
#define CRIMILD_ANIMATION_CHANNEL_IMPL_

#include "Animation.hpp"
#include "Channel.hpp"
#include "Crimild_Coding.hpp"

namespace crimild {

    namespace animation {

        template< typename T >
        class ChannelImpl : public Channel {
        private:
            using TimeArray = Array< crimild::Real32 >;
            using KeyArray = Array< T >;

        public:
            ChannelImpl( void ) { }

            ChannelImpl( std::string name, const TimeArray &times, const KeyArray &keys )
                : Channel( name ),
                  _times( times ),
                  _keys( keys )
            {
            }

            ChannelImpl( std::string name, TimeArray &&times, KeyArray &&keys )
                : Channel( name ),
                  _times( times ),
                  _keys( keys )
            {
            }

            virtual ~ChannelImpl( void )
            {
            }

            virtual crimild::Real32 getDuration( void ) const override
            {
                if ( _times.size() == 0 ) {
                    return 0.0;
                }

                return _times.last();
            }

            virtual void evaluate( crimild::Real32 t, Animation *animation ) override
            {
                if ( _keys.size() == 0 || _times.size() == 0 ) {
                    return;
                }

                T result;

                if ( _keys.size() == 1 || t <= _times.first() ) {
                    result = _keys.first();
                } else if ( t >= _times.last() ) {
                    result = _keys.last();
                } else {
                    // TODO: cache last index to avoid this loop
                    crimild::Size index = 0;
                    for ( ; index < _times.size() - 1; index++ ) {
                        if ( t < _times[ index + 1 ] ) {
                            break;
                        }
                    }

                    const auto t0 = _times[ index ];
                    const auto t1 = _times[ index + 1 ];

                    const auto &v0 = _keys[ index ];
                    const auto &v1 = _keys[ index + 1 ];

                    const auto u = ( t - t0 ) / ( t1 - t0 );
                    interpolate( v0, v1, u, result );
                }
                animation->setValue( getName(), result );
            }

        private:
            template< typename U >
            void interpolate( const U &start, const U &end, crimild::Real32 t, U &result )
            {
                Interpolation::linear( start, end, t, result );
            }

            void interpolate( const Quaternion &start, const Quaternion &end, crimild::Real32 t, Quaternion &result )
            {
                result = slerp( start, end, t );
            }

        private:
            TimeArray _times;
            KeyArray _keys;

            /**
                           \name Coding
                        */
            //@{

        public:
            virtual void encode( coding::Encoder &encoder ) override
            {
                Codable::encode( encoder );

                encoder.encode( "name", getName() );
                encoder.encode( "times", _times );
                encoder.encode( "keys", _keys );
            }

            virtual void decode( coding::Decoder &decoder ) override
            {
                Codable::decode( decoder );

                std::string name;
                decoder.decode( "name", name );
                setName( name );

                decoder.decode( "times", _times );
                decoder.decode( "keys", _keys );
            }

            //@}
        };

        class Vector3fChannel : public ChannelImpl< Vector3f > {
            CRIMILD_IMPLEMENT_RTTI( crimild::animation::Vector3fChannel )

        public:
            Vector3fChannel( void ) { }

            Vector3fChannel( std::string name, const Array< crimild::Real32 > &times, const Array< Vector3f > &keys )
                : ChannelImpl( name, times, keys )
            {
            }

            Vector3fChannel( std::string name, Array< crimild::Real32 > &&times, Array< Vector3f > &&keys )
                : ChannelImpl( name, times, keys )
            {
            }

            virtual ~Vector3fChannel( void )
            {
            }
        };

        class QuaternionChannel : public ChannelImpl< Quaternion > {
            CRIMILD_IMPLEMENT_RTTI( crimild::animation::QuaternionChannel )

        public:
            QuaternionChannel( void ) { }

            QuaternionChannel( std::string name, const Array< crimild::Real32 > &times, const Array< Quaternion > &keys )
                : ChannelImpl( name, times, keys )
            {
            }

            QuaternionChannel( std::string name, Array< crimild::Real32 > &&times, Array< Quaternion > &&keys )
                : ChannelImpl( name, times, keys )
            {
            }

            virtual ~QuaternionChannel( void )
            {
            }
        };

        class Real32Channel : public ChannelImpl< Real32 > {
            CRIMILD_IMPLEMENT_RTTI( crimild::animation::Real32Channel )

        public:
            Real32Channel( void ) { }

            Real32Channel( std::string name, const Array< crimild::Real32 > &times, const Array< Real32 > &keys )
                : ChannelImpl( name, times, keys )
            {
            }

            Real32Channel( std::string name, Array< crimild::Real32 > &&times, Array< Real32 > &&keys )
                : ChannelImpl( name, times, keys )
            {
            }

            virtual ~Real32Channel( void )
            {
            }
        };

    }

}

#endif
