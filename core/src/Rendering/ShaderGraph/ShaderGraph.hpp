/*
 * Copyright (c) 2013-2018, Hernan Saez
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

#ifndef CRIMILD_RENDERING_SHADER_GRAPH_
#define CRIMILD_RENDERING_SHADER_GRAPH_

#include "Foundation/NamedObject.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Coding/Codable.hpp"

namespace crimild {

	namespace shadergraph {

		class Outlet;
		class Node;

		class ShaderGraph : public coding::Codable {
			CRIMILD_IMPLEMENT_RTTI( crimild::shadergraph::ShaderGraph )

		private:
			using OutletArray = containers::Array< Outlet * >;
			using OutletConnectionMap = containers::Map< Outlet *, OutletArray >;
			using NodeConnectionMap = containers::Map< Node *, OutletConnectionMap >;
			using NodeArray = containers::Array< SharedPointer< Node >>;
			
		public:
			ShaderGraph( void );
			virtual ~ShaderGraph( void );

		public:
			template< typename NODE_TYPE, typename... Args >
			NODE_TYPE *createNode( Args &&... args )
			{
				auto node = crimild::alloc< NODE_TYPE >( std::forward< Args >( args )... );
				_nodes.add( node );
				return crimild::get_ptr( node );
			}

			void eachNode( std::function< void( Node * ) > const &callback );

		private:
			NodeArray _nodes;

		public:
			void connect( Outlet *src, Outlet *dst );

			crimild::Size inDegree( Node *node );

			crimild::Size outDegree( Node *node );

			crimild::Bool isConnected( Outlet *outlet );

			Outlet *anyConnection( Outlet *outlet );
			void eachConnection( Outlet *outlet, std::function< void( Outlet * ) > const &callback );

		private:
			NodeConnectionMap _connections;
		};

	}

}

#endif

