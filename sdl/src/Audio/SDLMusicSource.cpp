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

#include "SDLMusicSource.hpp"

#include "Foundation/Log.hpp"
#include "Exceptions/FileNotFoundException.hpp"

using namespace crimild;
using namespace crimild::audio;

SDLMusicSource::SDLMusicSource( std::string filename )
{
	_music = Mix_LoadMUS( filename.c_str() );
	if ( _music == nullptr ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot load music file: ", Mix_GetError() );
		throw FileNotFoundException( filename );
	}
}

SDLMusicSource::~SDLMusicSource( void )
{
	if ( _music ) {
		Mix_FreeMusic( _music );
		_music = nullptr;
	}
}

void SDLMusicSource::play( void )
{
	Mix_HaltMusic();
	Mix_PlayMusic( _music, shouldLoop() ? -1 : 1 );
}

void SDLMusicSource::pause( void )
{
	if ( Mix_PlayingMusic() ) {
		Mix_PauseMusic();
	}
}

void SDLMusicSource::stop( void )
{
	Mix_HaltMusic();
}

crimild::Real32 SDLMusicSource::getDuration( void ) const
{
	return 0;
}

void SDLMusicSource::setLoop( crimild::Bool loop )
{
	_loop = loop;
}

crimild::Bool SDLMusicSource::shouldLoop( void ) const 
{
	return _loop;
}

AudioSource::Status SDLMusicSource::getStatus( void ) const
{
	if ( Mix_PlayingMusic() ) {
		if ( Mix_PausedMusic() ) {
			return AudioSource::Status::PAUSED;
		}
		else {
			return AudioSource::Status::PLAYING;
		}
	}
	return AudioSource::Status::STOPPED;
}

void SDLMusicSource::setPlayingOffset( crimild::Real32 offset )
{

}

crimild::Real32 SDLMusicSource::getPlayingOffset( void ) const
{
	return 0;
}

void SDLMusicSource::setVolume( crimild::Real32 volume )
{
	Mix_VolumeMusic( volume * MIX_MAX_VOLUME );
}

crimild::Real32 SDLMusicSource::getVolume( void ) const 
{
	return crimild::Real32( Mix_VolumeMusic( -1 ) ) / ( crimild::Real32 ) MIX_MAX_VOLUME;
}

void SDLMusicSource::setTransformation( const Transformation &t ) 
{ 
	AudioSource::setTransformation( t );
}

void SDLMusicSource::setRelativeToListener( crimild::Bool relative )
{

}

crimild::Bool SDLMusicSource::isRelativeToListener( void ) const
{
	return false;
}

void SDLMusicSource::setMinDistance( crimild::Real32 distance )
{

}

crimild::Real32 SDLMusicSource::getMinDistance( void ) const
{
	return 0;
}

void SDLMusicSource::setAttenuation( crimild::Real32 attenuation )
{
	
}

crimild::Real32 SDLMusicSource::getAttenuation( void ) const
{
	return 0;
}

void SDLMusicSource::onGetData( AudioSource::GetDataCallback const &callback )
{
	
}

crimild::UInt32 SDLMusicSource::getChannelCount( void ) const
{
	return 0;
}

crimild::UInt32 SDLMusicSource::getSampleRate( void ) const
{
	return 0;
}


