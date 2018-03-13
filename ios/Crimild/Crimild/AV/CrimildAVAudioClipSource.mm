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

#include "CrimildAVAudioClipSource.h"

#include "Exceptions/FileNotFoundException.hpp"

using namespace crimild;
using namespace crimild::audio;
using namespace crimild::ios;

AVAudioClipSource::AVAudioClipSource( std::string filename )
{
    NSString *file = [NSString stringWithCString: filename.c_str() encoding:[NSString defaultCStringEncoding]];
    _fileURL = [NSURL fileURLWithPath: file];
    
    AudioServicesCreateSystemSoundID( ( __bridge CFURLRef ) _fileURL, &_soundID );
}

AVAudioClipSource::~AVAudioClipSource( void )
{
    AudioServicesDisposeSystemSoundID( _soundID );
}

void AVAudioClipSource::play( void )
{
    AudioServicesPlaySystemSound( _soundID );
}

void AVAudioClipSource::pause( void )
{

}

void AVAudioClipSource::stop( void )
{

}

crimild::Real32 AVAudioClipSource::getDuration( void ) const
{
    return 0.0f;
}

void AVAudioClipSource::setLoop( crimild::Bool loop )
{

}

crimild::Bool AVAudioClipSource::shouldLoop( void ) const
{
    return false;
}

AudioSource::Status AVAudioClipSource::getStatus( void ) const
{
    return AudioSource::Status::STOPPED;
}

void AVAudioClipSource::setPlayingOffset( crimild::Real32 offset )
{

}

crimild::Real32 AVAudioClipSource::getPlayingOffset( void ) const
{
    return 0.0f;
}

void AVAudioClipSource::setVolume( crimild::Real32 volume )
{

}

crimild::Real32 AVAudioClipSource::getVolume( void ) const
{
    return 1.0f;
}

void AVAudioClipSource::setTransformation( const Transformation &t )
{
    AudioSource::setTransformation( t );
}

void AVAudioClipSource::setRelativeToListener( crimild::Bool relative )
{

}

crimild::Bool AVAudioClipSource::isRelativeToListener( void ) const
{
    return false;
}

void AVAudioClipSource::setMinDistance( crimild::Real32 distance )
{

}

crimild::Real32 AVAudioClipSource::getMinDistance( void ) const
{
    return 0.0f;
}

void AVAudioClipSource::setAttenuation( crimild::Real32 attenuation )
{

}

crimild::Real32 AVAudioClipSource::getAttenuation( void ) const
{
    return 0;
}

void AVAudioClipSource::onGetData( AudioSource::GetDataCallback const & )
{
    
}

crimild::UInt32 AVAudioClipSource::getChannelCount( void ) const
{
    return 1;
}

crimild::UInt32 AVAudioClipSource::getSampleRate( void ) const
{
    return 0;
}

