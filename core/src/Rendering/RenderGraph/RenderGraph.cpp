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

#include "RenderGraph.hpp"
#include "RenderGraphPass.hpp"
#include "RenderGraphResource.hpp"

#include "Foundation/Containers/List.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Exceptions/RuntimeException.hpp"

using namespace crimild;
using namespace crimild::containers;
using namespace crimild::rendergraph;

RenderGraph::RenderGraph( void )
{

}

RenderGraph::~RenderGraph( void )
{

}


void RenderGraph::eachPass( std::function< void( RenderGraphPass * ) > const &callback )
{
	_passes.each( [ callback ]( SharedPointer< RenderGraphPass > const &pass ) {
		callback( crimild::get_ptr( pass ) );
	});
}

RenderGraphResource *RenderGraph::createResource( void )
{
	auto r = crimild::alloc< RenderGraphResource >();
	_resources.add( r );
	return crimild::get_ptr( r );
}

void RenderGraph::eachResource( std::function< void( RenderGraphResource * ) > const &callback )
{
	_resources.each( [ callback ]( SharedPointer< RenderGraphResource > const &r ) {
		callback( crimild::get_ptr( r ) );
	});
}

void RenderGraph::read( RenderGraphPass *pass, Array< RenderGraphResource * > const &resources )
{
	resources.each( [ this, pass ]( RenderGraphResource *r ) {
		if ( r != nullptr ) {
			_graph.addEdge( r, pass );
		}
	});
}

void RenderGraph::write( RenderGraphPass *pass, Array< RenderGraphResource * > const &resources )
{
	resources.each( [ this, pass ]( RenderGraphResource *r ) {
		if ( r != nullptr ) {
			_graph.addEdge( pass, r );
		}
	});
}

void RenderGraph::compile( void )
{
	// make sure all connections are set
	_passes.each( [ this ]( SharedPointer< RenderGraphPass > const &pass ) {
		pass->setup( this );
	});

	auto sorted = _graph.sort();

	_sortedPasses.clear();
	sorted.each( [ this ]( RenderGraph::Node *node ) {
		if ( node->getType() == RenderGraph::Node::Type::PASS ) {
			_sortedPasses.add( static_cast< RenderGraphPass * >( node ) );
		}
	});
}

void RenderGraph::execute( Renderer *renderer, RenderQueue *renderQueue )
{
	if ( _sortedPasses.size() == 0 ) {
		compile();
	}
	
	_sortedPasses.each( [ renderer, renderQueue ]( RenderGraphPass *pass ) {
		pass->execute( renderer, renderQueue );
	});
}

