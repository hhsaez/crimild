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

#include "ShaderGraphOperation.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

ShaderGraphOperation::ShaderGraphOperation( void )
{
	
}

ShaderGraphOperation::~ShaderGraphOperation( void )
{

}

/*

Node::Node( void )
{
	
}

Node::~Node( void )
{

}

Outlet *Node::addInputOutlet( std::string name, Outlet::Type type )
{
	auto outlet = crimild::alloc< Outlet >( name, type );
	setInputOutlet( outlet );
	return crimild::get_ptr( outlet );
}

void Node::setInputOutlet( SharedPointer< Outlet > const &outlet )
{
	_inputs[ outlet->getName() ] = outlet;
	outlet->setNode( this );
}

Outlet *Node::getInputOutlet( std::string name )
{
	if ( !_inputs.contains( name ) ) {
		return nullptr;
	}

	return crimild::get_ptr( _inputs[ name ] );
}

void Node::eachInputOutlet( OutletArrayCallback const &callback )
{
	_inputs.eachValue( [ callback ]( SharedPointer< Outlet > const &outlet ) {
		callback( crimild::get_ptr( outlet ) );
	});
}

Outlet *Node::addOutputOutlet( std::string name, Outlet::Type type )
{
	auto outlet = crimild::alloc< Outlet >( name, type );
	setOutputOutlet( outlet );
	return crimild::get_ptr( outlet );
}

void Node::setOutputOutlet( SharedPointer< Outlet > const &outlet )
{
	_outputs[ outlet->getName() ] = outlet;
	outlet->setNode( this );
}

Outlet *Node::getOutputOutlet( std::string name )
{
	if ( !_outputs.contains( name ) ) {
		return nullptr;
	}

	return crimild::get_ptr( _outputs[ name ] );
}

void Node::eachOutputOutlet( OutletArrayCallback const &callback )
{
	_outputs.eachValue( [ callback ]( SharedPointer< Outlet > const &outlet ) {
		callback( crimild::get_ptr( outlet ) );
	});
}

void Node::prepare( ShaderGraph *graph, ShaderProgram *program )
{
	// no-op
}

*/

