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

#include "Animation.hpp"

#include "Clip.hpp"

using namespace crimild;
using namespace crimild::animation;

Animation::Animation( std::string name )
    : NamedObject( name )
{
}

Animation::Animation( SharedPointer< Clip > const &clip, crimild::Real32 offset, crimild::Real32 duration )
    : NamedObject( clip->getName() ),
      _duration( duration ),
      _frameRate( clip->getFrameRate() ),
      _offset( offset ),
      _clip( clip )
{
    if ( _duration <= 0.0 ) {
        setDuration( clip->getDuration() );
    }
}

Animation::~Animation( void )
{
}

Animation *Animation::update( const Clock &c )
{
    _clock += c;
    evaluate();
    return this;
}

Animation *Animation::sync( SharedPointer< Animation > const &other )
{
    return sync( crimild::get_ptr( other ) );
}

Animation *Animation::sync( Animation *other )
{
    // TODO: Sync should work with time scales, not absolute values
    _clock = other->getClock();
    evaluate();
    return this;
}

void Animation::evaluate( void )
{
    auto t = _clock.getAccumTime() * getFrameRate();

    auto animationTime = _offset;

    switch ( getPlaybackMode() ) {
        case PlaybackMode::REPEAT: {
            animationTime += fmod( t, _duration );
            break;
        }

        case PlaybackMode::MIRROR_REPEAT: {
            // triangular wave with period 'duration'
            animationTime += _duration - Numericf::fabs( fmod( t, 2.0 * _duration ) - _duration );
            break;
        }

        default: {
            animationTime += Numericf::clamp( t, 0.0, _duration );
            break;
        }
    }

    getClip()->evaluate( animationTime, this );
}

Animation *Animation::lerp( SharedPointer< Animation > const &other, crimild::Real32 factor, crimild::Bool sync )
{
    return lerp( crimild::get_ptr( other ), factor, sync );
}

Animation *Animation::lerp( Animation *other, crimild::Real32 factor, crimild::Bool sync )
{
    if ( factor == 0.0f ) {
        return this;
    }

    if ( sync ) {
        other->sync( this );
    }

    _accumulators.each( [ other, factor ]( const std::string &channelName, SharedPointer< Accumulator > &acc ) {
        if ( other->_accumulators.contains( channelName ) ) {
            acc->lerp( crimild::get_ptr( other->_accumulators[ channelName ] ), factor );
        }
    } );

    return this;
}

Animation *Animation::add( SharedPointer< Animation > const &other, crimild::Real32 strength )
{
    return add( crimild::get_ptr( other ), strength );
}

Animation *Animation::add( Animation *other, crimild::Real32 strength )
{
    _accumulators.each( [ other, strength ]( const std::string &channelName, SharedPointer< Accumulator > &acc ) {
        if ( other->_accumulators.contains( channelName ) ) {
            acc->add( crimild::get_ptr( other->_accumulators[ channelName ] ), strength );
        }
    } );

    return this;
}

void Animation::encode( coding::Encoder &encoder )
{
    Codable::encode( encoder );

    encoder.encode( "duration", _duration );
    encoder.encode( "frameRate", _frameRate );
    encoder.encode( "offset", _offset );

    std::string playbackModeStr;
    switch ( _playbackMode ) {
        case PlaybackMode::REPEAT: {
            playbackModeStr = "repeat";
            break;
        }

        case PlaybackMode::MIRROR_REPEAT: {
            playbackModeStr = "mirror_repeat";
            break;
        }

        default: {
            playbackModeStr = "once";
            break;
        }
    }
    encoder.encode( "playbackMode", playbackModeStr );

    encoder.encode( "clip", _clip );
}

void Animation::decode( coding::Decoder &decoder )
{
    Codable::decode( decoder );

    decoder.decode( "duration", _duration );
    decoder.decode( "frameRate", _frameRate );
    decoder.decode( "offset", _offset );

    std::string playbackModeStr;
    decoder.decode( "playbackMode", playbackModeStr );
    if ( playbackModeStr == "once" ) {
        setPlaybackMode( PlaybackMode::ONCE );
    } else if ( playbackModeStr == "mirror_repeat" ) {
        setPlaybackMode( PlaybackMode::MIRROR_REPEAT );
    } else {
        setPlaybackMode( PlaybackMode::REPEAT );
    }

    decoder.decode( "clip", _clip );
}
