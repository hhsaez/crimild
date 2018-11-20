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

#include "ShaderGraph.hpp"
#include "Foundation/Log.hpp"

using namespace crimild;
using namespace crimild::shadergraph;
using namespace crimild::containers;

ShaderGraph *ShaderGraph::_currentShaderGraph = nullptr;

ShaderGraph::ShaderGraph( void )
{
	makeCurrent();
}

ShaderGraph::~ShaderGraph( void )
{
	if ( _currentShaderGraph == this ) {
		_currentShaderGraph = nullptr;
	}
}

void ShaderGraph::makeCurrent( void )
{
	_currentShaderGraph = this;
}

void ShaderGraph::eachNode( std::function< void( ShaderGraphNode * ) > const &callback )
{
	_nodes.each( [ callback ]( SharedPointer< ShaderGraphNode > const &node ) {
		callback( crimild::get_ptr( node ) );
	});
}

void ShaderGraph::read( ShaderGraphNode *node, containers::Array< ShaderGraphNode * > const &inputs )
{
	inputs.each( [ this, node ]( ShaderGraphNode *in ) {
		if ( in != nullptr ) {
			_graph.addEdge( in, node );
		}
	});
}

void ShaderGraph::write( ShaderGraphNode *node, containers::Array< ShaderGraphNode * > const &outputs )
{
	outputs.each( [ this, node ]( ShaderGraphNode *out ) {
		if ( out != nullptr ) {
			_graph.addEdge( node, out );
		}
	});
}

std::string ShaderGraph::build( void )
{
	_connected.clear();
	
	_nodes.each( [ this ]( SharedPointer< ShaderGraphNode > const &node ) {
		node->setup( this );
	});

	auto reversed = _graph.reverse();
	_outputs.each( [ this, &reversed ]( ShaderGraphNode *output ) {
		_connected.insert( output );
		reversed.connected( output ).each( [ this ]( ShaderGraphNode *other ) {
			_connected.insert( other );
		});
	});

	containers::Array< ShaderGraphNode * > sorted;
	_graph.sort().each( [ this, &sorted ]( ShaderGraphNode *node ) {
		if ( _connected.contains( node ) ) {
			sorted.add( node );
		}
		else {
			CRIMILD_LOG_DEBUG( "Discarding ", node->getClassName() );
		}
	});

	return generateShaderSource( sorted );
}

crimild::Bool ShaderGraph::isConnected( ShaderGraphNode *node ) const
{
	return _connected.contains( node );
}

