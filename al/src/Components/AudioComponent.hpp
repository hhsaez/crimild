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

#ifndef CRIMILD_AL_COMPONENTS_AUDIO_
#define CRIMILD_AL_COMPONENTS_AUDIO_

#include "Audio/AudioClip.hpp"

namespace crimild {

	namespace al {

		class AudioComponent : public NodeComponent {
			CRIMILD_IMPLEMENT_RTTI( crimild::al::AudioComponent )

		public:
			explicit AudioComponent( AudioClipPtr const &audioClip );
			
			virtual ~AudioComponent( void );

			virtual void onAttach( void ) override;
			virtual void update( const Clock & ) override;

			void setGain( float value );
			float getGain( void ) const { return _gain; }

			bool isPlaying( void ) const;
			bool isPaused( void ) const;

			void play( bool loop = false );
			void stop( void );
			void pause( void );
			void resume( void );

		private:
			AudioClipPtr _audioClip;
			unsigned int _sourceId;
			float _gain;
		};

	}

}

#endif

