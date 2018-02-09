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

#ifndef CRIMILD_SCRIPTING_FOUNDATION_LUA_UTILS_
#define CRIMILD_SCRIPTING_FOUNDATION_LUA_UTILS_

#include <string>
#include <tuple>
#include <functional>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

namespace crimild {

	namespace scripting {

		class LuaUtils {
		public:
			static void cleanStack( lua_State *l );

		public:
			static bool checkLuaState( lua_State *l, int code );

		public:
			template< typename T >
			static T get( lua_State *l, const int index );

			template< typename T >
			static T checkGet( lua_State *l, const int index );

		public:
			template< std::size_t... Is >
			struct IndicesTag { };

			template< std::size_t N, std::size_t... Is >
			struct IndicesTagBuilder : IndicesTagBuilder< N - 1, N - 1, Is... > { };

			template< std::size_t... Is >
			struct IndicesTagBuilder< 0, Is... > {
				using Type = IndicesTag< Is... >;
			};

			template< typename... T, std::size_t... N >
			static std::tuple< T... > getArgs( lua_State *l, IndicesTag< N... > )
			{
				return std::make_tuple( LuaUtils::checkGet< T >( l, N + 1 )... );
			}

			template< typename... T >
			static std::tuple< T... > getArgs( lua_State *l )
			{
				constexpr std::size_t argCount = sizeof...( T );
				return getArgs< T... >( l, typename IndicesTagBuilder< argCount >::Type() );
			}

			template< typename Ret, typename... Args, std::size_t... N >
			static Ret lift( std::function< Ret( Args... ) > func, std::tuple< Args... > args, IndicesTag< N... > )
			{
				return func( std::get< N >( args )... );
			}

			template< typename Ret, typename... Args >
			static Ret lift( std::function< Ret( Args... ) > func, std::tuple< Args... > args )
			{
				return lift( func, args, typename IndicesTagBuilder< sizeof... ( Args ) >::Type() );
			}

		public:
			static void push( lua_State *l );
			static void push( lua_State *l, bool &&value );
			static void push( lua_State *l, int &&value );
			static void push( lua_State *l, unsigned int &&value );
			static void push( lua_State *l, float &&value );
			static void push( lua_State *l, double &&value );
			static void push( lua_State *l, std::string &&value );

		private:
			inline static void pushValues( lua_State * ) { }

			template< typename T, typename... Tail >
			static void pushValues( lua_State *l, T value, Tail... tail )
			{
				push( l, std::forward< T >( value ) );
				pushValues( l, tail... );
			}

			template< typename... Ts, std::size_t... N >
			static void pushDispatcher( lua_State *l, std::tuple< Ts... > &&values, IndicesTag< N... > )
			{
				pushValues( l, std::get< N >( values )... );
			}

		public:
			template< typename... Ts >
			static void push( lua_State *l, std::tuple< Ts... > &&values )
			{
				constexpr int valuesCount = sizeof...( Ts );
				lua_settop( l, valuesCount );

				pushDispatcher( l, std::forward< std::tuple< Ts... > >( values ), typename LuaUtils::IndicesTagBuilder< valuesCount >::Type() );
			}

		public:
			template < std::size_t, typename... Ts >
			struct PopTrait {
    			typedef std::tuple< Ts... > Type;

    			template < typename T >
    			static std::tuple< T > worker( lua_State *l, const int index ) {
        			return std::make_tuple( get< T >( l, index ) );
    			}

    			template < typename T1, typename T2, typename... Rest >
    			static std::tuple< T1, T2, Rest... > worker( lua_State *l, const int index ) {
        			std::tuple< T1 > head = std::make_tuple( get< T1 >( l, index ) );
        			return std::tuple_cat( head, worker< T2, Rest... >( l, index + 1 ) );
    			}

    			static Type apply( lua_State *l ) {
        			auto ret = worker< Ts... >( l, 1 );
        			lua_pop( l, ( int ) sizeof...( Ts ) );
        			return ret;
    			}
			};

			template < typename... Ts >
			struct PopTrait< 0, Ts... > {
    			typedef void Type;
    			static Type apply( lua_State * ) { }
			};

			template < typename T >
			struct PopTrait< 1, T > {
    			typedef T Type;

    			static Type apply( lua_State *l ) {
        			T ret = get< T >( l, -1 );
        			lua_pop( l, 1 );
        			return ret;
    			}
			};

			template < typename... T >
			static typename PopTrait< sizeof...( T ), T... >::Type pop( lua_State *l ) {
    			return PopTrait<sizeof...( T ), T... >::apply( l );
			}

		public:
			static std::string dumpStack( lua_State *l );

		public:
			std::string getErrorDescription( lua_State *l );
		};

	}
}

#endif

