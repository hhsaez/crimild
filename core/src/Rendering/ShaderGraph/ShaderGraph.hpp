/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#include "ShaderGraphNode.hpp"

#include "Foundation/Containers/Digraph.hpp"
#include "Foundation/Containers/Stack.hpp"
#include "Foundation/Containers/Set.hpp"

namespace crimild {

	namespace shadergraph {

		class ShaderGraph : public coding::Codable {
			CRIMILD_IMPLEMENT_RTTI( crimild::shadergraph::ShaderGraph )
			
		public:
			ShaderGraph( void );
			virtual ~ShaderGraph( void );

			template< typename NodeType, typename... Args >
			NodeType *addInputNode( Args &&... args )
			{
				auto n = addNode< NodeType >( std::forward< Args >( args )... );
				_inputs.add( n );
				return n;
			}

			template< typename NodeType, typename... Args >
			NodeType *addNode( Args &&... args )
			{
				auto n = crimild::alloc< NodeType >( this, std::forward< Args >( args )... );
				_graph.addVertex( crimild::get_ptr( n ) );
				_nodes.add( n );
				return crimild::get_ptr( n );
			}

			template< typename NodeType, typename... Args >
			NodeType *addOutputNode( Args &&... args )
			{
				auto n = addNode< NodeType >( std::forward< Args >( args )... );
				_outputs.add( n );
				return n;
			}

			void eachNode( std::function< void( ShaderGraphNode * ) > const &callback );

			void read( ShaderGraphNode *node, containers::Array< ShaderGraphNode * > const &inputs );
			void write( ShaderGraphNode *node, containers::Array< ShaderGraphNode * > const &outputs );

		private:
			containers::Digraph< ShaderGraphNode * > _graph;
			containers::Array< SharedPointer< ShaderGraphNode >> _nodes;
			containers::Array< ShaderGraphNode * > _inputs;
			containers::Array< ShaderGraphNode * > _outputs;

		public:
			std::string build( void );

			crimild::Bool isConnected( ShaderGraphNode *node ) const;

		private:
			containers::Set< ShaderGraphNode * > _connected;

		protected:
			virtual std::string generateShaderSource( containers::Array< ShaderGraphNode * > const &sortedNodes ) { return ""; }
		};

	}

}

#endif

