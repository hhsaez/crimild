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
#include "Group.hpp"

#include "Boundings/AABBBoundingVolume.hpp"

using namespace crimild;

Node::Node( std::string name )
	: NamedObject( name ),
	  _worldIsCurrent( false ),
	  _localBound( crimild::alloc< AABBBoundingVolume >( Vector3f( 0.0f, 0.0f, 0.0f ), 1.0f ) ),
	  _worldBound( crimild::alloc< AABBBoundingVolume >( Vector3f( 0.0f, 0.0f, 0.0f ), 1.0f ) )
{

}

Node::~Node( void )
{
	detachAllComponents();
}

Node *Node::getRootParent( void )
{
    auto root = getParent();
    if ( root != nullptr ) {
        while ( root->hasParent() ) {
            root = root->getParent();
        }
    }
    
    return root;
}

SharedPointer< Node > Node::detachFromParent( void )
{
    // do this before detaching
    auto node = crimild::retain( this );
    
    Group *parent = getParent< Group >();
    if ( parent != nullptr ) {
        parent->detachNode( node );
    }

    return node;
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

void Node::attachComponent( NodeComponent *component )
{
    attachComponent( crimild::retain( component ) );
}

void Node::attachComponent( SharedPointer< NodeComponent > const &component )
{
    if ( hasComponent( component ) ) {
		// the component is already attached to this node
		return;
	}

    // ignore return?
	detachComponentWithName( component->getComponentName() );
    
	component->setNode( this );
	_components[ component->getComponentName() ] = component;
	component->onAttach();
}

void Node::detachComponent( NodeComponent *component )
{
    if ( !hasComponent( component ) ) {
        // the component is not attached to this node
        return;
    }
    
    detachComponentWithName( component->getComponentName() );
}

void Node::detachComponent( SharedPointer< NodeComponent > const &component )
{
    detachComponent( crimild::get_ptr( component ) );
}

SharedPointer< NodeComponent > Node::detachComponentWithName( std::string name )
{
	if ( _components.find( name ) != _components.end() ) {
        auto current = _components[ name ];
        if ( current != nullptr ) {
            current->onDetach();
            current->setNode( nullptr );
        }

        _components[ name ] = nullptr;
		_components.erase( name );
        
        return current;
	}
    
    return nullptr;
}

void Node::detachAllComponents( void )
{
    forEachComponent( []( NodeComponent *cmp ) {
        cmp->onDetach();
        cmp->setNode( nullptr );
    });

	_components.clear();
}

void Node::startComponents( void )
{
	forEachComponent( []( NodeComponent *component ) { component->start(); } );
}

void Node::forEachComponent( std::function< void ( NodeComponent * ) > callback )
{
	// create a copy of the component's collection
	// to prevent errors when attaching or detaching
	// components during an update pass
    // TODO: should we lock this instead?
	auto cs = _components;
	for ( auto cmp : cs ) {
		if ( cmp.second != nullptr ) {
            callback( crimild::get_ptr( cmp.second ) );
		}
	}
}

