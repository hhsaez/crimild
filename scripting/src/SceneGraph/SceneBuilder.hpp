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

#ifndef CRIMILD_SCRIPTING_SCENE_BUILDER_
#define CRIMILD_SCRIPTING_SCENE_BUILDER_

#include "Foundation/Scripted.hpp"

namespace crimild {

	namespace scripting {

		class SceneBuilder : public crimild::scripting::Scripted {
		private:
			typedef std::function< crimild::Pointer< crimild::NodeComponent >( crimild::scripting::ScriptContext::Iterable & ) > BuilderFunction;

		public:
			SceneBuilder( std::string rootNodeName = "scene" );

			virtual ~SceneBuilder( void );

			crimild::Pointer< crimild::Node > fromFile( const std::string &filename );

		public:
			template< typename T >
			void registerComponent( void )
			{
				registerComponentBuilder< T >( []( crimild::scripting::ScriptContext::Iterable &it ) {
					crimild::Pointer< T > cmp( new T( it ) );
					return cmp;
				});
			}

			template< typename T >
			void registerComponentBuilder( BuilderFunction builder )
			{
				_componentBuilders[ T::_COMPONENT_NAME() ] = builder;
			}

		private:
			void buildNode( ScriptContext::Iterable &i, Group *parent );
			void setupCamera( ScriptContext::Iterable &i, Camera *camera );
			void setTransformation( ScriptContext::Iterable &it, Node *node );
			
			void buildNodeComponents( ScriptContext::Iterable &it, Node *node );
			Pointer< NodeComponent > buildRigidBodyComponent( ScriptContext::Iterable &it );
			Pointer< NodeComponent > buildColliderComponent( ScriptContext::Iterable &it );

		private:
			std::string _rootNodeName;
			std::map< std::string, BuilderFunction > _componentBuilders;
		};

	}

}

#endif

