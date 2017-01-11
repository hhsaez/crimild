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

#include "AudioManager.hpp"
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

AudioManager &AudioManager::getInstance( void )
{
    static AudioManager instance;
    return instance;
}

AudioManager::AudioManager( void )
{
	CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

    Log::info( CRIMILD_CURRENT_CLASS_NAME, "Initializing audio sub-system" );
    
	_device = alcOpenDevice( NULL );
    if ( !_device ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot open sound device" );
        return;
    }

    _context = alcCreateContext( _device, NULL );
    if ( !_context ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot open sound context" );
    }

    alcMakeContextCurrent( _context );

    setGeneralGain( 1.0f );

	CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

AudioManager::~AudioManager( void )
{
	CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

    alcDestroyContext( _context );
    alcCloseDevice( _device );

	CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void AudioManager::setGeneralGain( float value )
{
	CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

	_gain = value;
    alListenerf( AL_GAIN, _gain );

	CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

