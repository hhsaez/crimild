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

#include "Simulation/Systems/OpenALAudioSystem.hpp"

#include "Audio/OpenALOggAudioSource.hpp"
#include "Audio/OpenALWavAudioSource.hpp"
#include "Foundation/OpenALUtils.hpp"
#include "Foundation/Log.hpp"

namespace crimild {

    namespace audio {

        struct OpenALContext {
            ALCdevice_struct *device = nullptr;
            ALCcontext_struct *context = nullptr;
        };

    }

}

using namespace crimild;
using namespace crimild::audio;

void OpenALAudioSystem::start( void ) noexcept
{
    CRIMILD_LOG_INFO( "Starting OpenAL Audio System" );

    AudioSystem::start();

    m_context = crimild::alloc< OpenALContext >();

    CRIMILD_LOG_DEBUG( "Opening OpenAL device" );
    m_context->device = alcOpenDevice( nullptr );
    if ( m_context->device == nullptr ) {
        CRIMILD_LOG_ERROR( "Cannot open OpenAL device" );
        return;
    }

    CRIMILD_LOG_DEBUG( "Creating OpenAL context" );
    m_context->context = alcCreateContext( m_context->device, nullptr );
    if ( m_context == nullptr ) {
        CRIMILD_LOG_ERROR( "Cannot create OpenAL context" );
        return;
    }

    CRIMILD_LOG_DEBUG( "Setting OpenAL context as current" );
    alcMakeContextCurrent( m_context->context );

    CRIMILD_LOG_DEBUG( "OpenAL Audio System Started" );

    CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenALAudioSystem::update( void ) noexcept
{
    if ( m_context == nullptr ) {
        return;
    }

    m_oggSources.each(
        []( auto source ) {
            source->onUpdate();
        } );
}

void OpenALAudioSystem::stop( void ) noexcept
{
    if ( m_context == nullptr ) {
        return;
    }

    CRIMILD_LOG_INFO( "Terminating OpenAL Audio System" );

    AudioSystem::stop();

    if ( m_context->context ) {
        alcDestroyContext( m_context->context );
    }

    if ( m_context->device ) {
        alcCloseDevice( m_context->device );
    }

    m_context = nullptr;

    CRIMILD_CHECK_AL_ERRORS_AFTER_CURRENT_FUNCTION;
}

AudioListener *OpenALAudioSystem::getAudioListener( void ) noexcept
{
    return nullptr;
}

SharedPointer< audio::AudioSource > OpenALAudioSystem::createAudioSource( std::string filename, bool asStream ) noexcept
{
    auto ext = StringUtils::getFileExtension( filename );
    if ( ext == "ogg" ) {
        return crimild::alloc< OpenALOggAudioSource >( this, filename );
    } else if ( ext == "wav" ) {
        return crimild::alloc< OpenALWavAudioSource >( filename );
    } else {
        return nullptr;
    }
}

void OpenALAudioSystem::play( OpenALOggAudioSource *source ) noexcept
{
    if ( !m_oggSources.contains( source ) ) {
        m_oggSources.add( source );
    }
}

void OpenALAudioSystem::pause( OpenALOggAudioSource *source ) noexcept
{
    if ( m_oggSources.contains( source ) ) {
        m_oggSources.remove( source );
    }
}

void OpenALAudioSystem::stop( OpenALOggAudioSource *source ) noexcept
{
    if ( m_oggSources.contains( source ) ) {
        m_oggSources.remove( source );
    }
}
