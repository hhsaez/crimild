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

#include "ScriptingContext.hpp"

using namespace crimild;
using namespace crimild::scripting;

ScriptingContext::ScriptingContext( void )
{
	_luaState = luaL_newstate();
	luaL_openlibs( _luaState );
}

ScriptingContext::~ScriptingContext( void )
{
	if ( _luaState != nullptr ) {
		lua_close( _luaState );
	}
}

bool ScriptingContext::loadScript( std::string fileName )
{
	if ( luaL_loadfile( _luaState, fileName.c_str() ) != 0 ) {
		Log::Error << "Cannot load script file " << fileName << Log::End;
		return false;
	} 

	if ( lua_pcall( _luaState, 0, 0, 0 ) != 0 ) {
		Log::Error << "Cannot initialize script file " << fileName << Log::End;
		return false;
	}

	return true;
}

void ScriptingContext::parseScript( std::string script )
{
	luaL_loadstring( _luaState, script.c_str() );
	lua_pcall( 0, 0, 0, 0 );
}

void ScriptingContext::cleanStack( void )
{
	int n = lua_gettop( _luaState );
	lua_pop( _luaState, n );
}

bool ScriptingContext::popStack( const std::string &name ) 
{
	_level = 0;
	std::string var = "";

	for ( unsigned int i = 0; i < name.size(); i++ ) {
		char c = name.at( i );
		if ( c == '.' ) {
			if ( _level == 0 ) {
				lua_getglobal( _luaState, var.c_str() );
			}
			else {
				lua_getfield( _luaState, -1, var.c_str() );
			}

			if ( lua_isnil( _luaState, -1 ) ) {
				crimild::Log::Error << "Variable " << var << " is not defined" << Log::End;
				return false;
			}
			else {
				var = "";
				_level++;
			}
		}
		else {
			var += c;
		}
	}

	if ( _level == 0 ) {
		lua_getglobal( _luaState, var.c_str() );
	}
	else {
		lua_getfield( _luaState, -1, var.c_str() );
	}

	if ( lua_isnil( _luaState, -1 ) ) {
		crimild::Log::Error << "Variable " << var << " is not defined" << Log::End;
		return false;
	}

	return true;
}

void ScriptingContext::registerFunction( std::string name, std::function< int ( ScriptingContext & ) > &callback )
{
	/*
	lua_pushcfunction( _luaState, [=]( lua_State *L ) -> int {
		//return callback( *this );
		return 0;
	});

	lua_setglobal( _luaState, name.c_str() ); 
	lua_pcall( _luaState, 0, 0, 0 ); 
	*/
}

