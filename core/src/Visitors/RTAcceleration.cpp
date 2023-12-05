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

#include "Visitors/RTAcceleration.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Bounds3.hpp"
#include "Mathematics/Bounds3_bisect.hpp"
#include "Mathematics/Bounds3_combine.hpp"
#include "Mathematics/Bounds3_io.hpp"
#include "Mathematics/Bounds3_overlaps.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Random.hpp"
#include "Mathematics/Transformation_operators.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/io.hpp"
#include "Mathematics/max.hpp"
#include "Mathematics/min.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/BoxPrimitive.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Materials/PrincipledVolumeMaterial.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

// Simple utility tool to check if a triangulated primitive is actually one of the unit ones
// Because of how coding works, we cannot rely on pointer equality.
static bool isSamePrimitive( const Primitive *A, const Primitive *B ) noexcept
{
    if ( A->getVertexData().size() != B->getVertexData().size() ) {
        return false;
    }

    const auto dataA = A->getVertexData().first()->getBufferView();
    const auto dataB = B->getVertexData().first()->getBufferView();

    if ( dataA->getLength() != dataB->getLength() ) {
        return false;
    }

    return memcmp( dataA->getData(), dataB->getData(), dataA->getLength() ) == 0;
}

static void splitPrimitives(
    const std::vector< Int32 > &offsets,
    const Bounds3 &bounds,
    const std::vector< Bounds3 > &primBounds,
    Array< Int > &indexOffsets,
    Array< RTPrimAccelNode > &primTree,
    Int depth
) noexcept
{
    const Int32 N = offsets.size();

    auto addLeaf = [ & ]( bool forced, const auto &offsets ) {
        const Int32 indexOffset = indexOffsets.size();
        for ( auto i = 0; i < N; i++ ) {
            indexOffsets.add( offsets[ i ] );
        }
        primTree.add(
            RTPrimAccelNode::createLeafNode(
                N,
                indexOffset
            )
        );
    };

    if ( N <= 4 || depth == 0 ) {
        addLeaf( depth == 0, offsets );
        return;
    }

    // TODO:
    // Create an array for each axis
    // Accumulate above/below tris
    // Pick the one with the lowest cost
    // The cost could be the difference between sizes for above/below
    // Handle special case where one array is empty (?)

    const Int axis = maxDimension( bounds );
    Real split;
    Bounds3 aboveB, belowB;
    bisect( bounds, axis, split, belowB, aboveB );
    std::vector< Int > above;
    std::vector< Int > below;

    for ( auto i = 0; i < N; ++i ) {
        const auto o = offsets[ i ];
        // Divide by 3 since offsets represent values in the index array
        // and bounds are per triangle/primitive
        const auto &B = primBounds[ o / 3 ];
        if ( overlaps( aboveB, B ) ) {
            above.push_back( o );
        }

        if ( overlaps( belowB, B ) ) {
            below.push_back( o );
        }
    }

    if ( below.size() == offsets.size() && above.size() == offsets.size() ) {
        // Cannot find a good split
        // TODO: Try a different axis?
        addLeaf( true, offsets );
        return;
    }

    const auto nodeIdx = [ & ] {
        auto node = RTPrimAccelNode {};
        node.split = split;
        node.flags = axis;
        const auto nodeIdx = primTree.size();
        primTree.add( node );
        return nodeIdx;
    }();

    if ( below.size() == offsets.size() ) {
        addLeaf( true, below );
    } else {
        splitPrimitives( below, belowB, primBounds, indexOffsets, primTree, depth - 1 );
    }

    primTree[ nodeIdx ].aboveChild |= ( primTree.size() << 2 );

    if ( above.size() == offsets.size() ) {
        addLeaf( true, above );
    } else {
        splitPrimitives( above, aboveB, primBounds, indexOffsets, primTree, depth - 1 );
    }
}

static void optimizePrimitive(
    Primitive *primitive,
    Array< VertexP3N3TC2 > &triangles,
    Array< Int > &indices,
    Array< Int > &indexOffsets,
    Array< RTPrimAccelNode > &primTree
) noexcept
{
    auto vbo = primitive->getVertexData()[ 0 ];
    auto ibo = primitive->getIndices();

    auto triOffset = triangles.size();
    triangles.resize( triangles.size() + vbo->getVertexCount() );
    memcpy( triangles.getData() + triOffset, vbo->getBufferView()->getData(), sizeof( VertexP3N3TC2 ) * vbo->getVertexCount() );

    auto baseIndexOffset = indices.size();
    indices.resize( indices.size() + ibo->getIndexCount() );
    for ( auto i = Size( 0 ); i < ibo->getIndexCount(); ++i ) {
        indices[ baseIndexOffset + i ] = triOffset + ibo->getIndex( i );
    }

    const auto triCount = ibo->getIndexCount() / 3;

    auto rootBounds = Bounds3 {};
    std::vector< Int32 > offsets( triCount );
    std::vector< Bounds3 > primBounds( triCount );
    for ( auto i = Size( 0 ); i < triCount; ++i ) {
        const Int32 idx = i * 3;
        offsets[ i ] = idx;

        auto B = Bounds3 {};
        for ( auto pi = 0; pi < 3; ++pi ) {
            const auto P = triangles[ indices[ baseIndexOffset + idx + pi ] ].position;
            B = combine( B, Point3f( P ) );
        }

        primBounds[ i ] = B;

        rootBounds = combine( rootBounds, B );
    }

    splitPrimitives( offsets, rootBounds, primBounds, indexOffsets, primTree, 10 );
}

