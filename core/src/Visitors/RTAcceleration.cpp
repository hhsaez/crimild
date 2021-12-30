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
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Transformation_operators.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Materials/PrincipledVolumeMaterial.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

void RTAcceleration::traverse( Node *node ) noexcept
{
    node->perform( UpdateWorldState() );
    NodeVisitor::traverse( get_ptr( node ) );
}

void RTAcceleration::visitGroup( Group *group ) noexcept
{
    assert( group->getNodeCount() <= 2 && "Invalid group node" );

    // Compute a transformation for representing bounding volumes
    const auto size = group->getWorldBound()->getMax() - group->getWorldBound()->getMin();
    auto boundingTransform = translation( vector3( group->getWorldBound()->getCenter() ) ) * scale( size.x, size.y, size.z );

    const auto groupIndex = m_result.nodes.size();

    m_result.nodes.add(
        RTAcceleratedNode {
            .type = RTAcceleratedNode::Type::GROUP,
            .world = boundingTransform,
        } );

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
            if ( material->getClassName() == materials::PrincipledBSDF::__CLASS_NAME ) {
                const auto pbr = static_cast< const materials::PrincipledBSDF * >( material );
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
                    } );
            } else if ( material->getClassName() == materials::PrincipledVolume::__CLASS_NAME ) {
                const auto volume = static_cast< const materials::PrincipledVolume * >( material );
                const auto &props = volume->getProps();
                m_result.materials.add(
                    RTAcceleratedMaterial {
                        .albedo = props.albedo,
                        .density = props.density,
                    } );
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
            // Extend from data
            auto vbo = primitive->getVertexData()[ 0 ];
            auto triOffset = m_result.primitives.triangles.size();
            m_result.primitives.triangles.resize( m_result.primitives.triangles.size() + vbo->getVertexCount() );
            memcpy( m_result.primitives.triangles.getData() + triOffset, vbo->getBufferView()->getData(), sizeof( VertexP3N3TC2 ) * vbo->getVertexCount() );

            auto ibo = primitive->getIndices();
            auto indexOffset = m_result.primitives.indices.size();
            m_result.primitives.indices.resize( m_result.primitives.indices.size() + ibo->getIndexCount() );
            for ( auto i = Size( 0 ); i < ibo->getIndexCount(); ++i ) {
                m_result.primitives.indices[ indexOffset + i ] = triOffset + ibo->getIndex( i );
            }

            const auto triCount = ibo->getIndexCount() / 3;
            const auto indexOffsets = m_result.primitives.indexOffsets.size();
            for ( auto i = Size( 0 ); i < triCount; i++ ) {
                m_result.primitives.indexOffsets.add( indexOffset + i * 3 );
            }

            const Int32 primitiveID = m_result.primitives.primTree.size();
            m_result.primitives.primTree.add(
                RTPrimAccelNode::createLeafNode(
                    triCount,
                    indexOffsets ) );
            m_primitiveIDs.insert( primitive, primitiveID );
        }

        return m_primitiveIDs[ primitive ];
    }();

    m_result.nodes.add(
        RTAcceleratedNode {
            .type = RTAcceleratedNode::Type::GEOMETRY,
            .world = [ & ] {
                // Compute a transformation for representing bounding volumes
                const auto size = geometry->getWorldBound()->getMax() - geometry->getWorldBound()->getMin();
                return translation( vector3( geometry->getWorldBound()->getCenter() ) ) * scale( size.x, size.y, size.z );
            }(),
        } );

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
    auto boundingTransform = translation( vector3( csg->getWorldBound()->getCenter() ) ) * scale( csg->getWorldBound()->getRadius() );

    const auto csgIndex = m_result.nodes.size();

    m_result.nodes.add(
        RTAcceleratedNode {
            .type = nodeType,
            .world = boundingTransform,
        } );

    if ( auto left = csg->getLeft() ) {
        left->accept( *this );
    }

    if ( auto right = csg->getRight() ) {
        m_result.nodes[ csgIndex ].secondChildIndex = m_result.nodes.size();
        right->accept( *this );
    }
}
