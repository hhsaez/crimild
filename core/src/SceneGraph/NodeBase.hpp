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

#ifndef CRIMILD_CORE_SCENEGRAPH_NODE_
#define CRIMILD_CORE_SCENEGRAPH_NODE_

#include "Foundation/Named.hpp"
#include "Foundation/SmallObject.hpp"
#include "Simulation/Event.hpp"

#include <map>
#include <set>

/**
   A new way to define scenes, where everything is a Node.

   No more components, behaviors, frame graph. Just Nodes.

   Benefits:
   - No more need to call UpdateWorldState or StartCompponents explicitely, which
   could be error prone.
   - Simpler class design.

   Open Questions:
   - How to handle picking?
   - How to get Geometries to render? Keep an extra structure for visibility set? An octree?
   - Give more responsibilities to Simulation: keep visibility sets, behaviors to execute, etc. Then make nodes register/unregister from the different systems

   Some nodes:
   - Behavior
   - Node3D, Geometry3D
   - EventHandler (with an event name)
   - Signal (or EventDispatcher)
*/

namespace crimild::ex {

    class Simulation;

    class Node
        : public SmallObject<>,
          public std::enable_shared_from_this< Node >,
          public Named {
    public:
        explicit Node( std::string_view name = "" ) noexcept;
        virtual ~Node( void ) = default;

        virtual Event handle( const Event &e ) noexcept;

        inline bool hasParent( void ) const noexcept
        {
            return !m_parent.expired();
        }

        inline std::shared_ptr< Node > getParent( void ) const noexcept
        {
            return m_parent.lock();
        }

        inline std::shared_ptr< Node > getParent( void ) noexcept
        {
            return m_parent.lock();
        }

        void attach( std::shared_ptr< Node > const &child ) noexcept;

        template< typename NodeType = Node >
        std::shared_ptr< NodeType > getChildAt( size_t index ) noexcept
        {
            return std::static_pointer_cast< NodeType >( m_children[ index ] );
        }

        virtual void setSimulation( std::weak_ptr< Simulation > const &simulation ) noexcept;
        inline bool hasSimulation( void ) const noexcept { return !m_simulation.expired(); }
        inline std::shared_ptr< Simulation > getSimulation( void ) const noexcept { return m_simulation.lock(); }

        void addToGroup( std::string_view groupName ) noexcept;
        void removeFromGroup( std::string_view groupName ) noexcept;
        inline const std::set< std::string > &getGroups( void ) const noexcept { return m_groups; }

    private:
        std::vector< std::shared_ptr< Node > > m_children;
        std::weak_ptr< Node > m_parent;
        std::weak_ptr< Simulation > m_simulation;
        std::set< std::string > m_groups;
    };

}

#endif