RTAcceleration::RTAcceleration( bool preferUnitPrimitives ) noexcept
    : m_preferUnitPrimitives( preferUnitPrimitives )
{
    // no-op
}

void RTAcceleration::traverse( Node *node ) noexcept
{
    m_stats.reset();

    node->perform( UpdateWorldState() );
    NodeVisitor::traverse( get_ptr( node ) );

    printStats();
}

void RTAcceleration::visitGroup( Group *group ) noexcept
{
    assert( group->getNodeCount() <= 2 && "Invalid group node" );

    // Compute a transformation for representing bounding volumes
    const auto size = Real( 0.5 ) * ( group->getWorldBound()->getMax() - group->getWorldBound()->getMin() );
    auto boundingTransform = translation( Vector3f( group->getWorldBound()->getCenter() ) ) * scale( size.x, size.y, size.z );

    const auto groupIndex = m_result.nodes.size();

    m_result.nodes.add(
        RTAcceleratedNode {
            .type = RTAcceleratedNode::Type::GROUP,
            .world = boundingTransform,
        }
    );

    if ( auto left = group->getNodeAt( 0 ) ) {
        left->accept( *this );
    }

    if ( group->getNodeCount() == 2 ) {
        auto right = group->getNodeAt( 1 );
        m_result.nodes[ groupIndex ].secondChildIndex = m_result.nodes.size();
        right->accept( *this );
    }
}

void RTAcceleration::visitGeometry( Geometry *geometry ) noexcept
{
    auto primitive = geometry->anyPrimitive();
    if ( primitive == nullptr ) {
        return;
    }

    auto primitiveType = [ & ] {
        switch ( primitive->getType() ) {
            case Primitive::Type::SPHERE: {
                return RTAcceleratedNode::Type::PRIMITIVE_SPHERE;
            }
            case Primitive::Type::BOX: {
                return RTAcceleratedNode::Type::PRIMITIVE_BOX;
            }
            case Primitive::Type::OPEN_CYLINDER:
            case Primitive::Type::CYLINDER: {
                return RTAcceleratedNode::Type::PRIMITIVE_CYLINDER;
            }
            case Primitive::Type::TRIANGLES: {
                if ( m_preferUnitPrimitives ) {
                    if ( isSamePrimitive( primitive, SpherePrimitive::UNIT_SPHERE.get() ) ) {
                        return RTAcceleratedNode::Type::PRIMITIVE_SPHERE;
                    } else if ( isSamePrimitive( primitive, BoxPrimitive::UNIT_BOX.get() ) ) {
                        return RTAcceleratedNode::Type::PRIMITIVE_BOX;
                    }
                }

                // For the moment, I'm defining as "valid" primitives only those
                // containing both vertices and indices.
                auto isValid = !primitive->getVertexData().empty();
                isValid = isValid && ( primitive->getVertexData()[ 0 ]->getVertexCount() > 0 );
                isValid = isValid && ( primitive->getIndices()->getIndexCount() > 0 );
                // Additionally, only the P3N3TC2 layout is valid, since we're going to
                // do physically-based rendering calculations.
                isValid = isValid && ( primitive->getVertexData()[ 0 ]->getVertexLayout() == VertexP3N3TC2::getLayout() );
                // Make sure we do have the right amount of indices
                isValid = isValid && ( primitive->getIndices()->getIndexCount() % 3 == 0 );
                if ( isValid ) {
                    return RTAcceleratedNode::Type::PRIMITIVE_TRIANGLES;
                } else {
                    return RTAcceleratedNode::Type::INVALID;
                }
            }
            default: {
                return RTAcceleratedNode::Type::INVALID;
            }
        }
    }();

    if ( primitiveType == RTAcceleratedNode::Type::INVALID ) {
        return;
    }

    const Int32 materialIndex = [ & ] {
        const auto material = [ & ]() -> Material * {
            if ( auto materials = geometry->getComponent< MaterialComponent >() ) {
                if ( auto material = materials->first() ) {
                    return material;
                }
            }
            return nullptr;
        }();

        if ( material == nullptr ) {
            return -1;
        }

        if ( !m_materialIDs.contains( material ) ) {
            const UInt32 materialID = m_result.materials.size();
            if ( auto pbr = dynamic_cast< materials::PrincipledBSDF * >( material ) ) {
                const auto &props = pbr->getProps();
                m_result.materials.add(
                    RTAcceleratedMaterial {
                        .albedo = props.albedo,
                        .metallic = props.metallic,
                        .roughness = props.roughness,
                        .ambientOcclusion = props.ambientOcclusion,
                        .transmission = props.transmission,
                        .indexOfRefraction = props.indexOfRefraction,
                        .emissive = props.emissive,
                    }
                );
            } else if ( auto volume = dynamic_cast< materials::PrincipledVolume * >( material ) ) {
                const auto &props = volume->getProps();
                m_result.materials.add(
                    RTAcceleratedMaterial {
                        .albedo = props.albedo,
                        .density = props.density,
                    }
                );
            } else {
                return -1;
            }
            m_materialIDs.insert( material, materialID );
        }

        return m_materialIDs[ material ];
    }();

    if ( materialIndex < 0 ) {
        return;
    }

    const Int32 primitiveIndex = [ & ] {
        if ( primitiveType != RTAcceleratedNode::Type::PRIMITIVE_TRIANGLES ) {
            return -1;
        }

        if ( !m_primitiveIDs.contains( primitive ) ) {
            if ( primitive->getVertexData().empty() ) {
                return -1;
            }

            // Extend from data
            auto vbo = primitive->getVertexData()[ 0 ];
            if ( vbo->getVertexCount() == 0 ) {
                return -1;
            }

            auto ibo = primitive->getIndices();
            if ( ibo->getIndexCount() == 0 ) {
                return -1;
            }

            m_stats.onBeforePrimitive();

            const Int32 primID = m_result.primitives.primTree.size();

            optimizePrimitive(
                primitive,
                m_result.primitives.triangles,
                m_result.primitives.indices,
                m_result.primitives.indexOffsets,
                m_result.primitives.primTree
            );

            m_primitiveIDs.insert( primitive, primID );

            m_stats.onAfterPrimitive();
        }

        return m_primitiveIDs[ primitive ];
    }();

    m_result.nodes.add(
        RTAcceleratedNode {
            .type = RTAcceleratedNode::Type::GEOMETRY,
            .world = [ & ] {
                // Compute a transformation for representing bounding volumes
                const auto size = Real( 0.5 ) * ( geometry->getWorldBound()->getMax() - geometry->getWorldBound()->getMin() );
                return translation( Vector3f( geometry->getWorldBound()->getCenter() ) ) * scale( size.x, size.y, size.z );
            }(),
        }
    );

    m_result.nodes.add( RTAcceleratedNode {
        .type = primitiveType,
        .primitiveIndex = primitiveIndex,
        .materialIndex = materialIndex,
        .world = geometry->getWorld(),
    } );
}

