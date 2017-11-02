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

#include "SFMLAudioMusicSource.hpp"

using namespace crimild;
using namespace crimild::audio;
using namespace crimild::sfml;

SFMLAudioMusicSource::SFMLAudioMusicSource( std::string filename )
{
    if ( !_music.openFromFile( filename ) ) {
		throw FileNotFoundException( filename );
	}
}

SFMLAudioMusicSource::~SFMLAudioMusicSource( void )
{

}

void SFMLAudioMusicSource::play( void )
{
	_music.play();
}

void SFMLAudioMusicSource::pause( void )
{
	_music.pause();
}

void SFMLAudioMusicSource::stop( void )
{
	_music.stop();
}

void SFMLAudioMusicSource::setLoop( crimild::Bool loop )
{
	_music.setLoop( loop );
}

crimild::Bool SFMLAudioMusicSource::shouldLoop( void ) const 
{
	return _music.getLoop();
}

AudioSource::Status SFMLAudioMusicSource::getStatus( void ) const
{
	switch ( _music.getStatus() ) {
		case sf::SoundSource::Status::Stopped:
			return AudioSource::Status::STOPPED;
			break;

		case sf::SoundSource::Status::Playing:
			return AudioSource::Status::PLAYING;
			break;

		case sf::SoundSource::Status::Paused:
			return AudioSource::Status::PAUSED;
			break;
	}

	return AudioSource::Status::STOPPED;
}

void SFMLAudioMusicSource::setPlayingOffset( crimild::Real32 offset )
{
	_music.setPlayingOffset( sf::seconds( offset ) );
}

crimild::Real32 SFMLAudioMusicSource::getPlayingOffset( void ) const
{
	return _music.getPlayingOffset().asSeconds();
}

void SFMLAudioMusicSource::setVolume( crimild::Real32 volume )
{
	_music.setVolume( volume );
}

crimild::Real32 SFMLAudioMusicSource::getVolume( void ) const 
{
	return _music.getVolume();
}

void SFMLAudioMusicSource::setTransformation( const Transformation &t ) 
{ 
	AudioSource::setTransformation( t );

	const auto position = t.getTranslate();
	_music.setPosition( position.x(), position.y(), position.z() );
}

void SFMLAudioMusicSource::setRelativeToListener( crimild::Bool relative )
{
	_music.setRelativeToListener( relative );
}

crimild::Bool SFMLAudioMusicSource::isRelativeToListener( void ) const
{
	return _music.isRelativeToListener();
}

void SFMLAudioMusicSource::setMinDistance( crimild::Real32 distance )
{
	_music.setMinDistance( distance );
}

crimild::Real32 SFMLAudioMusicSource::getMinDistance( void ) const
{
	return _music.getMinDistance();
}

void SFMLAudioMusicSource::setAttenuation( crimild::Real32 attenuation )
{
	_music.setAttenuation( attenuation );
}

crimild::Real32 SFMLAudioMusicSource::getAttenuation( void ) const
{
	return _music.getAttenuation();
}

