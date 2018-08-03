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

#include "ShaderBuilder.hpp"
#include "ShaderGraph.hpp"
#include "Node.hpp"
#include "Outlet.hpp"

#include "Foundation/Containers/List.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Exceptions/RuntimeException.hpp"
#include "Rendering/ShaderProgram.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

ShaderBuilder::ShaderBuilder( void )
{
	
}

ShaderBuilder::~ShaderBuilder( void )
{

}

SharedPointer< ShaderProgram > ShaderBuilder::build( SharedPointer< ShaderGraph > const &vsGraph, SharedPointer< ShaderGraph > const &fsGraph )
{
	auto program = crimild::alloc< ShaderProgram >();

	generateVertexShader( crimild::get_ptr( vsGraph ), crimild::get_ptr( program ) );
	generateFragmentShader( crimild::get_ptr( fsGraph ), crimild::get_ptr( program ) );

	return program;
}

ShaderBuilder::NodeArray ShaderBuilder::prepareNodes( ShaderGraph *graph, ShaderProgram *program )
{
	containers::List< Node * > frontier;
	containers::Map< Node *, crimild::Int32 > inCount;
	NodeArray sorted;

	graph->eachNode( [ graph, &frontier, &inCount ]( Node *node ) {
		auto inputs = graph->inDegree( node );
		if ( inputs > 0 ) {
			inCount[ node ] = inputs;
		}
		else {
			frontier.add( node );
		}
	});

	while ( !frontier.empty() ) {
		auto node = frontier.first();
		frontier.remove( node );
		node->prepare( graph, program );
		sorted.add( node );

		node->eachOutputOutlet( [ graph, &sorted, &frontier, &inCount ]( Outlet *outlet ) {
			graph->eachConnection( outlet, [ outlet, &sorted, &frontier, &inCount ]( Outlet *other ) {
				auto otherNode = other->getNode();
				if ( sorted.contains( otherNode ) || frontier.contains( otherNode ) ) {
					// loops not allowed
					std::string msg = "Shader Graph contains a loop: ";
                    msg += outlet->getNode()->getClassName();
                    msg += " attempted to push " + std::string( otherNode->getClassName() );
					throw RuntimeException( msg );
				}
				else {
					inCount[ otherNode ] -= 1;
					if ( inCount[ otherNode ] == 0 ) {
						frontier.add( otherNode );
					}
				}
			});
		});
	}

	return sorted;
}

void ShaderBuilder::generateVertexShader( ShaderGraph *graph, ShaderProgram *program )
{
	auto nodes = prepareNodes( graph, program );

	auto src = generateShaderSource( nodes, graph, program );

	program->setVertexShader( crimild::alloc< Shader >( src ) );
}

void ShaderBuilder::generateFragmentShader( ShaderGraph *graph, ShaderProgram *program )
{
	auto nodes = prepareNodes( graph, program );

	auto src = generateShaderSource( nodes, graph, program );

	program->setFragmentShader( crimild::alloc< Shader >( src ) );
}

