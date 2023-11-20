/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Simulation/SimulationNew.hpp"

#include "SceneGraph/NodeBase.hpp"

using namespace crimild;
using namespace crimild::ex;

void Simulation::setScene( std::shared_ptr< Node > const &scene ) noexcept
{
    if ( m_scene != nullptr ) {
        std::weak_ptr< Simulation > temp;
        m_scene->setSimulation( temp );
    }

    m_scene = scene;

    if ( m_scene != nullptr ) {
        m_scene->setSimulation( weak_from_this() );
    }
}

void Simulation::attach( std::shared_ptr< Geometry3D > const &geometry ) noexcept
{
    m_geometries.insert( geometry );
}

void Simulation::detach( std::shared_ptr< Geometry3D > const &geometry ) noexcept
{
    m_geometries.erase( geometry );
}

void Simulation::addNodeToGroup( std::weak_ptr< Node > const &node, std::string_view groupName ) noexcept
{
    m_groups[ std::string( groupName ) ].insert( node );
}

void Simulation::removeNodeFromGroup( std::weak_ptr< Node > const &node, std::string_view groupName ) noexcept
{
    m_groups[ std::string( groupName ) ].erase( node );
}
