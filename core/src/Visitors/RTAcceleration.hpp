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
#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/max.hpp"
#include "Rendering/Vertex.hpp"
#include "Visitors/NodeVisitor.hpp"

namespace crimild {

    class Material;
    class Primitive;

    /**
       \brief A representation for a node in the scene hierarchy

       \todo Support more than one primitive per geometry
       \todo Support more than one material per geometry

       \todo Optimize for better cache usage while traversing. Consider
       moving materialIndex to a different structure.
     */
    struct RTAcceleratedNode {
        /**
         * \todo Pack this into a union with other fields (\see RTPrimAccelNode)
         */
        enum struct Type : UInt32 {
            INVALID,

            GROUP,

            /**
             * \brief A geometry is a container of primitives.
             *
             * In this case, bounds will be used as a first intersection test.
             *
             * The primitive associated with this geometry will be stored in the node
             * right after the current one. Therefore, there's no need to store a
             * separated primitive index.
             */
            GEOMETRY,

            CSG_UNION,
            CSG_INTERSECTION,
            CSG_DIFFERENCE,

            PRIMITIVE_SPHERE,
            PRIMITIVE_BOX,
            PRIMITIVE_CYLINDER,
            /**
             * \brief A triangulated primitive
             *
             * In this case, RTAcceleratedNode::primitiveIndex will point to an optimized
             * triangulated mesh, represented by the RTPrimAccelNode class.
             */
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
         * \brief Index to a  array
         *
         * \todo Find a way to pack this field with others in a union.
         * \remarks Only valid for primitives
         */
        Int32 materialIndex;

        // union {
        //     /**
        //      * \remarks Only valid for interior nodes (groups, geometries and csg).
        //      */
        //     Transformation bounds;

        /**
         * \brief World transformation
         *
         * \remarks Only valid for leaf nodes (primitives)
         */
        Transformation world;
        // };
    };

    struct RTAcceleratedMaterial {
        alignas( 16 ) ColorRGB albedo = ColorRGB::Constants::WHITE;
        alignas( 4 ) Real32 metallic = 0;
        alignas( 4 ) Real32 roughness = 0;
        alignas( 4 ) Real32 ambientOcclusion = 1;
        alignas( 4 ) Real32 transmission = 0;
        alignas( 4 ) Real32 indexOfRefraction = 0;
        alignas( 16 ) ColorRGB emissive = ColorRGB::Constants::BLACK;

        /**
         * \brief Volume density
         *
         * A non-negative value is used to indicate that this is a volumetric
         * material
         */
        alignas( 4 ) Real32 density = Real( -1 );
    };

    /**
     * \brief Primitive acceleration representation
     *
     * Interior nodes provide access to three pieces of information:
     * - Split axis: which of the x, y or z axes was split at this node.
     * - Split position: the position of the splitting plane along the axis.
     * - Children: information about to how to reach the two child nodes beneth it.
     *
     * Leaf nodes need to record only which primitives overlap it.
     *
     * Additional care was taken to ensure that nodes use only 8 bytes of memory
     * (assuming a 32bits Float and Int representation). This maximizes the number
     * of nodes that can live in the same cache line at any given point in time
     * (up to 8 in a 64-byte cache line), improving performance when traversing
     * later on.
     *
     * \todo (hernan) I don't like the name. Maybe replaced "primitive" by "triangle"
     * since that's what we're optimizing here.
     */
    struct RTPrimAccelNode {
        union {
            /**
             * \remarks Valid only for interior nodes
             */
            Real split;

            /**
             * If the leaf node is overlapped by zero or one triangle, this variable is
             * used to directly get the index of the first point of that triangle, without
             * any need to push offsets dynamically.
             *
             * \todo This seems to be overkill. Maybe I just need to use the offset below.
             *
             * \remarks Valid only for leaf nodes
             */
            Int onePrimitive;

            /**
             * Stores the offset for the first primitive index
             *
             * \todo Since triangles are represented by indexed vertex buffers,
             * this actually is a double indirection offset. This offset
             * is used to get index for the first point in a triangle.
             *
             * \remarks Valid only for leaf nodes
             */
            Int primitiveIndicesOffset;
        };

