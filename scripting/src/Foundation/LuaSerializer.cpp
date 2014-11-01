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

#include "LuaSerializer.hpp"

using namespace crimild;
using namespace crimild::scripting;

LuaSerializer::LuaSerializer( std::ostream &output )
	: _output( output ),
	  _depth( 0 )
{

}

LuaSerializer::~LuaSerializer( void )
{

}

void LuaSerializer::pushObject( std::string name )
{
	pushLine( name != "" ? ( name + " = {" ) : "{" );
	++_depth;
}

void LuaSerializer::popObject( void )
{
	--_depth;
	pushLine( _depth > 0 ? "}," : "}" );
}

void LuaSerializer::pushProperty( std::string name, const char *value ) 
{
	pushLine( name + " = '" + value + "', " );
}

void LuaSerializer::pushProperty( std::string name, std::string value ) 
{
	pushProperty( name, value.c_str() );
}

void LuaSerializer::pushProperty( std::string name, bool value )
{
	pushLine( name + " = " + ( value ? "true" : "false" ) + ", " );
}

void LuaSerializer::pushProperty( std::string name, const Vector3f &v )
{
	std::stringstream str;
	str << name << " = { "
		<< v[ 0 ] << ", " 
		<< v[ 1 ] << ", " 
		<< v[ 2 ] 
		<< " }, ";
	pushLine( str.str() );
}

void LuaSerializer::pushProperty( std::string name, const Quaternion4f &q )
{
	std::stringstream str;
	str << name << " = { "
		<< q.getImaginary()[ 0 ] << ", "
		<< q.getImaginary()[ 1 ] << ", "
		<< q.getImaginary()[ 2 ] << ", "
		<< q.getReal()
		<< " }, ";
	pushLine( str.str() );
}

void LuaSerializer::pushText( std::string text )
{
	_output << text;
}

void LuaSerializer::pushLine( std::string line )
{
	for ( int i = 0; i < 4 * _depth; i++ ) {
		_output << " ";
	}

	_output << line << "\n";
}

