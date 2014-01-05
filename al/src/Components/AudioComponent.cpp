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

#include "AudioComponent.hpp"
#include "Audio/AudioManager.hpp"
#include "Audio/Utils.hpp"

#ifdef __APPLE__
    #import <OpenAL/al.h>
    #import <OpenAL/alc.h>
#else
    #include <al.h>
    #include <alc.h>
#endif

using namespace crimild;
using namespace crimild::al;

AudioComponent::AudioComponent( AudioClipPtr audioClip )
	: _audioClip( audioClip ),
	  _gain( 1.0f )
{
	CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

	AudioManager::getInstance();

	alGenSources( 1, &_sourceId );
	alSourcei( _sourceId, AL_BUFFER, audioClip->getBufferId() );

	CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

AudioComponent::~AudioComponent( void )
{
	CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( _sourceId > 0 ) {
		alDeleteSources( 1, &_sourceId );
	}

	CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void AudioComponent::onAttach( void )
{
	CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

	const Vector3f &translate = getNode()->getWorld().getTranslate();
	alSource3f( _sourceId, AL_POSITION, translate[ 0 ], translate[ 1 ], translate[ 2 ] );

	CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void AudioComponent::update( const Time & )
{
	const Vector3f &translate = getNode()->getWorld().getTranslate();
	alSource3f( _sourceId, AL_POSITION, translate[ 0 ], translate[ 1 ], translate[ 2 ] );
}

void AudioComponent::setGain( float value )
{
	CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

	_gain = value;
	alSourcef( _sourceId, AL_GAIN, _gain );

	CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

bool AudioComponent::isPlaying( void ) const
{
	int state;
	alGetSourcei( _sourceId, AL_SOURCE_STATE, &state );
	return state == AL_PLAYING;
}

bool AudioComponent::isPaused( void ) const
{
	int state;
	alGetSourcei( _sourceId, AL_SOURCE_STATE, &state );
	return state == AL_PAUSED;
}

void AudioComponent::play( bool loop )
{
	CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

	getNode()->perform( UpdateWorldState() );

	alSourcei( _sourceId, AL_LOOPING, loop ? AL_TRUE : AL_FALSE );
	const Vector3f &translate = getNode()->getWorld().getTranslate();
	alSource3f( _sourceId, AL_POSITION, translate[ 0 ], translate[ 1 ], translate[ 2 ] );
	alSourcePlay( _sourceId );

	CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void AudioComponent::stop( void )
{
	if ( isPlaying() ) {
		alSourceStop( _sourceId );
	}
}

void AudioComponent::pause( void )
{
	if ( isPlaying() ) {
		alSourcePause( _sourceId );
	}
}

void AudioComponent::resume( void )
{
	if ( isPaused() || !isPlaying() ) {
		alSourcePlay( _sourceId );
	}
}

