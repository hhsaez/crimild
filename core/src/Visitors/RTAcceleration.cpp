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
#include "Primitives/Primitive.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

void RTAcceleration::traverse( Node *node ) noexcept
{
    m_result.nodes.resize( 1 );
    m_level = 0;

    node->perform( UpdateWorldState() );

    NodeVisitor::traverse( node );
}

void RTAcceleration::visitGroup( Group *group ) noexcept
{
    m_result.nodes[ m_level ].type = RTAcceleratedNode::Type::GROUP;
    m_result.nodes[ m_level ].invWorld = group->getWorld().invMat;

    const auto childCount = group->getNodeCount();
    m_result.nodes[ m_level ].childCount = childCount;

    if ( childCount > 0 ) {
        const auto firstChildIndex = m_result.nodes.size();

        m_result.nodes[ m_level ].firstChildIndex = firstChildIndex;

        m_result.nodes.resize( m_result.nodes.size() + childCount );

        Int32 parentIndex = m_level;

        for ( auto i = 0; i < childCount; ++i ) {
            m_level = firstChildIndex + i;

            m_result.nodes[ firstChildIndex + i ] = {
                .type = RTAcceleratedNode::Type::INVALID,
                .parentIndex = parentIndex,
            };

            if ( auto node = group->getNodeAt( i ) ) {
                node->accept( *this );
            }
        }

        m_level = parentIndex;
    }
}

void RTAcceleration::visitGeometry( Geometry *geometry ) noexcept
{
    const auto material = [ & ]() -> materials::PrincipledBSDF * {
        if ( auto materials = geometry->getComponent< MaterialComponent >() ) {
            if ( auto material = materials->first() ) {
                return static_cast< materials::PrincipledBSDF * >( material );
            }
        }
        return nullptr;
    }();

    if ( material == nullptr ) {
        return;
    }

    if ( !m_materialIDs.contains( material ) ) {
        const UInt32 materialID = m_result.materials.size();
        m_result.materials.add( material->getProps() );
        m_materialIDs.insert( material, materialID );
    }

    auto nodeType = RTAcceleratedNode::Type::INVALID;
    Int32 index = -1;
    geometry->forEachPrimitive(
        [ & ]( auto primitive ) {
            switch ( primitive->getType() ) {
                case Primitive::Type::SPHERE: {
                    nodeType = RTAcceleratedNode::Type::PRIMITIVE_SPHERE;
                    index = m_materialIDs[ material ];
                    break;
                }
                case Primitive::Type::BOX: {
                    nodeType = RTAcceleratedNode::Type::PRIMITIVE_BOX;
                    index = m_materialIDs[ material ];
                    break;
                }
                case Primitive::Type::CYLINDER: {
                    nodeType = RTAcceleratedNode::Type::PRIMITIVE_CYLINDER;
                    index = m_materialIDs[ material ];
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

    m_result.nodes[ m_level ].type = nodeType;
    m_result.nodes[ m_level ].index = index;
    m_result.nodes[ m_level ].invWorld = geometry->getWorld().invMat;
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

    m_result.nodes[ m_level ].type = nodeType;
    m_result.nodes[ m_level ].invWorld = csg->getWorld().invMat;

    NodeVisitor::visitCSGNode( csg );
}
