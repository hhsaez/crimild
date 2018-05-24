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

#include "LuaEncoder.hpp"

#include <Coding/Codable.hpp>

using namespace crimild;
using namespace crimild::coding;

LuaEncoder::LuaEncoder( void )
{

}

LuaEncoder::~LuaEncoder( void )
{

}
            
void LuaEncoder::encode( SharedPointer< Codable > const &codable ) 
{
	_ss << "{ ";
    _indentLevel++;
    
    encodeKey( "type" );
	_ss <<  "'" << codable->getClassName() << "', ";
	
    encodeKey( "id" );
    _ss << codable->getUniqueID() << ", ";
    
	codable->encode( *this );
    
    _indentLevel--;
	
    _ss << getIndentSpaces() << "}";
    
    if ( _arrayKeys.size() == 0 ) {
        _ss << "\n";
    }
}

void LuaEncoder::encode( std::string key, SharedPointer< Codable > const &codable ) 
{
    if ( codable == nullptr ) {
        return;
    }
    
	encodeKey( key );
	encode( codable );
	_ss << ", ";
}

void LuaEncoder::encode( std::string key, std::string value ) 
{
	encodeKey( key );
	_ss << "'" << value << "', ";
}

void LuaEncoder::encode( std::string key, const Transformation &value )
{
	encodeKey( key );
	
    _ss << "{ ";
    _indentLevel++;
    
    encode( "translate", value.getTranslate() );    
    encode( "rotate_q", value.getRotate() );    
    encode( "scale", value.getScale() );
    
    _indentLevel--;
	_ss << getIndentSpaces() << "}, ";
}

void LuaEncoder::encode( std::string key, const VertexFormat &value )
{
    // no-op
}

void LuaEncoder::encodeArrayBegin( std::string key, crimild::Size count )
{
	_arrayKeys.push( key );
    
    _ss << getIndentSpaces() << key << " = { ";
    
    ++_indentLevel;
}

std::string LuaEncoder::beginEncodingArrayElement( std::string key, crimild::Size index )
{
    return key;
}

void LuaEncoder::endEncodingArrayElement( std::string key, crimild::Size index )
{
	// no-op
}

void LuaEncoder::encodeArrayEnd( std::string key ) 
{
	_arrayKeys.pop();
    
    --_indentLevel;
	_ss << getIndentSpaces() << "},";
}

void LuaEncoder::encodeKey( std::string key )
{
    _ss << getIndentSpaces();
	if ( _arrayKeys.empty() || key.find( _arrayKeys.top() ) != 0 ) {
		_ss << key << " = ";
	}
}

std::string LuaEncoder::getIndentSpaces( void )
{
    std::string res = "\n";
    for ( crimild::Size i = 0; i < _indentLevel; i++ ) {
        res += "    ";
    }
    return res;
}

std::string LuaEncoder::dump( void )
{
    return _ss.str();
}

