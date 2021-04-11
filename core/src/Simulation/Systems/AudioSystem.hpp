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

#ifndef CRIMILD_SIMULATION_SYSTEMS_AUDIO_
#define CRIMILD_SIMULATION_SYSTEMS_AUDIO_

#include "Foundation/Singleton.hpp"
#include "Simulation/Systems/System.hpp"

namespace crimild {

    namespace audio {

        class AudioListener;
        class AudioSource;

    }

    class AudioSystem
        : public System,
          public DynamicSingleton< AudioSystem > {
        CRIMILD_IMPLEMENT_RTTI( crimild::AudioSystem )

    public:
        virtual ~AudioSystem( void ) = default;

    public:
        /**
           \brief Get the shared audio listener
        */
        virtual audio::AudioListener *getAudioListener( void ) noexcept = 0;

    public:
        /**
           \brief Creates a new audio source from a file

           \param asStream Indicates if the audio file should be streamed
           instead of loaded completely into memory. Some implementations
           may ignore this flag.

           The actual implementation must be performed by subclasses
           based on each platform requirements.
        */
        virtual SharedPointer< audio::AudioSource > createAudioSource( std::string filename, bool asStream ) noexcept = 0;
    };

}

#endif
