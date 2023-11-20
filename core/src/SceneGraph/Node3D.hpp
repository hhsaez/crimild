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

#ifndef CRIMILD_CORE_SCENEGRAPH_NODE_3D_
#define CRIMILD_CORE_SCENEGRAPH_NODE_3D_

// #include "Mathematics/Point_equality.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/Transformation_constants.hpp"
// #include "Mathematics/Transformation_inverse.hpp"
// #include "Mathematics/Transformation_translation.hpp"
#include "SceneGraph/NodeBase.hpp"

namespace crimild::ex {

    /// \brief A node
    ///
    /// This is a 3d node, with transfromations
    class Node3D : public Node {
    public:
        explicit Node3D( std::string_view name = "" ) noexcept;
        virtual ~Node3D( void ) noexcept = default;

        virtual Event handle( const Event &e ) noexcept override;

        std::shared_ptr< Node3D > getParentNode3D( void ) const noexcept;

        inline bool localNeedsUpdate( void ) const noexcept { return m_localNeedsUpdate; }
        void setLocal( const Transformation &local ) noexcept;
        Transformation &getLocal( void ) noexcept;

        inline bool worldNeedsUpdate( void ) const noexcept { return m_worldNeedsUpdate; }
        void setWorld( const Transformation &world ) noexcept;
        Transformation &getWorld( void ) noexcept;

    private:
        Transformation m_local = Transformation::Constants::IDENTITY;
        Transformation m_world = Transformation::Constants::IDENTITY;
        bool m_localNeedsUpdate = false;
        bool m_worldNeedsUpdate = false;
    };

}

#endif