void RTAcceleration::visitCSGNode( CSGNode *csg ) noexcept
{
    auto nodeType = [ & ] {
        switch ( csg->getOperator() ) {
            case CSGNode::Operator::UNION:
                return RTAcceleratedNode::Type::CSG_UNION;
            case CSGNode::Operator::INTERSECTION:
                return RTAcceleratedNode::Type::CSG_INTERSECTION;
            case CSGNode::Operator::DIFFERENCE:
                return RTAcceleratedNode::Type::CSG_DIFFERENCE;
            default:
                return RTAcceleratedNode::Type::INVALID;
        }
    }();

    if ( nodeType == RTAcceleratedNode::Type::INVALID ) {
        return;
    }

    // Compute a transformation for representing bounding volumes
    auto boundingTransform = translation( Vector3f( csg->getWorldBound()->getCenter() ) ) * scale( csg->getWorldBound()->getRadius() );

    const auto csgIndex = m_result.nodes.size();

    m_result.nodes.add(
        RTAcceleratedNode {
            .type = nodeType,
            .world = boundingTransform,
        }
    );

    if ( auto left = csg->getLeft() ) {
        left->accept( *this );
    }

    if ( auto right = csg->getRight() ) {
        m_result.nodes[ csgIndex ].secondChildIndex = m_result.nodes.size();
        right->accept( *this );
    }
}

void RTAcceleration::printStats( void ) noexcept
{
    std::cout << "Stats: "
              << "\n\tPrimitives: " << m_stats.primitiveCount
              << "\n\tAvg Node/Primitive: " << ( Real( m_result.primitives.primTree.size() ) / m_stats.primitiveCount )
              << "\n\tMax Nodes/Primitive: " << m_stats.maxNodeCount
              << "\n\tMax Leaf/Primitive: " << m_stats.maxLeafCount
              << "\n\tAvg Tri/Primitive: " << ( Real( m_result.primitives.triangles.size() ) / m_stats.primitiveCount )
              << "\n\tMax Tri/Primitive: " << m_stats.maxTriCount
              << "\n\tMax Depth: " << m_stats.maxDepth
              << "\n\tMax Tri/Leaf: " << m_stats.maxLeafTriCount
              << "\n\tSplits: " << m_stats.splits
              << std::endl;
}
