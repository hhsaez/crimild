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

#include "JSONEncoder.hpp"

#include <Coding/Codable.hpp>

using namespace crimild;
using namespace crimild::coding;
using namespace nlohmann;

crimild::Bool JSONEncoder::encode( SharedPointer< Codable > const &codable )
{
    // This is wrong!!! It should be handled by arrays instead
    json temp;
    if ( m_json != nullptr ) {
        temp = m_json;
        m_json = json {};
    }

    m_json[ "__CLASS_NAME" ] = codable->getClassName();
    m_json[ "__id" ] = codable->getUniqueID();

    codable->encode( *this );

    if ( temp != nullptr ) {
        m_json = temp;
    }

    return true;
}

crimild::Bool JSONEncoder::encode( std::string key, SharedPointer< Codable > const &codable )
{
    if ( codable == nullptr ) {
        return false;
    }

    encodeKey( key );
    encode( codable );
    // _ss << ", ";

    return true;
}

crimild::Bool JSONEncoder::encode( std::string key, const Transformation &value )
{
    /*
	encodeKey( key );

    _ss << "{ ";
    _indentLevel++;

    encode( "translate", value.getTranslate() );
    encode( "rotate_q", value.getRotate() );
    encode( "scale", value.getScale() );

    _indentLevel--;
	_ss << getIndentSpaces() << "}, ";
    */

    // assert( false );

    return true;
}

void JSONEncoder::encodeArrayBegin( std::string key, crimild::Size count )
{
    // _arrayKeys.push( key );

    // _ss << getIndentSpaces() << key << " = { ";

    // ++_indentLevel;
}

std::string JSONEncoder::beginEncodingArrayElement( std::string key, crimild::Size index )
{
    return key;
}

void JSONEncoder::endEncodingArrayElement( std::string key, crimild::Size index )
{
    // no-op
}

void JSONEncoder::encodeArrayEnd( std::string key )
{
    // _arrayKeys.pop();

    // --_indentLevel;
    // _ss << getIndentSpaces() << "},";
}

crimild::Bool JSONEncoder::encodeKey( std::string key )
{
    // _ss << getIndentSpaces();
    // if ( _arrayKeys.empty() || key.find( _arrayKeys.top() ) != 0 ) {
    //     _ss << key << " = ";
    // }

    return true;
}

std::string JSONEncoder::getIndentSpaces( void )
{
    std::string res = "\n";
    for ( crimild::Size i = 0; i < _indentLevel; i++ ) {
        res += "    ";
    }
    return res;
}

std::string JSONEncoder::dump( void )
{
    return _ss.str();
}
