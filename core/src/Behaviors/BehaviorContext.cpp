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

#include "Behavior.hpp"
#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"

using namespace crimild;
using namespace crimild::behaviors;

BehaviorContextValue::BehaviorContextValue( void )
{
}

BehaviorContextValue::BehaviorContextValue( std::string key, std::string value )
    : _key( key ),
      _value( value )
{
}

BehaviorContextValue::~BehaviorContextValue( void )
{
}

void BehaviorContextValue::encode( coding::Encoder &encoder )
{
    Codable::encode( encoder );

    encoder.encode( "key", _key );
    encoder.encode( "value", _value );
}

void BehaviorContextValue::decode( coding::Decoder &decoder )
{
    Codable::decode( decoder );

    decoder.decode( "key", _key );
    decoder.decode( "value", _value );
}

BehaviorContext::BehaviorContext( void )
{
}

BehaviorContext::~BehaviorContext( void )
{
    removeAllTargets();
}

void BehaviorContext::reset( void )
{
    _agent = nullptr;
    _clock.reset();
    removeAllTargets();
}

void BehaviorContext::update( const crimild::Clock &c )
{
    _clock += c;
}

void BehaviorContext::addTarget( crimild::Node *target )
{
    // TODO: check repetitions? use set?
    _targets.push_back( target );
    ++_targetCount;
}

crimild::Size BehaviorContext::getTargetCount( void ) const
{
    return _targetCount;
}

crimild::Node *BehaviorContext::getTargetAt( crimild::Size index )
{
    if ( getTargetCount() <= index ) {
        return nullptr;
    }

    return _targets[ index ];
}

void BehaviorContext::removeAllTargets( void )
{
    _targets.clear();
    _targetCount = 0;
}

void BehaviorContext::foreachTarget( BehaviorContext::TargetCallback const &callback )
{
    for ( auto &t : _targets ) {
        if ( t != nullptr ) {
            callback( t );
        }
    }
}

void BehaviorContext::encode( coding::Encoder &encoder )
{
    Codable::encode( encoder );

    crimild::Array< std::string > keys;
    crimild::Array< SharedPointer< Variant > > values;
    for ( auto &it : m_values ) {
        keys.add( it.first );
        values.add( it.second );
    }
    encoder.encode( "keys", keys );
    encoder.encode( "values", values );
}

void BehaviorContext::decode( coding::Decoder &decoder )
{
    Codable::decode( decoder );

    crimild::Array< std::string > keys;
    decoder.decode( "keys", keys );

    crimild::Array< SharedPointer< Variant > > values;
    decoder.decode( "values", values );

    m_values.clear();
    for ( size_t i = 0; i < keys.size(); ++i ) {
        auto key = keys[ i ];
        auto value = values[ i ];
        m_values[ key ] = value;
    }
}

void BehaviorContext::dump( void ) const
{
    std::stringstream ss;

    _values.each( [ &ss ]( std::string, const SharedPointer< BehaviorContextValue > &value ) {
        ss << "\n\t\"" << value->getKey() << "\" = \"" << value->getValue() << "\"";
    } );

    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Behavior context dump: ", ss.str() );
}
