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

#include "Node.hpp"

using namespace Crimild;

Node::Node( std::string name )
	: NamedObject( name ),
	  _parent( nullptr ),
	  _worldIsCurrent( false )
{
}

Node::~Node( void )
{
	detachAllComponents();
}

Node *Node::getRootParent( void )
{
	if ( !hasParent() ) {
		return nullptr;
	}

	Node *root = getParent();
	while ( root->getParent() ) {
		root = root->getParent();
	}
	
	return root;
}

void Node::perform( NodeVisitor &visitor )
{
	visitor.traverse( this );
}

void Node::perform( const NodeVisitor &visitor )
{
	const_cast< NodeVisitor & >( visitor ).traverse( this );
}

void Node::accept( NodeVisitor &visitor )
{
	visitor.visitNode( this );
}

void Node::attachComponent( NodeComponentPtr component )
{
	if ( component->getNode() == this ) {
		// the component is already attached to this node
		return;
	}

	detachComponentWithName( component->getName() );
	component->setNode( this );
	_components[ component->getName() ] = component;
	component->onAttach();
}

void Node::detachComponent( NodeComponentPtr component )
{
	if ( component->getNode() != this ) {
		// the component is not attached to this node
		return;
	}

	detachComponentWithName( component->getName() );
}

void Node::detachComponentWithName( std::string name )
{
	if ( _components.find( name ) != _components.end() ) {
		_components[ name ]->onDetach();
		_components[ name ]->setNode( nullptr );
		_components.erase( name );
	}
}

NodeComponent *Node::getComponentWithName( std::string name )
{
	return _components[ name ].get();
}

void Node::detachAllComponents( void )
{
	for ( auto cmp : _components ) {
		if ( cmp.second != nullptr ) {
			cmp.second->onDetach();
			cmp.second->setNode( nullptr );
		}
	}

	_components.clear();
}

void Node::updateComponents( const Time &t )
{
	for ( auto cmp : _components ) {
		if ( cmp.second != nullptr ) {
			cmp.second->update( t );
		}
	}
}