        union {
            /**
             * The two low-order bits of this field are used to identify whether
             * this is an interior node (with x, y, and z splits, represented
             * by the values 0, 1 and 2 respectively) and leaf nodes (represented
             * by the value 3).
             *
             * \remarks Valid for interior and leaf nodes
             */
            Int flags;

            /**
             * Stores the number of primitives that overlap this node.
             *
             * Since this variable shares the same space as the flags variable above,
             * only 30 bits are actually available for use. Also, keep in mind that
             * the low-order bits are reserved for flags.
             *
             * \remarks Valid only for leaf nodes
             */
            Int primCount;

            /**
             * \remarks Valid only for interior nodes
             */
            Int aboveChild;
        };

        [[nodiscard]] inline Real getSplitPos( void ) const { return split; }
        [[nodiscard]] inline Int getPrimCount( void ) const { return primCount >> 2; }
        [[nodiscard]] inline Int getSplitAxis( void ) const { return flags & 3; }
        [[nodiscard]] inline Bool isLeaf( void ) const { return ( flags & 3 ) == 3; }
        [[nodiscard]] inline Int getAboveChild( void ) const { return aboveChild >> 2; }

        [[nodiscard]] static RTPrimAccelNode createLeafNode( Int triCount, Int indexOffsets ) noexcept
        {
            auto ret = RTPrimAccelNode {};
            ret.flags = 3; // identifies a leaf node
            ret.primCount |= ( triCount << 2 );
            ret.primitiveIndicesOffset = indexOffsets;
            return ret;
        }
    };

    struct RTPrimAccel {
        Array< VertexP3N3TC2 > triangles; //< vertices
        Array< Int > indices;
        Array< Int > indexOffsets;
        Array< RTPrimAccelNode > primTree; //< triangleTree
    };

    /**
     * \todo Add camera parameters (projection, view, fov, etc...)
     */
    class RTAcceleration : public NodeVisitor {
    public:
        struct Result {
            Array< RTAcceleratedNode > nodes;
            Array< RTAcceleratedMaterial > materials;

            RTPrimAccel primitives;
        };

        explicit RTAcceleration( bool preferUnitPrimitives = true ) noexcept;
        virtual ~RTAcceleration( void ) = default;

        virtual void traverse( Node *node ) noexcept override;

        virtual void visitGroup( Group *group ) noexcept override;
        virtual void visitGeometry( Geometry *geometry ) noexcept override;
        virtual void visitCSGNode( CSGNode *csg ) noexcept override;

        inline const Result &getResult( void ) noexcept { return m_result; }

    private:
        void printStats( void ) noexcept;

    private:
        bool m_preferUnitPrimitives = true;
        Result m_result;
        Map< Material *, Int32 > m_materialIDs;
        Map< Primitive *, Int32 > m_primitiveIDs;

        struct Stats {
            Int primitiveCount = 0;
            Int nodeCount = 0;
            Int maxNodeCount = 0;
            Int leafCount = 0;
            Int maxLeafCount = 0;
            Int maxLeafTriCount = 0;
            Int triCount = 0;
            Int maxTriCount = 0;
            Int maxDepth = 0;
            Vector3i splits = Vector3i { 0, 0, 0 };

            void reset( void ) noexcept
            {
                // TODO
            }

            void onBeforePrimitive( void ) noexcept
            {
                nodeCount = 0;
                leafCount = 0;
                triCount = 0;
            }

            void onAfterPrimitive( void ) noexcept
            {
                primitiveCount++;

                maxNodeCount = crimild::max( maxNodeCount, nodeCount );
                maxLeafCount = crimild::max( maxLeafCount, leafCount );
                maxTriCount = crimild::max( maxTriCount, triCount );
            }

            void onSplit( Int depth ) noexcept
            {
                maxDepth = crimild::max( maxDepth, depth );
            }

            void onLeaf( Int triCount ) noexcept
            {
                leafCount++;
                this->triCount += triCount;
                maxLeafTriCount = crimild::max( maxLeafTriCount, triCount );
            }

            void onNode( Int axis ) noexcept
            {
                nodeCount++;
                splits[ axis ]++;
            }
        };

        Stats m_stats;
    };

}

#endif
