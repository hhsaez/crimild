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

#include "LuaUtils.hpp"

using namespace crimild;
using namespace crimild::scripting;

void LuaUtils::cleanStack( lua_State *l )
{
	int n = lua_gettop( l );
    lua_pop( l, n );
}

bool LuaUtils::checkLuaState( lua_State *l, int code )
{
	if ( code != LUA_OK ) {
		return false;
	}

    Log::warning( CRIMILD_CURRENT_CLASS_NAME, lua_tostring( l, -1 ) );
	return true;
}

namespace crimild {

	namespace scripting {
		
		template<>
		bool LuaUtils::get< bool >( lua_State *l, const int index )
		{
			return lua_toboolean( l, index );
		}

		template<>
		int LuaUtils::get< int >( lua_State *l, const int index )
		{
			return lua_tointeger( l, index );
		}

		template<>
		unsigned int LuaUtils::get< unsigned int >( lua_State *l, const int index )
		{
			return lua_tounsigned( l, index );
		}

		template<>
		float LuaUtils::get< float >( lua_State *l, const int index )
		{
			return lua_tonumber( l, index );
		}

		template<>
		double LuaUtils::get< double >( lua_State *l, const int index )
		{
			return lua_tonumber( l, index );
		}

		template<>
		const char *LuaUtils::get< const char * >( lua_State *l, const int index )
		{
			return lua_tostring( l, index );
		}

		template<>
		std::string LuaUtils::get< std::string >( lua_State *l, const int index )
		{
			return lua_tostring( l, index );
		}

		template<>
		bool LuaUtils::checkGet< bool >( lua_State *l, const int index )
		{
			return lua_toboolean( l, index );
		}

		template<>
		int LuaUtils::checkGet< int >( lua_State *l, const int index )
		{
			return luaL_checkint( l, index );
		}

		template<>
		unsigned int LuaUtils::checkGet< unsigned int >( lua_State *l, const int index )
		{
			return luaL_checkunsigned( l, index );
		}

		template<>
		float LuaUtils::checkGet< float >( lua_State *l, const int index )
		{
			return luaL_checknumber( l, index );
		}

		template<>
		double LuaUtils::checkGet< double >( lua_State *l, const int index )
		{
			return luaL_checknumber( l, index );
		}

		template<>
		const char *LuaUtils::checkGet< const char * >( lua_State *l, const int index )
		{
			return luaL_checkstring( l, index );
		}

		template<>
		std::string LuaUtils::checkGet< std::string >( lua_State *l, const int index )
		{
			return luaL_checkstring( l, index );
		}

	}
}

void LuaUtils::push( lua_State *l )
{
	// do nothing
}

void LuaUtils::push( lua_State *l, bool &&b ) 
{
    lua_pushboolean( l, b );
}

void LuaUtils::push( lua_State *l, int &&i ) {
    lua_pushinteger( l, i );
}

void LuaUtils::push( lua_State *l, unsigned int &&u ) 
{
    lua_pushunsigned( l, u );
}

void LuaUtils::push( lua_State *l, float &&f ) 
{
    lua_pushnumber( l, f );
}

void LuaUtils::push( lua_State *l, double &&d ) 
{
    lua_pushnumber( l, d );
}

void LuaUtils::push( lua_State *l, std::string &&s ) 
{
    lua_pushlstring( l, s.c_str(), s.size() );
}

std::string LuaUtils::dumpStack( lua_State *l )
{
	std::stringstream str;

	int top = lua_gettop( l );

	str << "Total in stack: " << top << "\n";

	for ( int i = 1; i <= top; i++ ) {
		int t = lua_type( l, i );
		switch ( t ) {
			case LUA_TSTRING:
				str << "string: " << lua_tostring( l, i ) << "\n";
				break;
			case LUA_TBOOLEAN:
				str << "boolean: " << ( lua_toboolean( l, i ) ? "true" : "false" ) << "\n";
				break;
			case LUA_TNUMBER:
				str << "number: " << lua_tonumber( l, i ) << "\n";
				break;
			default:
				str << lua_typename( l, t ) << "\n";
				break;
		}

		str << " ";
	}

	str << "\n";

	return str.str();
}

std::string LuaUtils::getErrorDescription( lua_State *l )
{
	/*
	std::string msg = 

	lua_getglobal(l, "debug");
  	lua_getfield(l, -1, "traceback");
  	lua_remove(l, -2);
  	int errindex = -p_iArgCount - 2;
  	lua_insert(L, errindex);
  	int error = lua_pcall(L, p_iArgCount, return_amount, errindex);


	if ( l && !lua_isnil( l, -1 ) ) {
		std::string msg = lua_tostring( l, -1 );
		if ( msg == "" ) {
			msg = "(error object is not a string)";
		}

		lua_getfield( l, LUA_GLOBALSINDEX, "debug" );
		if ( !lua_istable( l, -1 ) ) {
			lua_pop( l, 1 );
			return "cannot get debug table";
		}

		lua_getfield( l, -1, "traceback" );
		if ( !lua_isfunction( l, -1 ) ) {
			lua_pop( l, 2 );
			return "cannot get traceback";
		}

		lua_pushvalue( l, 1 );
		lua_pushinteger( l, 2 );
		lua_call( l, 2, 1 );

		msg += "\n[LUA ERROR]: ";
		msg += lua_tounsigned( l, -1 );

		lua_pop( l, 1 );

		return msg;
	}
	*/
	return "unknown error";
}

