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

#include "SceneGraph/NodeBase.hpp"

#include "Simulation/SimulationNew.hpp"

using namespace crimild::ex;

using Event = crimild::Event;

Node::Node( std::string_view name ) noexcept
    : Named( name )
{
    // no-op
}

Event Node::handle( const Event &e ) noexcept
{
    for ( auto &child : m_children ) {
        child->handle( e );
    }
    return e;
}

void Node::attach( std::shared_ptr< Node > const &child ) noexcept
{
    m_children.push_back( child );
    child->m_parent = weak_from_this();
}

void Node::setSimulation( std::weak_ptr< Simulation > const &simulation ) noexcept
{
    // TODO: Remove things from old simulation?

    m_simulation = simulation;

    if ( hasSimulation() ) {
        for ( const auto &groupName : m_groups ) {
            getSimulation()->addNodeToGroup( weak_from_this(), groupName );
        }
    }

    for ( auto &child : m_children ) {
        child->setSimulation( simulation );
    }
}

void Node::addToGroup( std::string_view groupName ) noexcept
{
    if ( hasSimulation() ) {
        getSimulation()->addNodeToGroup( weak_from_this(), groupName );
    }
    m_groups.insert( std::string( groupName ) );
}

void Node::removeFromGroup( std::string_view groupName ) noexcept
{
    if ( hasSimulation() ) {
        getSimulation()->removeNodeFromGroup( weak_from_this(), groupName );
    }
    m_groups.erase( std::string( groupName ) );
}
