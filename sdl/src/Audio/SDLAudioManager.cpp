/*
 * Copyright (c) 2013 - 2018, Hugo Hernan Saez
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

#include "SDLAudioManager.hpp"
#include "SDLAudioListener.hpp"
#include "SDLMusicSource.hpp"
#include "SDLClipSource.hpp"

#include "Foundation/Log.hpp"

using namespace crimild;
using namespace crimild::audio;

#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )
#include <SDL.h>
#include <SDL_mixer.h>
#endif

SDLAudioManager::SDLAudioManager( void )
	: _audioListener( crimild::alloc< SDLAudioListener >() )
{
#if !defined( CRIMILD_PLATFORM_EMSCRIPTEN )
	SDL_InitSubSystem( SDL_INIT_AUDIO );

	if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
	}
#endif
}

SDLAudioManager::~SDLAudioManager( void )
{

}

AudioSourcePtr SDLAudioManager::createAudioSource( std::string filename, crimild::Bool isMusic )
{
	if ( isMusic ) {
		return crimild::alloc< SDLMusicSource >( filename );
	}

	return crimild::alloc< SDLClipSource >( filename );
}

