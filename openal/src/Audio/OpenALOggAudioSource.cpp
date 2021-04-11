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

#include "Audio/OpenALOggAudioSource.hpp"

#include "Foundation/Log.hpp"
#include "Foundation/OpenALUtils.hpp"
#include "Simulation/Systems/OpenALAudioSystem.hpp"

#include <stb_vorbis.c>

using namespace crimild;
using namespace crimild::audio;

// Define the number of buffers and buffer size (in milliseconds) to use. 4
// buffers with 8192 samples each gives a nice per-chunk size, and lets the
// queue last for almost one second at 44.1khz.
#define CRIMILD_OPENAL_NUM_BUFFERS 4
#define CRIMILD_OPENAL_BUFFER_SAMPLES 8192

class OpenALOggAudioSource::OggAudioStream {
public:
    stb_vorbis *stream = nullptr;
    stb_vorbis_info info = { 0 };

    Bool streamBuffer( ALuint buffer, Int32 format, Size bufferSize, Size &totalSamplesLeft ) noexcept
    {
        // Uncomment this to avoid VLAs
        //#define BUFFER_SIZE 4096 * 32

        ALshort pcm[ bufferSize ];
        Int32 size = 0;
        Int32 result = 0;

        while ( size < bufferSize ) {
            result = stb_vorbis_get_samples_short_interleaved(
                stream,
                info.channels,
                pcm + size,
                bufferSize - size );
            if ( result > 0 ) {
                size += result * info.channels;
            } else {
                break;
            }
        }

        if ( size == 0 ) {
            return false;
        }

        alBufferData(
            buffer,
            format,
            pcm,
            size * sizeof( ALshort ),
            info.sample_rate );
        totalSamplesLeft -= size;

        CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;

        return true;
    }
};

