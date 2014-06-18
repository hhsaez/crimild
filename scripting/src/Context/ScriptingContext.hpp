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

#ifndef CRIMILD_SCRIPTING_CONTEXT_
#define CRIMILD_SCRIPTING_CONTEXT_

#include <Crimild.hpp>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

namespace crimild {

	namespace scripting {

		class ScriptingContext {
			CRIMILD_DISALLOW_COPY_AND_ASSIGN( ScriptingContext )

		public:
			ScriptingContext( void );

			~ScriptingContext( void );

			bool loadScript( std::string fileName );

			void parseScript( std::string script );

		public:
			template< typename T >
			T getValue( const std::string &variableName )
			{
				if ( _luaState == nullptr ) {
					crimild::Log::Error << "Scripting context is not initialized" << Log::End;
					return getDefault< T >();
				}

				T result = popStack( variableName ) ? get< T >( variableName ) : getDefault< T >();

				lua_pop( _luaState, _level + 1 );
				return result;
			}

			template< typename T >
			std::vector< T > getArray( const std::string& name ) {
    			std::vector< T > result;

    			if ( !popStack( name.c_str() ) ) {
    				Log::Error << "Array not found " << name << Log::End;
        			cleanStack();
        			return std::vector< T >();
    			}

    			lua_pushnil( _luaState );
    			while ( lua_next( _luaState, -2 ) ) {
    				if ( lua_isnumber( _luaState, -1 ) ) {
        				result.push_back( ( T ) lua_tonumber( _luaState, -1 ) );
        			}
        			lua_pop( _luaState, 1 );
    			}

    			cleanStack();
    			return result;
			}

		public:
			void beginFunctionCall( const std::string &name )
			{
				_argCount = 0;
				lua_getglobal( _luaState, name.c_str() );
			}

			template< typename T >
			void addFunctionArgument( T arg )
			{
				lua_pushnumber( _luaState, arg );
				_argCount++;
			}

			void addFunctionArgument( std::string arg )
			{
				lua_pushstring( _luaState, arg.c_str() );
				_argCount++;
			}

			void invokeFunction( bool waitForResult = true )
			{
				lua_pcall( _luaState, _argCount, waitForResult ? 1 : 0, 0 );
			}

			template< typename T >
			T getFunctionResult( void )
			{
				T result = ( T ) lua_tonumber( _luaState, -1 );
				lua_pop( _luaState, 1 );
				return result;
			}

		public:
			void registerFunction( std::string name, std::function< int ( ScriptingContext & ) > &callback );

		private:
			void cleanStack( void );

			bool popStack( const std::string &name );

		    template< typename T >
    		T get( const std::string &name ) 
    		{
    			// default
      			return 0;
    		}

		    template< typename T >
    		T getDefault( void ) {
      			return 0;
    		}

		private:
			lua_State *_luaState;
			int _level = 0;
			int _argCount = 0;
		};

		template<>
		inline std::vector< std::string > ScriptingContext::getArray( const std::string &name )
		{
			std::vector< std::string > result;

			if ( !popStack( name.c_str() ) ) {
				Log::Error << "Array not found " << name << Log::End;
    			cleanStack();
    			return std::vector< std::string >();
			}

			lua_pushnil( _luaState );
			while ( lua_next( _luaState, -2 ) ) {
				if ( lua_isstring( _luaState, -1 ) ) {
    				result.push_back( std::string( lua_tostring( _luaState, -1 ) ) );
    			}
    			lua_pop( _luaState, 1 );
			}

			cleanStack();
			return result;
		}

		template <>
		inline bool ScriptingContext::get( const std::string &name ) 
		{
    		return ( bool ) lua_toboolean( _luaState, -1 );
		}
 
		template <>
		inline float ScriptingContext::get( const std::string &name ) 
		{
    		if ( !lua_isnumber( _luaState, -1 ) ) {
    			crimild::Log::Error << name << " " << " is not a number" << crimild::Log::End;
    		}

    		return ( float ) lua_tonumber( _luaState, -1 );
		}
 
		template <>
		inline int ScriptingContext::get( const std::string &name ) 
		{
    		if ( !lua_isnumber( _luaState, -1 ) ) {
    			crimild::Log::Error << name << " " << " is not a number" << crimild::Log::End;
    		}
    		return ( int ) lua_tonumber( _luaState, -1 );
		}
 
		template <>
		inline std::string ScriptingContext::get( const std::string &name ) 
		{
    		std::string s = "null";
    		if ( lua_isstring( _luaState, -1 ) ) {
      			s = std::string( lua_tostring( _luaState, -1 ) );
    		} else {
      			crimild::Log::Error << name << " " << " is not a string" << crimild::Log::End;
    		}

    		return s;
		}

		template<>
		inline std::string ScriptingContext::getDefault( void )
		{
			return "null";
		}

		template<>
		inline std::string ScriptingContext::getFunctionResult( void )
		{
			std::string result = lua_tostring( _luaState, -1 );
			lua_pop( _luaState, 1 );
			return result;
		}

	}
	
}

#endif

