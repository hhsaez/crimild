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

#include "Audio/OpenALWavAudioSource.hpp"

#include "Foundation/Log.hpp"
#include "Foundation/OpenALUtils.hpp"

using namespace crimild;
using namespace crimild::audio;

/*
 * Struct that holds the RIFF data of the Wave file.
 * The RIFF data is the meta data information that holds,
 * the ID, size and format of the wave file
 */
struct RIFF_Header {
    char chunkID[ 4 ];
    int chunkSize; //size not including chunkSize or chunkID
    char format[ 4 ];
};

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
struct WAVE_Format {
    char subChunkID[ 4 ];
    int subChunkSize;
    short audioFormat;
    short numChannels;
    int sampleRate;
    int byteRate;
    short blockAlign;
    short bitsPerSample;
};

/*
* Struct to hold the data of the wave file
*/
struct WAVE_Data {
    char subChunkID[ 4 ]; //should contain the word data
    int subChunk2Size;    //Stores the size of the data block
};

OpenALWavAudioSource::OpenALWavAudioSource( std::string filename ) noexcept
{
    WAVE_Format wave_format;
    RIFF_Header riff_header;
    WAVE_Data wave_data;

    FILE *soundFile = fopen( filename.c_str(), "rb" );
    if ( !soundFile ) {
        CRIMILD_LOG_ERROR( "Cannot open file ", filename );
        return;
    }

    // Read in the first chunk into the struct
    fread( &riff_header, sizeof( RIFF_Header ), 1, soundFile );

    //check for RIFF and WAVE tag in memeory
    if ( riff_header.chunkID[ 0 ] != 'R' || riff_header.chunkID[ 1 ] != 'I' || riff_header.chunkID[ 2 ] != 'F' || riff_header.chunkID[ 3 ] != 'F' ) {
        CRIMILD_LOG_ERROR( "Invalid RIFF header: ", riff_header.chunkID );
        return;
    }

    if ( riff_header.format[ 0 ] != 'W' || riff_header.format[ 1 ] != 'A' || riff_header.format[ 2 ] != 'V' || riff_header.format[ 3 ] != 'E' ) {
        CRIMILD_LOG_ERROR( "Invalid WAVE header: ", riff_header.format );
        return;
    }

    //Read in the 2nd chunk for the wave info
    fread( &wave_format, sizeof( WAVE_Format ), 1, soundFile );

    //check for fmt tag in memory
    if ( wave_format.subChunkID[ 0 ] != 'f' || wave_format.subChunkID[ 1 ] != 'm' || wave_format.subChunkID[ 2 ] != 't' || wave_format.subChunkID[ 3 ] != ' ' ) {
        CRIMILD_LOG_ERROR( "Invalid Wave format", wave_format.subChunkID );
        return;
    }

    //check for extra parameters;
    if ( wave_format.subChunkSize > 16 ) {
        fseek( soundFile, sizeof( short ), SEEK_CUR );
    }

    //Read in the the last byte of data before the sound file
    fread( &wave_data, sizeof( WAVE_Data ), 1, soundFile );
    //check for data tag in memory
    if ( wave_data.subChunkID[ 0 ] != 'd' || wave_data.subChunkID[ 1 ] != 'a' || wave_data.subChunkID[ 2 ] != 't' || wave_data.subChunkID[ 3 ] != 'a' ) {
        CRIMILD_LOG_ERROR( "Invalid data header", wave_data.subChunkID );
        return;
    }

    //Allocate memory for data
    std::vector< unsigned char > data( wave_data.subChunk2Size );

    // Read in the sound data into the soundData variable
    if ( !fread( &data[ 0 ], data.size() * sizeof( unsigned char ), 1, soundFile ) ) {
        CRIMILD_LOG_ERROR( "Error loading WAVE data into struct" );
        return;
    }

    fclose( soundFile );

    CRIMILD_LOG_DEBUG( "WAV file loaded: ", filename );

    auto numChannels = wave_format.numChannels;
    auto bitsPerSample = wave_format.bitsPerSample;
    auto frequency = wave_format.sampleRate;

    //load( wave_format.numChannels, wave_format.bitsPerSample, wave_format.sampleRate, data.size(), &data[ 0 ] );

    CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

    ALenum format = AL_FORMAT_MONO8;

    //The format is worked out by looking at the number of
    //channels and the bits per sample.
    if ( numChannels == 1 ) {
        if ( bitsPerSample == 8 ) {
            format = AL_FORMAT_MONO8;
        } else if ( bitsPerSample == 16 ) {
            format = AL_FORMAT_MONO16;
        }
    } else if ( numChannels == 2 ) {
        if ( bitsPerSample == 8 ) {
            format = AL_FORMAT_STEREO8;
        } else if ( bitsPerSample == 16 ) {
            format = AL_FORMAT_STEREO16;
        }
    }

    //create our openAL buffer and check for success
    alGenBuffers( 1, &m_buffer );
    alBufferData( m_buffer, format, ( void * ) data.data(), data.size(), frequency );

    alGenSources( 1, &m_source );
    alSourcei( m_source, AL_BUFFER, m_buffer );

    CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

OpenALWavAudioSource::~OpenALWavAudioSource( void ) noexcept
{
    if ( m_source > 0 ) {
        alDeleteSources( 1, &m_source );
    }

    if ( m_buffer > 0 ) {
        alDeleteBuffers( 1, &m_buffer );
    }
}

void OpenALWavAudioSource::play( void )
{
    CRIMILD_CHECK_AL_ERRORS_BEFORE_CURRENT_FUNCTION;

    if ( getStatus() != AudioSource::Status::PLAYING ) {
        alSourcePlay( m_source );
    }

    CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenALWavAudioSource::pause( void )
{
    if ( getStatus() == AudioSource::Status::PLAYING ) {
        alSourcePause( m_source );
    }
}

void OpenALWavAudioSource::stop( void )
{
    if ( getStatus() == AudioSource::Status::PLAYING ) {
        alSourceStop( m_source );
    }
}

crimild::Real32 OpenALWavAudioSource::getDuration( void ) const
{
    return 0;
}

void OpenALWavAudioSource::setLoop( crimild::Bool loop )
{
    alSourcei( m_source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE );
}

crimild::Bool OpenALWavAudioSource::shouldLoop( void ) const
{
    return false;
}

AudioSource::Status OpenALWavAudioSource::getStatus( void ) const
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

void OpenALWavAudioSource::setPlayingOffset( crimild::Real32 offset )
{
}

crimild::Real32 OpenALWavAudioSource::getPlayingOffset( void ) const
{
    return 0;
}

void OpenALWavAudioSource::setVolume( crimild::Real32 volume )
{
    alSourcef( m_source, AL_GAIN, volume );
}

crimild::Real32 OpenALWavAudioSource::getVolume( void ) const
{
    return 0;
}

void OpenALWavAudioSource::setTransformation( const Transformation &t )
{
    AudioSource::setTransformation( t );
}

void OpenALWavAudioSource::setRelativeToListener( crimild::Bool relative )
{
}

crimild::Bool OpenALWavAudioSource::isRelativeToListener( void ) const
{
    return false;
}

void OpenALWavAudioSource::setMinDistance( crimild::Real32 distance )
{
}

crimild::Real32 OpenALWavAudioSource::getMinDistance( void ) const
{
    return 0;
}

void OpenALWavAudioSource::setAttenuation( crimild::Real32 attenuation )
{
}

crimild::Real32 OpenALWavAudioSource::getAttenuation( void ) const
{
    return 0;
}

void OpenALWavAudioSource::onGetData( AudioSource::GetDataCallback const & )
{
}

crimild::UInt32 OpenALWavAudioSource::getChannelCount( void ) const
{
    return 0;
}

crimild::UInt32 OpenALWavAudioSource::getSampleRate( void ) const
{
    return 0;
}
