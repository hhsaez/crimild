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

#ifndef CRIMILD_SCRIPTING_FOUNDATION_SCRIPT_CONTEXT_
#define CRIMILD_SCRIPTING_FOUNDATION_SCRIPT_CONTEXT_

#include "Function.hpp"

#include <Crimild.hpp>

#include <string>
#include <tuple>

namespace crimild {

	namespace scripting {

		class ScriptContext {
			CRIMILD_DISALLOW_COPY_AND_ASSIGN( ScriptContext )

		public:
			ScriptContext( void );
			ScriptContext( bool openDefaultLibs );
			virtual ~ScriptContext( void );

			void reset( void );

		private:
			lua_State *_state;
			bool _openDefaultLibs;

		public:
			bool load( std::string fileName, bool supportCoroutines = false );

			bool parse( std::string text );

		public:
			int yield( void );
			int resume( void );

		private:
			lua_State *_backgroundThreadState;

		public:
			void push( void ) { }

			template< typename T, typename... Ts >
			void push( T &&value, Ts &&... values )
			{
				LuaUtils::push( _state, std::forward< T >( value ) );
				push( std::forward< Ts >( values )... );
			}

		public:
			template< typename T >
			T read( const int index = -1 ) const
			{
				return LuaUtils::get< T >( _state, index );
			}

		public:
			bool test( const std::string &expr )
			{
				std::stringstream str;
				str << "evalExpr = " << expr;
				if ( !luaL_dostring( _state, str.str().c_str() ) ) {
					lua_getglobal( _state, "evalExpr" );
					return !lua_isnil( _state, -1 );
				}

				return false;
			}

			template< typename T >
			T eval( const std::string &expr )
			{
				std::stringstream str;
				str << "evalExpr = " << expr;
				if ( !luaL_dostring( _state, str.str().c_str() ) ) {
					lua_getglobal( _state, "evalExpr" );
					T ret = lua_isnil( _state, -1 ) ? getDefaultValue< T >() : read< T >( -1 );
					lua_pop( _state, 1 );
					return ret;
				}

				return getDefaultValue< T >();
			}

		private:
			template< typename T >
			T getDefaultValue( void ) const { return 0; }

		public:
			class Iterable {
			public:
				Iterable( ScriptContext &context, const std::string &prefix, int index ) 
					: _context( context ), 
					  _index( index )
				{ 
					std::stringstream str;
					str << prefix;
					if ( _index >= 0 ) {
						str << "[" << ( _index + 1 ) << "]";
					}
					_prefix = str.str();
				}
				
				virtual ~Iterable( void ) { }

				const std::string &getPrefix( void ) const { return _prefix; }
				
				int getIndex( void ) const { return _index; }
				
				ScriptContext *getContext( void ) { return &_context; }

				bool test( const std::string &expr )
				{
					return _context.test( expandExpression( expr ) );
				}

				template< typename T >
				T eval( void )
				{
					return _context.eval< T >( _prefix );
				}

				template< typename T >
				T eval( const std::string &expr )
				{
					return _context.eval< T >( expandExpression( expr ) );
				}

				void foreach( const std::string &name, std::function< void( ScriptContext &, ScriptContext::Iterable &i ) > callback )
				{
					_context.foreach( expandExpression( name ), callback );
				}

				std::string expandExpression( std::string expr ) 
				{
					if ( _prefix == "" ) {
						return expr;
					}

					return _prefix + "." + expr;
				}

			private:
				ScriptContext &_context;
				std::string _prefix;
				int _index;
			};

			void foreach( const std::string &name, std::function< void( ScriptContext &, ScriptContext::Iterable &i ) > callback );

		public:
			std::string dumpStack( void );

		public:
			bool isNil( const std::string &global )
			{
				lua_getglobal( _state, global.c_str() );
				const bool result = lua_isnil( _state, -1 );
				lua_pop( _state, 1 );
				return result;
			}

		public:
			template< typename... T >
			typename LuaUtils::PopTrait< sizeof...( T ), T... >::Type pop( void )
			{
				return LuaUtils::pop< T... >( _state );
			}

