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
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Visitors/NodeVisitor.hpp"

namespace crimild {

    class Material;

    /**
       \brief A representation for a node in the scene hierarchy

       \todo Support more than one primitive per geometry
       \todo Support more than one material per geometry
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
        };

        Type type = Type::INVALID;

        /**
           \brief Offset to the first child

           \remarks This is only valid on nodes that can have children

           \todo This might be used to represent the offset to the first triangle in a primitive
           \todo This might be used to represent the offset to the first triangle in a primitive
         */
        Int32 firstChildIndex = -1;

        /**
           \brief Number of children
           \remarks This is only valid on nodes that can have children
           \todo This might be used to represent the number of triangles in a primitive
         */
        Int32 childCount = -1;

        /**
           \brief Index to the parent node
         */
        Int32 parentIndex = -1;

        /**
           \brief Index to an external array

           For Type::GEOMETRY, this represents the index to the first material
         */
        Int32 index = -1;

        /**
           \brief Inverse world transformation
         */
        Matrix4 invWorld;
    };

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
        UInt32 m_level = 0;
        Map< Material *, UInt32 > m_materialIDs;
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
                if ( node.childCount > 0 ) {
                    if ( lastVisited < current ) {
                        // begin traversal
                        if ( !fn( node, current ) ) {
                            // callback failed. don't traverse children
                            lastVisited = current;
                            current = node.parentIndex;
                        } else {
                            // traverse first child
                            lastVisited = current;
                            current = node.firstChildIndex;
                        }
                    } else {
                        auto next = lastVisited + 1;
                        if ( next < node.firstChildIndex + node.childCount ) {
                            // next child
                            lastVisited = current;
                            current = next;
                        } else {
                            // done traversing. return to parent
                            lastVisited = current;
                            current = node.parentIndex;
                        }
                    }
                } else {
                    // Don't care for result. Just go back to parent anyway
                    fn( node, current );
                    // no children. return to parent
                    lastVisited = current;
                    current = node.parentIndex;
                }
            }
        }

    }

}

#endif
