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

#ifndef CRIMILD_ANIMATION_ANIMATION_
#define CRIMILD_ANIMATION_ANIMATION_

#include "Accumulator.hpp"

#include "Coding/Codable.hpp"
#include "Foundation/NamedObject.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Mathematics/Clock.hpp"

namespace crimild {

	namespace animation {

		class Clip;
		
		class Animation :
			public coding::Codable,
			public NamedObject {
			CRIMILD_IMPLEMENT_RTTI( crimild::animation::Animation )

		public:
			enum class PlaybackMode {
				ONCE,
				REPEAT,
				MIRROR_REPEAT
			};
			
		public:
			explicit Animation( std::string name );
			explicit Animation( SharedPointer< Clip > const &clip, crimild::Real32 offset = 0.0f, crimild::Real32 duration = -1 );
			virtual ~Animation( void );

			Clip *getClip( void ) { return crimild::get_ptr( _clip ); }
			void setClip( SharedPointer< Clip > &clip, crimild::Real64 offset = 0.0, crimild::Real64 duration = -1.0 );

			const Clock &getClock( void ) const { return _clock; }

			void setPlaybackMode( PlaybackMode playbackMode ) { _playbackMode = playbackMode; }
			PlaybackMode getPlaybackMode( void ) const { return _playbackMode; }

			void setDuration( crimild::Real32 duration ) { _duration = duration; }
			crimild::Real32 getDuration( void ) const { return _duration; }

			void setOffset( crimild::Real32 offset ) { _offset = offset; }
			crimild::Real32 getOffset( void ) const { return _offset; }

			void setTimeScale( crimild::Real32 timeScale ) { _clock.setTimeScale( timeScale ); }
			crimild::Real32 getTimeScale( void ) const { return _clock.getTimeScale(); }

		private:
			crimild::Clock _clock;
			crimild::Real32 _duration = 0.0;
			crimild::Real32 _offset = 0.0;
			PlaybackMode _playbackMode = PlaybackMode::REPEAT;
			SharedPointer< Clip > _clip;

		public:
			Animation *update( const Clock &c );
			Animation *lerp( Animation *other, crimild::Real32 factor );
			Animation *add( Animation *other, crimild::Real32 strength );

		public:
			template< typename T >
			Animation *setValue( const std::string &channelName, const T &value )
			{
				if ( !_accumulators.contains( channelName ) ) {
					_accumulators[ channelName ] = crimild::alloc< AccumulatorImpl< T >>();
				}

				auto acc = crimild::get_ptr( _accumulators[ channelName ] );
				if ( acc != nullptr ) {
					static_cast< AccumulatorImpl< T > * >( acc )->set( value );
				}

				return this;
			}

			template< typename T >
			Animation *getValue( const std::string &channelName, T &value )
			{
				if ( _accumulators.contains( channelName ) ) {
					auto acc = crimild::get_ptr( _accumulators[ channelName ] );
					if ( acc != nullptr ) {
						value = static_cast< AccumulatorImpl< T > * >( acc )->get();
					}
				}
				
				return this;
			}

		private:
			containers::Map< std::string, SharedPointer< Accumulator >> _accumulators;
		};

	}

}

#endif


