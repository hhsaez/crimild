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

#include "AudioClip.hpp"
#include "Utils.hpp"

#ifdef __APPLE__
    #import <OpenAL/al.h>
    #import <OpenAL/alc.h>
#else
    #include <al.h>
    #include <alc.h>
#endif

using namespace crimild;
using namespace crimild::al;

AudioClip::AudioClip( void )
{

}

AudioClip::~AudioClip( void )
{
	if ( _bufferId > 0 ) {
		alDeleteBuffers( 1, &_bufferId );
	}
}

void AudioClip::load( unsigned int numChannels, unsigned int bitsPerSample, unsigned int frequency, unsigned int size, const unsigned char *data )
{
	CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

	ALenum format = AL_FORMAT_MONO8;

	//The format is worked out by looking at the number of
	//channels and the bits per sample.
	if ( numChannels == 1 ) {
    	if ( bitsPerSample == 8 ) {
        	format = AL_FORMAT_MONO8;
        }
    	else if ( bitsPerSample == 16 ) {
        	format = AL_FORMAT_MONO16;
        }
	} else if ( numChannels == 2 ) {
    	if ( bitsPerSample == 8 ) {
        	format = AL_FORMAT_STEREO8;
        }
    	else if ( bitsPerSample == 16 ) {
        	format = AL_FORMAT_STEREO16;
        }
	}

    //create our openAL buffer and check for success
    alGenBuffers( 1, &_bufferId );
    alBufferData( _bufferId, format, ( void * ) &data[ 0 ], size, frequency );

	CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

