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

#include "ShaderGraph.hpp"
#include "Outlet.hpp"
#include "Node.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

ShaderGraph::ShaderGraph( void )
{
	
}

ShaderGraph::~ShaderGraph( void )
{

}

void ShaderGraph::eachNode( std::function< void( Node * ) > const &callback )
{
	_nodes.each( [ callback ]( SharedPointer< Node > const &node ) {
		callback( crimild::get_ptr( node ) );
	});
}

void ShaderGraph::connect( Outlet *src, Outlet *dst )
{
	assert( src != nullptr && dst != nullptr );

	auto srcNode = src->getNode();
	auto srcNodeRef = crimild::retain( srcNode );
	if ( !_nodes.contains( srcNodeRef ) ) {
		_nodes.add( srcNodeRef );
	}
	
	auto dstNode = dst->getNode();
	auto dstNodeRef = crimild::retain( dstNode );
	if ( !_nodes.contains( dstNodeRef ) ) {
		_nodes.add( dstNodeRef );
	}

	auto &srcConnections = _connections[ srcNode ][ src ];	
	if ( !srcConnections.contains( dst ) ) {
		srcConnections.add( dst );
	}

	auto &dstConnections = _connections[ dstNode ][ dst ];
	if ( !dstConnections.contains( src ) ) {
		dstConnections.add( src );
	}
}

crimild::Size ShaderGraph::inDegree( Node *node )
{
	crimild::Size d = 0;
	node->eachInputOutlet( [ this, &d ]( Outlet *outlet ) {
		if ( isConnected( outlet ) ) {
			d++;
		}
	});
	return d;
}

crimild::Size ShaderGraph::outDegree( Node *node )
{
	crimild::Size d = 0;
	node->eachOutputOutlet( [ this, &d ]( Outlet *outlet ) {
		if ( isConnected( outlet ) ) {
			d++;
		}
	});
	return d;
}

crimild::Bool ShaderGraph::isConnected( Outlet *outlet )
{
	if ( !_connections.contains( outlet->getNode() ) ) {
		return false;
	}

    if ( !_connections[ outlet->getNode() ].contains( outlet ) ) {
        return false;
    }

    return !_connections[ outlet->getNode() ][ outlet ].empty();
}

Outlet *ShaderGraph::anyConnection( Outlet *outlet )
{
	if ( !isConnected( outlet ) ) {
		return nullptr;
	}

	return _connections[ outlet->getNode() ][ outlet ].first();
}

void ShaderGraph::eachConnection( Outlet *outlet, std::function< void( Outlet * ) > const &callback )
{
	if ( isConnected( outlet ) ) {
		_connections[ outlet->getNode() ][ outlet ].each( [ callback ]( Outlet *otherOutlet ) {
			callback( otherOutlet );			
		});
	}
}

