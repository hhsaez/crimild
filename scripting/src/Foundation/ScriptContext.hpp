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

#ifndef CRIMILD_SCRIPTING_LOG_VERBOSE
#define CRIMILD_SCRIPTING_LOG_VERBOSE 0
#endif

namespace crimild {

	namespace scripting {
        
        class ScriptContext;
        
        class ScriptEvaluator {
        public:
            ScriptEvaluator( ScriptContext *ctx, std::string prefix = "" );
            ~ScriptEvaluator( void );
            
            const std::string &getPrefix( void ) const { return _prefix; }
            void setPrefix( std::string prefix ) { _prefix = prefix; }
            
            ScriptContext *getContext( void ) { return _context; }

			ScriptEvaluator getChildEvaluator( std::string childName )
			{
				return ScriptEvaluator( getContext(), getPrefix() + "." + childName );
			}
            
        private:
            ScriptContext *_context = nullptr;
            std::string _prefix;
            
        public:
            bool getPropValue( const std::string &expr, std::string &result, const char *defaultValue )
            {
                return getPropValue< std::string >( expr, result, std::string( defaultValue ) );
            }
            
            template< typename T >
            bool getPropValue( const std::string &expr, T &result, const T &defaultValue )
            {
                if ( !getPropValue< T >( expr, result ) ) {
                    result = defaultValue;
                }
                
                return true;
            }
            
            template< typename T >
            bool getPropValue( T &result )
            {
                return getPropValue< T >( "", result );
            }
            
            template< typename T >
            bool getPropValue( const std::string &expr, T &result );

            template< typename T >
            T getPropValue( const std::string &expr )
            {
                T result;
                getPropValue< T >( expr, result );
                return result;
            }
            
            bool foreach( const std::string &name, std::function< void( ScriptEvaluator &, int ) > callback );
            
        private:
            inline std::string expandExpression( std::string expr )
            {
                if ( _prefix == "" ) {
                    return expr;
                }
                
                if ( expr == "" ) {
                    return _prefix;
                }
                
                return _prefix + "." + expr;
            }
        };

		class ScriptContext {
		public:
			ScriptContext( void );
			ScriptContext( bool openDefaultLibs );
			virtual ~ScriptContext( void );
            
            lua_State *getLuaState( void ) { return _state; }

			void reset( void );

		private:
			lua_State *_state = nullptr;
			bool _openDefaultLibs;

		public:
			bool load( std::string fileName, bool supportCoroutines = false );

			bool parse( std::string text );
            
        public:
            ScriptEvaluator &getEvaluator( void ) { return _evaluator; }
            
        private:
            ScriptEvaluator _evaluator;

		public:
			int yield( void );
			int resume( void );

		private:
			lua_State *_backgroundThreadState = nullptr;

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
			bool foreach( const std::string &name, std::function< void( ScriptEvaluator &, int ) > callback );

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
        
        template< typename T >
        inline bool ScriptEvaluator::getPropValue( const std::string &expr, T &result )
        {
            auto state = getContext()->getLuaState();
            
            std::stringstream str;
            str << "evalExpr = " << expandExpression( expr );
            if ( !luaL_dostring( state, str.str().c_str() ) ) {
                lua_getglobal( state, "evalExpr" );
                bool hasValue = !lua_isnil( state, -1 );
                if ( hasValue ) {
                    result = getContext()->read< T >( -1 );
                }
                lua_pop( state, 1 );
                return hasValue;
            }
            else {
#if CRIMILD_SCRIPTING_LOG_VERBOSE
                Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot parse ", expandExpression( expr ), "\n\tReason: ", lua_tostring( state, -1 ) );
#endif
            }
            
            return false;
        }
        
        template<>
        inline bool ScriptEvaluator::getPropValue( const std::string &name, Vector3f &result )
        {
            return foreach( name, [&result]( ScriptEvaluator &eval, int index ) {
                float v;
                eval.getPropValue< float >( v );
                result[ index ] = v;
            });
        }
        
        template<>
        inline bool ScriptEvaluator::getPropValue( const std::string &name, Vector4f &result )
        {
            return foreach( name, [&result]( ScriptEvaluator &eval, int index ) {
                float v;
                eval.getPropValue< float >( v );
                result[ index ] = v;
            });
        }
        
        template<>
        inline bool ScriptEvaluator::getPropValue( const std::string &name, Quaternion4f &result )
        {
            Vector4f values;
            bool hasValue = foreach( name, [&values]( ScriptEvaluator &eval, int index ) {
                float v;
                eval.getPropValue< float >( v );
                values[ index ] = v;
            });
            
            if ( hasValue ) result = Quaternion4f( values[ 0 ], values[ 1 ], values[ 2 ], values[ 3 ] );
            return hasValue;
        }
        
        template<>
        inline bool ScriptEvaluator::getPropValue( const std::string &name, Transformation &result )
        {
            getPropValue( name + ".translate", result.translate() );
            getPropValue( name + ".scale", result.scale() );
            
            Vector4f axisAngle;
            if ( getPropValue( name + ".rotate", axisAngle ) ) {
                result.rotate().fromAxisAngle( Vector3f( axisAngle[ 0 ], axisAngle[ 1 ], axisAngle[ 2 ] ), Numericf::DEG_TO_RAD * axisAngle[ 3 ] );
                return true;
            }
            
            if ( getPropValue( name + ".rotate_q", result.rotate() ) ) {
                return true;
            }

            Vector3f lookAt;
            if ( getPropValue( name + ".lookAt", lookAt ) ) {
                result.lookAt( lookAt, Vector3f( 0.0f, 1.0f, 0.0f ) );
                return true;    // redundant?
            }
            
            return true;
        }

    }

}

#endif

