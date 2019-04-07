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

#include "CrimildAVAudioMusicSource.h"

#include "Exceptions/FileNotFoundException.hpp"

using namespace crimild;
using namespace crimild::audio;
using namespace crimild::ios;

AVAudioMusicSource::AVAudioMusicSource( std::string filename )
{
    NSString* file = [NSString stringWithCString:filename.c_str() encoding:[NSString defaultCStringEncoding]];
    // make sure the file is mp3
    file = [file stringByReplacingOccurrencesOfString:@".ogg" withString:@".mp3"];
    
    _fileURL = [NSURL fileURLWithPath: file];
    
    NSError *error;
    _localAudioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:_fileURL error:&error];
    [_localAudioPlayer prepareToPlay];
}

AVAudioMusicSource::~AVAudioMusicSource( void )
{
    _localAudioPlayer = nil;
}

void AVAudioMusicSource::play( void )
{
    if (_localAudioPlayer != nil) {
        [_localAudioPlayer play];
    }
}

void AVAudioMusicSource::pause( void )
{
    if ( _localAudioPlayer != nil && [_localAudioPlayer isPlaying] ) {
        [_localAudioPlayer pause];
    }
}

void AVAudioMusicSource::stop( void )
{
    if ( _localAudioPlayer != nil && [_localAudioPlayer isPlaying] ) {
        [_localAudioPlayer stop];
        [_localAudioPlayer setCurrentTime: 0.0];
    }
}

crimild::Real32 AVAudioMusicSource::getDuration( void ) const
{
    return 0.0f;
}

void AVAudioMusicSource::setLoop( crimild::Bool loop )
{
    if ( _localAudioPlayer != nil ) {
        [_localAudioPlayer setNumberOfLoops: loop ? -1 : 1];
    }
}

crimild::Bool AVAudioMusicSource::shouldLoop( void ) const
{
    if ( _localAudioPlayer == nil ) {
        return false;
    }
    
    return [_localAudioPlayer numberOfLoops] < 0;
}

AudioSource::Status AVAudioMusicSource::getStatus( void ) const
{
    if ( _localAudioPlayer == nil ) {
        return AudioSource::Status::STOPPED;
    }
    
    return [_localAudioPlayer isPlaying] ? AudioSource::Status::PLAYING : AudioSource::Status::PAUSED;
}

void AVAudioMusicSource::setPlayingOffset( crimild::Real32 offset )
{
    if ( _localAudioPlayer != nil ) {
        [_localAudioPlayer setCurrentTime: offset];
    }
}

crimild::Real32 AVAudioMusicSource::getPlayingOffset( void ) const
{
    if ( _localAudioPlayer == nil ) {
        return 0.0f;
    }
    
    return [_localAudioPlayer currentTime];
}

void AVAudioMusicSource::setVolume( crimild::Real32 volume )
{
    if ( _localAudioPlayer != nil ) {
        [_localAudioPlayer setVolume: volume];
    }
}

crimild::Real32 AVAudioMusicSource::getVolume( void ) const
{
    return _localAudioPlayer != nil ? [_localAudioPlayer volume] : 0.0f;
}

void AVAudioMusicSource::setTransformation( const Transformation &t )
{
    AudioSource::setTransformation( t );
}

void AVAudioMusicSource::setRelativeToListener( crimild::Bool relative )
{
    
}

crimild::Bool AVAudioMusicSource::isRelativeToListener( void ) const
{
    return false;
}

void AVAudioMusicSource::setMinDistance( crimild::Real32 distance )
{
    
}

crimild::Real32 AVAudioMusicSource::getMinDistance( void ) const
{
    return 0.0f;
}

void AVAudioMusicSource::setAttenuation( crimild::Real32 attenuation )
{
    
}

crimild::Real32 AVAudioMusicSource::getAttenuation( void ) const
{
    return 0;
}

void AVAudioMusicSource::onGetData( AudioSource::GetDataCallback const & )
{
    
}

crimild::UInt32 AVAudioMusicSource::getChannelCount( void ) const
{
    return 1;
}

crimild::UInt32 AVAudioMusicSource::getSampleRate( void ) const
{
    return 0;
}

