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

#ifndef CRIMILD_AUDIO_SOURCE_
#define CRIMILD_AUDIO_SOURCE_

#include "Foundation/SharedObject.hpp"
#include "Foundation/Types.hpp"

#include "Mathematics/Transformation.hpp"

namespace crimild {

	namespace audio {

		/**
		   \brief Abstract interface for audio sources
		 */
		class AudioSource : public SharedObject {
		public:
			enum class Status {
				STOPPED,
				PLAYING,
				PAUSED
			};

			struct Chunk {
				const crimild::Int16 *samples;
				crimild::Size sampleCount;
			};

		protected:
			AudioSource( void ) { }

		public:
	        virtual ~AudioSource( void ) { }

	    public:
	    	virtual void play( void ) = 0;
	    	virtual void pause( void ) = 0;
	    	virtual void stop( void ) = 0;

	    	virtual void setLoop( crimild::Bool loop ) = 0;
	    	virtual crimild::Bool shouldLoop( void ) const = 0;

	    	virtual void setAutoplay( crimild::Bool autoplay ) = 0;
	    	virtual crimild::Bool shouldAutoplay( void ) const = 0;

	    	virtual Status getStatus( void ) const = 0;

	    	virtual void setPlayingOffset( crimild::Real32 offset ) = 0;
	    	virtual crimild::Real32 getPlayingOffset( void ) const = 0;

	    	virtual void setVolume( crimild::Real32 volume ) = 0;
	    	virtual crimild::Real32 getVolume( void ) const = 0;

	    	virtual void enableSpatialization( crimild::Bool enabled ) { _spatializationEnabled = enabled; }
	    	virtual crimild::Bool isSpatializationEnabled( void ) const { return _spatializationEnabled; }

	    	virtual void setTransformation( const Transformation &t ) { _transformation = t; }
	    	virtual const Transformation &getTransformation( void ) const { return _transformation; }

	    	virtual void setRelativeToListener( crimild::Bool relative ) = 0;
	    	virtual crimild::Bool isRelativeToListener( void ) const = 0;

	    	virtual void setMinDistance( crimild::Real32 distance ) = 0;
	    	virtual crimild::Real32 getMinDistance( void ) const = 0;

	    	virtual void setAttenuation( crimild::Real32 attenuation ) = 0;
	    	virtual crimild::Real32 getAttenuation( void ) const = 0;

			using GetDataCallback = std::function< void( Chunk const & ) >;
			virtual void onGetData( GetDataCallback const &callback ) = 0;

			virtual crimild::UInt32 getChannelCount( void ) const = 0;

			virtual crimild::UInt32 getSampleRate( void ) const = 0;

	    private:
	    	crimild::Bool _spatializationEnabled = false;
	    	Transformation _transformation;
		};

		using AudioSourcePtr = SharedPointer< AudioSource >;

	}

}

#endif

