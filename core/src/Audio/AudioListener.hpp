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

#ifndef CRIMILD_AUDIO_LISTENER_
#define CRIMILD_AUDIO_LISTENER_

#include "Coding/Codable.hpp"
#include "Crimild_Mathematics.hpp"

namespace crimild {

    namespace audio {

        /**
           \brief Represents a microphone in the virtual world

           \warning There should be only one active listener at
           any time in any given scene.
         */
        class AudioListener : public coding::Codable {
            CRIMILD_IMPLEMENT_RTTI( crimild::audio::AudioListener )

        protected:
            AudioListener( void );

        public:
            virtual ~AudioListener( void );

            /**
                    \brief Updates the listener transformation

                    Different platform may need to override this function
                    in order to update specific APIs parameters, like
                    position, direction and other vectors
            */
            virtual void setTransformation( const Transformation &t ) { _transformation = t; }
            virtual const Transformation &getTransformation( void ) const { return _transformation; }

        private:
            Transformation _transformation;

            /**
               \name Coding support
            */
            //@{

        public:
            virtual void encode( coding::Encoder &encoder ) override;
            virtual void decode( coding::Decoder &decoder ) override;

            //@}
        };

        using AudioListenerPtr = SharedPointer< AudioListener >;

    }

}

#endif
