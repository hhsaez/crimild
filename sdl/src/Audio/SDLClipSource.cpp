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

#include "SDLClipSource.hpp"

#include "Foundation/Log.hpp"
#include "Exceptions/FileNotFoundException.hpp"

using namespace crimild;
using namespace crimild::audio;

SDLClipSource::SDLClipSource( std::string filename )
{
#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )			
	_chunk = Mix_LoadWAV( filename.c_str() );
	if ( _chunk == nullptr ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Failed to load sound effect: ", Mix_GetError() );
		throw FileNotFoundException( filename );
	}
#endif
}

SDLClipSource::~SDLClipSource( void )
{
#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )			
	if ( _chunk ) {
		Mix_FreeChunk( _chunk );
		_chunk = nullptr;
	}
#endif
}

void SDLClipSource::play( void )
{
#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )			
	switch ( getStatus() ) {
		case AudioSource::Status::STOPPED:
			_channel = Mix_PlayChannel( -1, _chunk, shouldLoop() ? -1 : 0 );
			break;

		case AudioSource::Status::PAUSED:
			Mix_Resume( _channel );
			break;
			
		default:
			break;
	}
#endif
}

void SDLClipSource::pause( void )
{
#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )			
	if ( getStatus() == AudioSource::Status::PLAYING ) {
		Mix_Pause( _channel );
	}
#endif
}

void SDLClipSource::stop( void )
{
#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )			
	if ( getStatus() != AudioSource::Status::STOPPED ) {
		Mix_HaltChannel( _channel );
		_channel = -1;
	}
#endif
}

crimild::Real32 SDLClipSource::getDuration( void ) const
{
	return 0;
}

void SDLClipSource::setLoop( crimild::Bool loop )
{
	_loop = loop;
}

crimild::Bool SDLClipSource::shouldLoop( void ) const 
{
	return _loop;
}

AudioSource::Status SDLClipSource::getStatus( void ) const
{
#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )			
	if ( _channel >= 0 && Mix_Playing( _channel ) ) {
		if ( Mix_Paused( _channel ) ) {
			return AudioSource::Status::PAUSED;
		}
		else {
			return AudioSource::Status::PLAYING;
		}
	}
#endif

	return AudioSource::Status::STOPPED;	
}

void SDLClipSource::setPlayingOffset( crimild::Real32 offset )
{

}

crimild::Real32 SDLClipSource::getPlayingOffset( void ) const
{
	return 0;
}

void SDLClipSource::setVolume( crimild::Real32 volume )
{
#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )			
	if ( _channel >= 0 ) {
		Mix_Volume( _channel, volume * MIX_MAX_VOLUME );
	}
#endif
}

crimild::Real32 SDLClipSource::getVolume( void ) const 
{
#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )			
	if ( _channel < 0 ) {
		return 0;
	}

	return ( crimild::Real32 ) Mix_Volume( _channel, -1 ) / ( crimild::Real32 ) MIX_MAX_VOLUME;
#else
	return 0;
#endif
}

void SDLClipSource::setTransformation( const Transformation &t ) 
{ 
	AudioSource::setTransformation( t );
}

void SDLClipSource::setRelativeToListener( crimild::Bool relative )
{

}

crimild::Bool SDLClipSource::isRelativeToListener( void ) const
{
	return false;
}

void SDLClipSource::setMinDistance( crimild::Real32 distance )
{

}

crimild::Real32 SDLClipSource::getMinDistance( void ) const
{
	return 0;
}

void SDLClipSource::setAttenuation( crimild::Real32 attenuation )
{

}

crimild::Real32 SDLClipSource::getAttenuation( void ) const
{
	return 0;
}

void SDLClipSource::onGetData( AudioSource::GetDataCallback const & )
{

}

crimild::UInt32 SDLClipSource::getChannelCount( void ) const
{
	return 0;
}

crimild::UInt32 SDLClipSource::getSampleRate( void ) const
{
	return 0;
}

