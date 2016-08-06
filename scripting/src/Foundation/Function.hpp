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

#ifndef CRIMILD_SCRIPTING_FOUNDATION_FUNCTION_
#define CRIMILD_SCRIPTING_FOUNDATION_FUNCTION_

#include "LuaUtils.hpp"

#include <Crimild.hpp>
#include <tuple>
#include <functional>

namespace crimild {

	namespace scripting {

		class AbstractFunction {
		public:
			virtual ~AbstractFunction( void ) { }

			virtual int apply( lua_State * ) = 0;
		};

		int luaDispatcher( lua_State *state );

		template< int N, typename ReturnType, typename... Args >
		class Function : public AbstractFunction {
		private:
			typedef std::function< ReturnType( Args... ) > FunctionType;

			FunctionType _function;
			std::string _name;
			lua_State **_state;

		public:
			Function( lua_State *&l, const std::string &name, ReturnType( *func )( Args... ) )
				: Function( l, name, FunctionType { func } )
			{

			}

			Function( lua_State *&l, const std::string &name, FunctionType func )
				: _function( func ),
				  _name( name ),
				  _state( &l )
			{
			    lua_pushlightuserdata( l, ( void * ) static_cast< AbstractFunction * >( this ) );
			    lua_pushcclosure( l, &luaDispatcher, 1 );
			    lua_setglobal( l, name.c_str() );
			}

			virtual ~Function( void )
			{
				if ( _state != nullptr && ( *_state ) != nullptr ) {
					lua_pushnil( *_state );
					lua_setglobal( *_state, _name.c_str() );
				}
			}

			int apply( lua_State * )
			{
				std::tuple< Args... > args = LuaUtils::getArgs< Args... >( *_state );
				ReturnType value = LuaUtils::lift( _function, args, typename LuaUtils::IndicesTagBuilder< sizeof...( Args ) >::Type() );
				LuaUtils::push( *_state, std::forward< ReturnType >( value ) );
				return N;
			}

		};

	}

}

#endif

