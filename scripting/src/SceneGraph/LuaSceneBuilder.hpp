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

#ifndef CRIMILD_SCRIPTING_SCENE_BUILDER_LUA_
#define CRIMILD_SCRIPTING_SCENE_BUILDER_LUA_

#include "Foundation/Scripted.hpp"

namespace crimild {

	namespace scripting {
        
        class LuaNodeBuilderRegistry : public StaticSingleton< LuaNodeBuilderRegistry > {
        public:
            using NodeBuilderFunction = std::function< SharedPointer< Node > ( crimild::scripting::ScriptEvaluator & ) >;
            
        public:
            LuaNodeBuilderRegistry( void );
            virtual ~LuaNodeBuilderRegistry( void );
            
            template< typename T >
            void registerNodeBuilder( std::string type )
            {
                _nodeBuilders[ type ] = []( crimild::scripting::ScriptEvaluator &eval ) {
                    auto node = crimild::alloc< T >();
                    node->load( eval );
                    return node;
                };
            }
            
            void registerCustomNodeBuilder( std::string type, NodeBuilderFunction builder )
            {
                _nodeBuilders[ type ] = builder;
            }
            
            NodeBuilderFunction getBuilder( std::string type ) { return _nodeBuilders[ type ]; }
            
        public:
            template< class T >
            class RegistrationHelper {
            public:
                RegistrationHelper( const char *name )
                {
                    LuaNodeBuilderRegistry::getInstance()->registerNodeBuilder< T >( name );
                }
                
                ~RegistrationHelper( void )
                {
                    
                }
            };
            
        public:
            void flush( void );
            
        private:
            std::map< std::string, NodeBuilderFunction > _nodeBuilders;
        };
        
#define CRIMILD_SCRIPTING_REGISTER_NODE_BUILDER( X ) \
    static crimild::scripting::LuaNodeBuilderRegistry::RegistrationHelper< X > __nodeRegistrationHelper( #X );
        
        class LuaComponentBuilderRegistry : public StaticSingleton< LuaComponentBuilderRegistry > {
        public:
            using ComponentBuilderFunction = std::function< SharedPointer< NodeComponent > ( crimild::scripting::ScriptEvaluator & ) >;
            
        public:
            LuaComponentBuilderRegistry( void );
            virtual ~LuaComponentBuilderRegistry( void );
            
            template< typename T >
            void registerComponentBuilder( std::string type )
            {
                _componentBuilders[ type ] = []( crimild::scripting::ScriptEvaluator &eval ) {
                    return crimild::alloc< T >( eval );
                };
            }
            
            void registerCustomComponentBuilder( std::string type, ComponentBuilderFunction builder )
            {
                _componentBuilders[ type ] = builder;
            }
            
            ComponentBuilderFunction getBuilder( std::string type ) { return _componentBuilders[ type ]; }

        public:
            template< class T >
            class RegistrationHelper {
            public:
                RegistrationHelper( const char *name )
                {
                    LuaComponentBuilderRegistry::getInstance()->registerComponentBuilder< T >( name );
                }
                
                ~RegistrationHelper( void )
                {
                    
                }
            };
            
        public:
            void flush( void );
            
        private:
            std::map< std::string, ComponentBuilderFunction > _componentBuilders;
        };

#define CRIMILD_SCRIPTING_REGISTER_COMPONENT_BUILDER( X ) \
    static crimild::scripting::LuaComponentBuilderRegistry::RegistrationHelper< X > __componentRegistrationHelper( #X );
        
		class LuaSceneBuilder :
            public crimild::scripting::Scripted,
            public crimild::SceneBuilder {
                
		private:
			using NodeBuilderFunction = std::function< SharedPointer< Node > ( crimild::scripting::ScriptEvaluator & ) >;
			using ComponentBuilderFunction = std::function< SharedPointer< NodeComponent > ( crimild::scripting::ScriptEvaluator & ) >;

		public:
			LuaSceneBuilder( std::string rootNodeName = "scene" );

			virtual ~LuaSceneBuilder( void );

			virtual void reset( void ) override;

            virtual SharedPointer< Node > fromFile( const std::string &filename ) override;

		public:
			template< typename T >
			void generateNodeBuilder( std::string type )
			{
				_nodeBuilders[ type ] = []( crimild::scripting::ScriptEvaluator &eval ) {
					auto node = crimild::alloc< T >();
                    node->load( eval );
                    return node;
                };
			}

			template< typename T >
			void registerComponent( void )
			{
				registerComponentBuilder< T >( []( crimild::scripting::ScriptEvaluator &eval ) {
                    return crimild::alloc< T >( eval );
				});
			}

			template< typename T >
			void registerComponentBuilder( ComponentBuilderFunction builder )
			{
				_componentBuilders[ T::_COMPONENT_NAME() ] = builder;
			}

		private:
			SharedPointer< Node > buildNode( ScriptEvaluator &eval, Group *parent );

			void setTransformation( ScriptEvaluator &eval, SharedPointer< Node > const &node );
			
			void buildNodeComponents( ScriptEvaluator &eval, SharedPointer< Node > const &node );

		private:
			std::string _rootNodeName;
            std::map< std::string, NodeBuilderFunction > _nodeBuilders;
			std::map< std::string, ComponentBuilderFunction > _componentBuilders;
		};

	}

}

#endif

