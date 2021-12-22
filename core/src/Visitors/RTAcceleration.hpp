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

#ifndef CRIMILD_VISITORS_RT_ACCELERATION_
#define CRIMILD_VISITORS_RT_ACCELERATION_

#include "Foundation/Containers/Array.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Transformation.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Visitors/NodeVisitor.hpp"

namespace crimild {

    class Material;

    /**
       \brief A representation for a node in the scene hierarchy

       \todo Support more than one primitive per geometry
       \todo Support more than one material per geometry
       
       \todo Optimize for better cache usage while traversing. Consider
       moving materialIndex to a different structure.
     */
    struct RTAcceleratedNode {
        enum struct Type : UInt32 {
            INVALID,

            GROUP,
            //GEOMETRY, //< TODO

            CSG_UNION,
            CSG_INTERSECTION,
            CSG_DIFFERENCE,

            PRIMITIVE_SPHERE,
            PRIMITIVE_BOX,
            PRIMITIVE_CYLINDER,
            PRIMITIVE_TRIANGLES,
        };

        Type type = Type::INVALID;

        union {

            /**
             * \brief Offset to the second child
             * 
             * The first child in a group is stored immediatelly after the current node. The
             * second child can be store much later, though. 
             * 
             * We don't need to keep track of the number of children since a node has either one
             * or two child nodes. Empty nodes are not allowed.
             * 
             * \remarks This is only valid on nodes that can have children
            */
            Int32 secondChildIndex = -1;

            /**
             * \brief Offset to the primitive
             * \remarks This offset is only valid for geometry nodes using triangulated primitives
             */
            Int32 primitiveIndex;
        };

        /**
           \brief Index to the parent node
         */
        Int32 parentIndex = -1;

        /** 
         * \brief Index to a  array
         * \remarks Only valid for geometries
         */
        Int32 materialIndex;

        /**
         * \brief World transformation
         */
        Transformation world;
    };

    /**
     * \todo Add camera parameters (projection, view, fov, etc...)
     */
    class RTAcceleration : public NodeVisitor {
    public:
        struct Result {
            Array< RTAcceleratedNode > nodes;
            Array< materials::PrincipledBSDF::Props > materials;
        };

        virtual void traverse( Node *node ) noexcept override;

        virtual void visitGroup( Group *group ) noexcept override;
        virtual void visitGeometry( Geometry *geometry ) noexcept override;
        virtual void visitCSGNode( CSGNode *csg ) noexcept override;

        inline const Result &getResult( void ) noexcept { return m_result; }

    private:
        Result m_result;
        Int32 m_parentIndex = -1;
        Map< Material *, Int32 > m_materialIDs;
    };

    namespace utils {

        template< typename Fn >
        void traverseNonRecursive( const RTAcceleration::Result &scene, Fn fn ) noexcept
        {
            if ( scene.nodes.empty() ) {
                return;
            }

            Int32 current = 0;
            Int32 lastVisited = -1;

            while ( current >= 0 ) {
                const auto &node = scene.nodes[ current ];
                switch ( node.type ) {
                    case RTAcceleratedNode::Type::GROUP: {
                        if ( lastVisited < current ) {
                            // begin traversal
                            if ( !fn( node, current ) ) {
                                // callback failed. don't traverse children
                                lastVisited = current;
                                current = node.parentIndex;
                            } else {
                                // traverse first child
                                lastVisited = current;
                                current = current + 1;
                            }
                        } else if ( lastVisited != node.secondChildIndex ) {
                            // next child
                            lastVisited = current;
                            current = node.secondChildIndex;
                        } else {
                            // done traversing
                            lastVisited = current;
                            current = node.parentIndex;
                        }
                        break;
                    }
                    default: {
                        // Don't care for result. Just go back to parent anyway
                        fn( node, current );
                        // no children. return to parent
                        lastVisited = current;
                        current = node.parentIndex;
                        break;
                    }
                }
            }
        }
    }

}

#endif
