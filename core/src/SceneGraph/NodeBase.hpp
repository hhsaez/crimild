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

#ifndef CRIMILD_CORE_SCENEGRAPH_NODE_BASE_
#define CRIMILD_CORE_SCENEGRAPH_NODE_BASE_

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

namespace crimild {

    namespace simulation {

        /*
                class SimulationContext : public std::enable_shared_from_this< SimulationContext > {
                public:
                    void setScene( std::shared_ptr< scenegraph::NodeBase > const &scene ) noexcept
                    {
                        if ( m_scene != nullptr ) {
                            m_scene->setSimulation( nullptr );
                        }

                        m_scene = scene;

                        if ( m_scene != nullptr ) {
                            m_scene->setSimulation( weak_from_this() );
                        }
                    }

                    virtual void step( void ) noexcept
                    {
                        m_scene.step();

                        pruneExpired();
                    }

                    void pruneExpired( void ) noexcept
                    {
                        std::erase_if(
                            m_geometries,
                            []( const auto &ptr ) {
                                return ptr.expired();
                            }
                        );
                    }

                private:
                    std::shared_ptr< scenegraph::NodeBase > m_scene;
                    std::set< std::weak_ptr< scenegraph::Geometry >, std::owner_less<> > m_geometries;
                };
        */

    }

    namespace scenegraph {

        class NodeBase
            : public SmallObject<>,
              public std::enable_shared_from_this< NodeBase >,
              public Named {
        public:
            explicit NodeBase( std::string_view name = "" ) noexcept;
            virtual ~NodeBase( void ) = default;

            virtual Event handle( const Event &e ) noexcept;

            inline bool hasParent( void ) const noexcept
            {
                return !m_parent.expired();
            }

            inline std::shared_ptr< NodeBase > getParent( void ) const noexcept
            {
                return m_parent.lock();
            }

            inline std::shared_ptr< NodeBase > getParent( void ) noexcept
            {
                return m_parent.lock();
            }

            void attach( std::shared_ptr< NodeBase > const &child ) noexcept;

            template< typename NodeBaseType = NodeBase >
            std::shared_ptr< NodeBaseType > getChildAt( size_t index ) noexcept
            {
                return std::static_pointer_cast< NodeBaseType >( m_children[ index ] );
            }

        private:
            std::vector< std::shared_ptr< NodeBase > > m_children;
            std::weak_ptr< NodeBase > m_parent;
        };

    }

}

#endif
