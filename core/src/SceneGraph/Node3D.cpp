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

#include "SceneGraph/Node3D.hpp"

#include "Mathematics/Transformation_apply.hpp"

using namespace crimild;
using namespace crimild::ex;

Node3D::Node3D( std::string_view name ) noexcept
    : Node( name )
{
}

Event Node3D::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::NODE_3D_PARENT_TRANSFORMATION_CHANGED:
            m_worldNeedsUpdate = true;
            break;
        case Event::Type::NODE_3D_LOCAL_TRANSFORMATION_CHANGED:
            m_worldNeedsUpdate = true;
            return Node::handle( { Event::Type::NODE_3D_PARENT_TRANSFORMATION_CHANGED } );
        case Event::Type::NODE_3D_WORLD_TRANSFORMATION_CHANGED:
            m_localNeedsUpdate = true;
            return Node::handle( { Event::Type::NODE_3D_PARENT_TRANSFORMATION_CHANGED } );
        default:
            break;
    }
    return Node::handle( e );
}

std::shared_ptr< Node3D > Node3D::getParentNode3D( void ) const noexcept
{
    // This is quite expensive. I need to figure out a way to cache
    // this result. Maybe, we can traverse the scene tree upwards
    // when this node is added to another one, looking for the first
    // Node3D avaiable. That will be the "3D parent"

    std::weak_ptr< Node3D > ret;
    auto parent = getParent();
    while ( parent != nullptr ) {
        if ( auto node3D = std::dynamic_pointer_cast< Node3D >( parent ) ) {
            return node3D;
        }
        parent = parent->getParent();
    }
    return nullptr;
}

void Node3D::setLocal( const Transformation &local ) noexcept
{
    m_local = local;
    handle( { Event::Type::NODE_3D_LOCAL_TRANSFORMATION_CHANGED } );
}

Transformation &Node3D::getLocal( void ) noexcept
{
    if ( m_localNeedsUpdate ) {
        m_local = m_world;
        m_localNeedsUpdate = false;
    }
    return m_local;
}

void Node3D::setWorld( const Transformation &world ) noexcept
{
    m_world = world;
    handle( { Event::Type::NODE_3D_WORLD_TRANSFORMATION_CHANGED } );
}

Transformation &Node3D::getWorld( void ) noexcept
{
    if ( m_worldNeedsUpdate ) {
        if ( hasParent() ) {
            m_world = getParentNode3D()->getWorld() * m_local;
        } else {
            m_world = m_local;
        }
        m_worldNeedsUpdate = false;
    }
    return m_world;
}
