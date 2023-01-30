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

#include "Group.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"
#include "Exceptions/HasParentException.hpp"
#include "Foundation/Containers/Array.hpp"

#include <algorithm>
#include <cassert>
#include <thread>

using namespace crimild;

Group::Group( std::string name )
    : Node( name )
{
}

Group::~Group( void )
{
    detachAllNodes();
}

void Group::attachNode( Node *node )
{
    attachNode( crimild::retain( node ) );
}

void Group::attachNode( SharedPointer< Node > const &node )
{
    if ( node->getParent() == this ) {
        // the node is already attach to this group
        return;
    }

    if ( node->getParent() != nullptr ) {
        throw HasParentException( node->getName(), this->getName(), node->getParent()->getName() );
    }

    node->setParent( this );

    _nodes.add( node );
}

void Group::attachNodeAfter( SharedPointer< Node > const &node, SharedPointer< Node > const &before ) noexcept
{
    if ( node->hasParent() ) {
        node->detachFromParent();
    }

    size_t index;
    if ( _nodes.indexOf( before, index ) ) {
        _nodes.addAt( node, index + 1 );
    } else {
        _nodes.add( node );
    }
    node->setParent( this );
}

void Group::attachNodeBefore( SharedPointer< Node > const &node, SharedPointer< Node > const &before ) noexcept
{
    if ( node->hasParent() ) {
        node->detachFromParent();
    }

    size_t index;
    if ( _nodes.indexOf( before, index ) ) {
        _nodes.addAt( node, index );
    } else {
        _nodes.add( node );
    }

    node->setParent( this );
}

void Group::detachNode( Node *node )
{
    if ( node->getParent() == this ) {
        node->setParent( nullptr );
        _nodes.remove( crimild::retain( node ) );
    }
}

void Group::detachNode( SharedPointer< Node > const &node )
{
    detachNode( crimild::get_ptr( node ) );
}

void Group::detachAllNodes( void )
{
    _nodes.each( []( SharedPointer< Node > &node ) { node->setParent( nullptr ); } );
    _nodes.clear();
}

Node *Group::getNodeAt( unsigned int index )
{
    return crimild::get_ptr( _nodes[ index ] );
}

Node *Group::getNode( std::string name )
{
    Node *result = nullptr;
    bool found = false;
    _nodes.each( [ &result, &found, name ]( SharedPointer< Node > &node ) {
        if ( !found && node->getName() == name ) {
            result = crimild::get_ptr( node );
            found = true;
        }
    } );

    return result;
}

void Group::forEachNode( std::function< void( Node * ) > callback, bool skipDisabledNodes )
{
    if ( skipDisabledNodes ) {
        _nodes.each(
            [ &callback ]( SharedPointer< Node > &node ) {
                if ( node != nullptr && node->isEnabled() ) {
                    callback( crimild::get_ptr( node ) );
                }
            }
        );
    } else {
        _nodes.each(
            [ &callback ]( SharedPointer< Node > &node ) {
                if ( node != nullptr ) {
                    callback( crimild::get_ptr( node ) );
                }
            }
        );
    }
}

void Group::accept( NodeVisitor &visitor )
{
    visitor.visitGroup( this );
}

void Group::encode( coding::Encoder &encoder )
{
    Node::encode( encoder );

    encoder.encode( "nodes", _nodes );
}

void Group::decode( coding::Decoder &decoder )
{
    Node::decode( decoder );

    Array< SharedPointer< Node > > nodes;
    decoder.decode( "nodes", nodes );
    nodes.each( [ this ]( SharedPointer< Node > &n ) {
        attachNode( n );
    } );
}