OpenALOggAudioSource::OpenALOggAudioSource( OpenALAudioSystem *audioSystem, std::string filename ) noexcept
    : m_audioSystem( audioSystem ),
      m_buffers( CRIMILD_OPENAL_NUM_BUFFERS )
{
    CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

    m_audioStream = crimild::alloc< OggAudioStream >();

    // Generate buffers and sources
    alGenBuffers( m_buffers.size(), m_buffers.data() );
    alGenSources( 1, &m_source );

    m_audioStream->stream = stb_vorbis_open_filename( filename.c_str(), nullptr, nullptr );
    if ( !m_audioStream->stream ) {
        CRIMILD_LOG_ERROR( "Cannot open audio stream:", filename );
        return;
    }

    m_audioStream->info = stb_vorbis_get_info( m_audioStream->stream );

    // Assume audio is either mono or stereo
    // TODO: Support other audio formats?
    if ( m_audioStream->info.channels == 2 ) {
        m_format = AL_FORMAT_STEREO16;
    } else if ( m_audioStream->info.channels == 1 ) {
        m_format = AL_FORMAT_MONO16;
    } else {
        CRIMILD_LOG_ERROR( "Unsupported channel count: ", m_audioStream->info.channels );
        return;
    }

    m_frame.resize( CRIMILD_OPENAL_BUFFER_SAMPLES * m_audioStream->info.channels * sizeof( crimild::Int16 ) );

    // Set parameters so music plays in the front-center speaker
    // TODO: support 3D music
    alSource3i( m_source, AL_POSITION, 0, 0, -1 );
    alSourcei( m_source, AL_SOURCE_RELATIVE, AL_TRUE );
    alSourcei( m_source, AL_ROLLOFF_FACTOR, 0 );

    CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

OpenALOggAudioSource::~OpenALOggAudioSource( void ) noexcept
{
    if ( m_source > 0 ) {
        alDeleteSources( 1, &m_source );
        m_source = 0;
    }

    if ( m_buffers[ 0 ] > 0 ) {
        alDeleteBuffers( m_buffers.size(), m_buffers.data() );
    };

    if ( m_audioStream != nullptr ) {
        if ( m_audioStream->stream != nullptr ) {
            stb_vorbis_close( m_audioStream->stream );
        }
        m_audioStream = nullptr;
    }

    m_audioSystem = nullptr;
}

void OpenALOggAudioSource::play( void )
{
    CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

    if ( getStatus() == AudioSource::Status::PLAYING ) {
        return;
    }

    alSourceRewind( m_source );
    alSourcei( m_source, AL_BUFFER, 0 );

    for ( auto bufferId : m_buffers ) {
        auto len = stb_vorbis_get_samples_short_interleaved(
            m_audioStream->stream,
            m_audioStream->info.channels,
            m_frame.data(),
            CRIMILD_OPENAL_BUFFER_SAMPLES );
        if ( len <= 0 ) {
            break;
        }

        alBufferData(
            bufferId,
            m_format,
            m_frame.data(),
            len * m_audioStream->info.channels * sizeof( crimild::Int16 ),
            m_audioStream->info.sample_rate );
    }

    alSourceQueueBuffers( m_source, m_buffers.size(), m_buffers.data() );
    alSourcePlay( m_source );

    m_audioSystem->play( this );

    CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenALOggAudioSource::pause( void )
{
    if ( getStatus() == AudioSource::Status::PLAYING ) {
        m_audioSystem->pause( this );
        alSourcePause( m_source );
    }
}

void OpenALOggAudioSource::stop( void )
{
    if ( getStatus() == AudioSource::Status::PLAYING ) {
        m_audioSystem->stop( this );
        alSourceStop( m_source );
    }
}

crimild::Real32 OpenALOggAudioSource::getDuration( void ) const
{
    return 0;
}

void OpenALOggAudioSource::setLoop( crimild::Bool loop )
{
    //    alSourcei( m_source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE );
}

crimild::Bool OpenALOggAudioSource::shouldLoop( void ) const
{
    return false;
}

AudioSource::Status OpenALOggAudioSource::getStatus( void ) const
{
    Int32 state;
    alGetSourcei( m_source, AL_SOURCE_STATE, &state );
    switch ( state ) {
        case AL_PLAYING:
            return AudioSource::Status::PLAYING;

        case AL_PAUSED:
            return AudioSource::Status::PAUSED;

        default:
            return AudioSource::Status::STOPPED;
    }
}

void OpenALOggAudioSource::setPlayingOffset( crimild::Real32 offset )
{
}

crimild::Real32 OpenALOggAudioSource::getPlayingOffset( void ) const
{
    return 0;
}

void OpenALOggAudioSource::setVolume( crimild::Real32 volume )
{
    alSourcef( m_source, AL_GAIN, volume );
}

crimild::Real32 OpenALOggAudioSource::getVolume( void ) const
{
    return 0;
}

void OpenALOggAudioSource::setTransformation( const Transformation &t )
{
    AudioSource::setTransformation( t );
}

void OpenALOggAudioSource::setRelativeToListener( crimild::Bool relative )
{
}

crimild::Bool OpenALOggAudioSource::isRelativeToListener( void ) const
{
    return false;
}

void OpenALOggAudioSource::setMinDistance( crimild::Real32 distance )
{
}

crimild::Real32 OpenALOggAudioSource::getMinDistance( void ) const
{
    return 0;
}

void OpenALOggAudioSource::setAttenuation( crimild::Real32 attenuation )
{
}

crimild::Real32 OpenALOggAudioSource::getAttenuation( void ) const
{
    return 0;
}

void OpenALOggAudioSource::onGetData( AudioSource::GetDataCallback const & )
{
}

crimild::UInt32 OpenALOggAudioSource::getChannelCount( void ) const
{
    return 0;
}

crimild::UInt32 OpenALOggAudioSource::getSampleRate( void ) const
{
    return 0;
}

void OpenALOggAudioSource::onUpdate( void ) noexcept
{
    ALint state;
    alGetSourcei( m_source, AL_SOURCE_STATE, &state );

    ALint processed = 0;
    alGetSourcei( m_source, AL_BUFFERS_PROCESSED, &processed );

    while ( processed > 0 ) {
        ALuint bufferId = 0;

        CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION

        alSourceUnqueueBuffers( m_source, 1, &bufferId );
        CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION

        processed--;

        auto len = stb_vorbis_get_samples_short_interleaved(
            m_audioStream->stream,
            m_audioStream->info.channels,
            m_frame.data(),
            CRIMILD_OPENAL_BUFFER_SAMPLES );
        if ( len > 0 ) {
            alBufferData(
                bufferId,
                m_format,
                m_frame.data(),
                len * m_audioStream->info.channels * sizeof( crimild::Int16 ),
                m_audioStream->info.sample_rate );

            CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION
            alSourceQueueBuffers( m_source, 1, &bufferId );
            CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION
        }
    }

    if ( state != AL_PLAYING && state != AL_PAUSED ) {
        ALint queued;

        /* If no buffers are queued, playback is finished */
        alGetSourcei( m_source, AL_BUFFERS_QUEUED, &queued );
        if ( queued == 0 ) {
            return;
        }

        alSourcePlay( m_source );
        if ( alGetError() != AL_NO_ERROR ) {
            fprintf( stderr, "Error restarting playback\n" );
            return;
        }
    }
}
