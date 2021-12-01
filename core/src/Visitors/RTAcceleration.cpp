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
            .parentIndex = m_parentIndex,
            .world = boundingTransform,
        } );

    auto tempParent = m_parentIndex;
    m_parentIndex = groupIndex;

    if ( auto left = group->getNodeAt( 0 ) ) {
        left->accept( *this );
    }

    if ( group->getNodeCount() == 2 ) {
        auto right = group->getNodeAt( 1 );
        m_result.nodes[ groupIndex ].secondChildIndex = m_result.nodes.size();
        right->accept( *this );
    }

    m_parentIndex = tempParent;
}

void RTAcceleration::visitGeometry( Geometry *geometry ) noexcept
{
    auto nodeType = RTAcceleratedNode::Type::INVALID;
    geometry->forEachPrimitive(
        [ & ]( auto primitive ) {
            switch ( primitive->getType() ) {
                case Primitive::Type::SPHERE: {
                    nodeType = RTAcceleratedNode::Type::PRIMITIVE_SPHERE;
                    break;
                }
                case Primitive::Type::BOX: {
                    nodeType = RTAcceleratedNode::Type::PRIMITIVE_BOX;
                    break;
                }
                case Primitive::Type::CYLINDER: {
                    nodeType = RTAcceleratedNode::Type::PRIMITIVE_CYLINDER;
                    break;
                }
                default: {
                    break;
                }
            }
        } );

    if ( nodeType == RTAcceleratedNode::Type::INVALID ) {
        return;
    }

    const Int32 materialIndex = [ & ] {
        const auto material = [ & ]() -> materials::PrincipledBSDF * {
            if ( auto materials = geometry->getComponent< MaterialComponent >() ) {
                if ( auto material = materials->first() ) {
                    return static_cast< materials::PrincipledBSDF * >( material );
                }
            }
            return nullptr;
        }();

        if ( material == nullptr ) {
            return -1;
        }

        if ( !m_materialIDs.contains( material ) ) {
            const UInt32 materialID = m_result.materials.size();
            m_result.materials.add( material->getProps() );
            m_materialIDs.insert( material, materialID );
        }

        return m_materialIDs[ material ];
    }();

    if ( materialIndex < 0 ) {
        return;
    }

    m_result.nodes.add(
        RTAcceleratedNode {
            .type = nodeType,
            .parentIndex = m_parentIndex,
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
            .parentIndex = m_parentIndex,
            .world = boundingTransform,
        } );

    auto tempParent = m_parentIndex;
    m_parentIndex = csgIndex;

    if ( auto left = csg->getLeft() ) {
        left->accept( *this );
    }

    if ( auto right = csg->getRight() ) {
        m_result.nodes[ csgIndex ].secondChildIndex = m_result.nodes.size();
        right->accept( *this );
    }

    m_parentIndex = tempParent;
}