		public:
			template< typename... Ret, typename... Args >
			typename LuaUtils::PopTrait< sizeof...( Ret ), Ret... >::Type invoke( const std::string &name, Args &&... args )
			{
				lua_getglobal( _state, name.c_str() );

				constexpr int argCount = sizeof...( Args );
				constexpr int retCount = sizeof...( Ret );

				push( std::forward< Args >( args )... );

				lua_call( _state, argCount, retCount );

				return pop< Ret... >();
			}

		public:
			template< typename Ret, typename... Args >
			void registerFunction( const std::string &name, std::function< Ret( Args... ) > func )
			{
				auto tmp = std::unique_ptr< AbstractFunction > (
					new Function< 1, Ret, Args... > {
						_state,
						name, 
						func
					});

				_functions.insert( std::make_pair( name, std::move( tmp ) ) );
			}

			template < typename... Ret, typename... Args >
 			void registerFunction( const std::string &name, std::function< std::tuple< Ret... >( Args... ) > func ) 
 			{
 				constexpr int returnCount = sizeof...( Ret );
 				auto tmp = std::unique_ptr< AbstractFunction > (
 					new Function< returnCount, std::tuple< Ret... >, Args... >{
 						_state, 
 						name, 
 						func
 					});

 				_functions.insert( std::make_pair( name, std::move( tmp ) ) );
 			}

 			template <typename... Ret, typename... Args>
 			void registerFunction( const std::string &name, std::tuple< Ret... > ( *func )( Args... ) ) 
 			{
 				constexpr int returnCount = sizeof...( Ret );
 				auto tmp = std::unique_ptr< AbstractFunction > (
 					new Function< returnCount, std::tuple< Ret... >, Args... >{
 						_state, 
 						name, 
 						func
 					});

 				_functions.insert( std::make_pair( name, std::move( tmp ) ) );
 			}

 			void unregisterFunction( const std::string &name )  
 			{
 				auto it = _functions.find( name );
 				if ( it != _functions.end() ) {
 					_functions.erase( it );
 				}
 			}

		private:
			std::map< std::string, std::unique_ptr< AbstractFunction > > _functions;

		};

		template<>
		inline bool ScriptContext::getDefaultValue( void ) const
		{
			return false;
		}

		template<>
		inline std::string ScriptContext::getDefaultValue( void ) const 
		{ 
			return "null"; 
		}

		template<>
		inline Vector3f ScriptContext::Iterable::eval( const std::string &name )
		{
			Vector3f v;
			foreach( name, [&]( ScriptContext &, ScriptContext::Iterable &it ) {
				v[ it.getIndex() ] = it.eval< float >();
			});

			return v;
		}

		template<>
		inline Vector4f ScriptContext::Iterable::eval( const std::string &name )
		{
			Vector4f v;
			foreach( name, [&]( ScriptContext &, ScriptContext::Iterable &it ) {
				v[ it.getIndex() ] = it.eval< float >();
			});
			
			return v;
		}

		template<>
		inline Quaternion4f ScriptContext::Iterable::eval( const std::string &name )
		{
			Vector4f values;
			foreach( name, [&]( ScriptContext &, ScriptContext::Iterable &it ) {
				values[ it.getIndex() ] = it.eval< float >();
			});

			return Quaternion4f( values[ 0 ], values[ 1 ], values[ 2 ], values[ 3 ] );
		}

		template<>
		inline TransformationImpl ScriptContext::Iterable::eval( const std::string &name )
		{
			TransformationImpl result;

			if ( test( name + ".translate" ) ) result.setTranslate( eval< Vector3f >( name + ".translate" ) );
			
			if ( test( name + ".rotate" ) ) {
				Vector4f axisAngle = eval< Vector4f >( name + ".rotate" );
				result.rotate().fromAxisAngle( Vector3f( axisAngle[ 0 ], axisAngle[ 1 ], axisAngle[ 2 ] ), Numericf::DEG_TO_RAD * axisAngle[ 3 ] );
			}

			if ( test( name + ".rotate_q" ) ) result.setRotate( eval< Quaternion4f >( name + ".rotate_q" ) );

			if ( test( name + ".lookAt" ) ) result.lookAt( eval< Vector3f >( name + ".lookAt" ), Vector3f( 0.0f, 1.0f, 0.0f ) );
            
            if ( test( name + ".scale" ) ) result.setScale( eval< float >( name + ".scale" ) );

			return result;
		}
	}

}

#endif

