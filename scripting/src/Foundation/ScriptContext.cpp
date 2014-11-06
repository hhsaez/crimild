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

#include "ScriptContext.hpp"

#include <sstream>

using namespace crimild;
using namespace crimild::scripting;

ScriptContext::ScriptContext( void )
	: ScriptContext( false )
{
	reset();
}

ScriptContext::ScriptContext( bool openDefaultLibs )
	: _state( nullptr ),
	  _openDefaultLibs( openDefaultLibs )
{
	reset();
}

ScriptContext::~ScriptContext( void )
{
	if ( _state != nullptr ) {
		lua_close( _state );
		_state = nullptr;
	}
}

void ScriptContext::reset( void )
{
	if ( _state != nullptr ) {
		lua_close( _state );
		_state = nullptr;
	}

	_state = luaL_newstate();
	if ( _openDefaultLibs ) {
		luaL_openlibs( _state );
	}

	registerFunction(
		"getRuntimeDirectory",
		std::function< std::string ( void ) >( []( void ) -> std::string {
			return FileSystem::getInstance().getBaseDirectory();
		}));

	// this trick makes sure the runtime directory is in the search
	// path for external modules
	parse( "package.path = getRuntimeDirectory() .. '/?.lua;' .. package.path" );
}

bool ScriptContext::load( std::string fileName )
{
	if ( luaL_loadfile( _state, fileName.c_str() ) ) {
		Log::Error << "Cannot load file '" << fileName << "'" 
				   << "\n\tReason: "
				   << read< std::string >()
				   << Log::End;
		return false;
	}

	if ( lua_pcall( _state, 0, 0, 0 ) ) {
		Log::Error << "Cannot execute file '" << fileName << "'"
				   << "\n\tReason: " 
				   << read< std::string >()
				   << Log::End;
	    return false;
	}

	return true;
}

bool ScriptContext::parse( std::string text )
{
	if ( luaL_loadstring( _state, text.c_str() ) || lua_pcall( _state, 0, 0, 0 ) ) {
		Log::Error << "Cannot execute \"" << text << "\""
				   << "\n\tReason: " 
				   << read< std::string >()
				   << Log::End;
	    return false;
	}

	return true;
}

std::string ScriptContext::dumpStack( void )
{
	return LuaUtils::dumpStack( _state );
}

void ScriptContext::foreach( const std::string &name, std::function< void( ScriptContext &, ScriptContext::Iterable &i ) > callback )
{
	int count = eval< int >( "#" + name );
	for ( int i = 0; i < count; i++ ) {
		Iterable it( *this, name, i );
		callback( *this, it );
	}
}

