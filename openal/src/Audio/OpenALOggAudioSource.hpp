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

#ifndef CRIMILD_OPENAL_AUDIO_OGG_AUDIO_SOURCE_
#define CRIMILD_OPENAL_AUDIO_OGG_AUDIO_SOURCE_

#include "Audio/AudioSource.hpp"

namespace crimild {

    namespace audio {

        class OpenALAudioSystem;

        class OpenALOggAudioSource : public AudioSource {
        public:
            OpenALOggAudioSource( OpenALAudioSystem *audioSystem, std::string fileName ) noexcept;
            virtual ~OpenALOggAudioSource( void ) noexcept;

            virtual void play( void ) override;
            virtual void pause( void ) override;
            virtual void stop( void ) override;

            virtual crimild::Real32 getDuration( void ) const override;

            virtual void setLoop( crimild::Bool ) override;
            virtual crimild::Bool shouldLoop( void ) const override;

            virtual void setAutoplay( crimild::Bool autoplay ) override { m_autoplay = autoplay; }
            virtual crimild::Bool shouldAutoplay( void ) const override { return m_autoplay; }

            virtual audio::AudioSource::Status getStatus( void ) const override;

            virtual void setPlayingOffset( crimild::Real32 offset ) override;
            virtual crimild::Real32 getPlayingOffset( void ) const override;

            virtual void setVolume( crimild::Real32 volume ) override;
            virtual crimild::Real32 getVolume( void ) const override;

            virtual void setTransformation( const Transformation &t ) override;

            virtual void setRelativeToListener( crimild::Bool relative ) override;
            virtual crimild::Bool isRelativeToListener( void ) const override;

            virtual void setMinDistance( crimild::Real32 distance ) override;
            virtual crimild::Real32 getMinDistance( void ) const override;

            virtual void setAttenuation( crimild::Real32 attenuation ) override;
            virtual crimild::Real32 getAttenuation( void ) const override;

            virtual void onGetData( AudioSource::GetDataCallback const &callback ) override;

            virtual crimild::UInt32 getChannelCount( void ) const override;

            virtual crimild::UInt32 getSampleRate( void ) const override;

            void onUpdate( void ) noexcept;

        private:
            class OggAudioStream; //< pImpl

            OpenALAudioSystem *m_audioSystem = nullptr;

            SharedPointer< OggAudioStream > m_audioStream;

            std::vector< crimild::UInt32 > m_buffers;
            crimild::UInt32 m_source = 0;
            crimild::Int32 m_format = 0;
            std::vector< crimild::Int16 > m_frame;

            crimild::Bool m_autoplay = false;
        };

    }

}

#endif
