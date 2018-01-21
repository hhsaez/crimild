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

#include "SFMLAudioClipSource.hpp"

using namespace crimild;
using namespace crimild::audio;
using namespace crimild::sfml;

SFMLAudioClipSource::SFMLAudioClipSource( std::string filename )
{
	if ( !_soundBuffer.loadFromFile( filename ) ) {
		throw FileNotFoundException( filename );
	}

	_sound.setBuffer( _soundBuffer );
}

SFMLAudioClipSource::~SFMLAudioClipSource( void )
{

}

void SFMLAudioClipSource::play( void )
{
	_sound.play();
}

void SFMLAudioClipSource::pause( void )
{
	_sound.pause();
}

void SFMLAudioClipSource::stop( void )
{
	_sound.stop();
}

crimild::Real32 SFMLAudioClipSource::getDuration( void ) const
{
	if ( _sound.getBuffer() == nullptr ) {
		return 0.0f;
	}
	
	return _sound.getBuffer()->getDuration().asSeconds();
}

void SFMLAudioClipSource::setLoop( crimild::Bool loop )
{
	_sound.setLoop( loop );
}

crimild::Bool SFMLAudioClipSource::shouldLoop( void ) const 
{
	return _sound.getLoop();
}

AudioSource::Status SFMLAudioClipSource::getStatus( void ) const
{
	switch ( _sound.getStatus() ) {
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

void SFMLAudioClipSource::setPlayingOffset( crimild::Real32 offset )
{
	_sound.setPlayingOffset( sf::seconds( offset ) );
}

crimild::Real32 SFMLAudioClipSource::getPlayingOffset( void ) const
{
	return _sound.getPlayingOffset().asSeconds();
}

void SFMLAudioClipSource::setVolume( crimild::Real32 volume )
{
	_sound.setVolume( 100.0f * volume );
}

crimild::Real32 SFMLAudioClipSource::getVolume( void ) const 
{
	return 0.01f * _sound.getVolume();
}

void SFMLAudioClipSource::setTransformation( const Transformation &t ) 
{ 
	AudioSource::setTransformation( t );

	const auto position = t.getTranslate();
	_sound.setPosition( position.x(), position.y(), position.z() );
}

void SFMLAudioClipSource::setRelativeToListener( crimild::Bool relative )
{
	_sound.setRelativeToListener( relative );
}

crimild::Bool SFMLAudioClipSource::isRelativeToListener( void ) const
{
	return _sound.isRelativeToListener();
}

void SFMLAudioClipSource::setMinDistance( crimild::Real32 distance )
{
	_sound.setMinDistance( distance );
}

crimild::Real32 SFMLAudioClipSource::getMinDistance( void ) const
{
	return _sound.getMinDistance();
}

void SFMLAudioClipSource::setAttenuation( crimild::Real32 attenuation )
{
	_sound.setAttenuation( attenuation );
}

crimild::Real32 SFMLAudioClipSource::getAttenuation( void ) const
{
	return _sound.getAttenuation();
}

void SFMLAudioClipSource::onGetData( AudioSource::GetDataCallback const & )
{

}

crimild::UInt32 SFMLAudioClipSource::getChannelCount( void ) const
{
	return _sound.getBuffer()->getChannelCount();
}

crimild::UInt32 SFMLAudioClipSource::getSampleRate( void ) const
{
	return _sound.getBuffer()->getSampleRate();
}

