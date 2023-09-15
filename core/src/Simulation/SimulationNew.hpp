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

#ifndef CRIMILD_SIMULATION_2_
#define CRIMILD_SIMULATION_2_

#include "Foundation/SmallObject.hpp"

#include <map>
#include <memory>
#include <set>

namespace crimild::ex {

    class Node;
    class Geometry3D;

    class Simulation
        : public SmallObject<>,
          public std::enable_shared_from_this< Simulation > {
    public:
        Simulation( void ) = default;
        virtual ~Simulation( void ) = default;

        void setScene( std::shared_ptr< Node > const &scene ) noexcept;
        inline std::shared_ptr< Node > &getScene( void ) noexcept { return m_scene; }

        void attach( std::shared_ptr< Geometry3D > const &geometry ) noexcept;
        void detach( std::shared_ptr< Geometry3D > const &geometry ) noexcept;

        void addNodeToGroup( std::weak_ptr< Node > const &node, std::string_view groupName ) noexcept;
        void removeNodeFromGroup( std::weak_ptr< Node > const &node, std::string_view groupName ) noexcept;

        inline bool hasGroup( std::string_view groupName ) const noexcept
        {
            return m_groups.contains( std::string( groupName ) );
        }

        const auto &getNodesInGroup( std::string_view groupName ) const noexcept
        {
            return m_groups.at( std::string( groupName ) );
        }

        /*
          void pruneExpired( void ) noexcept
          {
          std::erase_if(
          m_geometries,
          []( const auto &ptr ) {
          return ptr.expired();
          }
          );
          }
        */
    private:
        std::shared_ptr< Node > m_scene;

        std::map< std::string, std::set< std::weak_ptr< Node >, std::owner_less<> > > m_groups;

        std::set< std::weak_ptr< Geometry3D >, std::owner_less<> > m_geometries;
    };

}

#endif
