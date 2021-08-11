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

#include "Visitors/IntersectWorld.hpp"

#include "Mathematics/Box_normal.hpp"
#include "Mathematics/Ray_apply.hpp"
#include "Mathematics/Sphere_normal.hpp"
#include "Mathematics/intersect.hpp"
#include "Primitives/Primitive.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"

using namespace crimild;

void IntersectWorld::traverse( Node *node ) noexcept
{
    NodeVisitor::traverse( node );

    m_results.sort( []( auto &a, auto &b ) { return a.t < b.t; } );
}

void IntersectWorld::visitGroup( Group *group ) noexcept
{
    if ( !group->getWorldBound()->testIntersection( m_ray ) ) {
        return;
    }

    NodeVisitor::visitGroup( group );
}

void IntersectWorld::visitGeometry( Geometry *geometry ) noexcept
{
    if ( geometry->getCullMode() != Node::CullMode::NEVER ) {
        // Use world bounds for intersection test, wihch is cheaper
        if ( !geometry->getWorldBound()->testIntersection( m_ray ) ) {
            return;
        }
    }

    geometry->forEachPrimitive(
        [ & ]( auto primitive ) {
            intersect( geometry, primitive );
        } );
}

void IntersectWorld::intersect( Geometry *geometry, Primitive *primitive ) noexcept
{
    switch ( primitive->getType() ) {
        case Primitive::Type::SPHERE: {
            const auto S = Sphere {};
            Real t0, t1;
            if ( crimild::intersect( m_ray, S, geometry->getWorld(), t0, t1 ) ) {
                auto pushResult = [ & ]( auto t ) {
                    const auto P = m_ray( t );
                    auto result = Result {
                        .geometry = geometry,
                        .t = t,
                        .point = P,
                    };
                    result.setFaceNormal( m_ray, normal( S, geometry->getWorld(), P ) );
                    m_results.add( result );
                };

                if ( t0 >= numbers::EPSILON ) {
                    pushResult( t0 );
                }

                if ( !isEqual( t0, t1 ) && t1 >= numbers::EPSILON ) {
                    pushResult( t1 );
                }
            }
            break;
        }

        case Primitive::Type::PLANE: {
            const auto P = Plane3 {};
            Real t;
            if ( crimild::intersect( m_ray, P, geometry->getWorld(), t ) && t >= numbers::EPSILON ) {
                const auto p = m_ray( t );
                auto result = Result {
                    .geometry = geometry,
                    .t = t,
                    .point = p,
                };
                result.setFaceNormal( m_ray, geometry->getWorld()( normal( P ) ) );
                m_results.add( result );
            }
            break;
        }

        case Primitive::Type::BOX: {
            const auto B = Box {};
            Real t0, t1;
            if ( crimild::intersect( m_ray, B, geometry->getWorld(), t0, t1 ) ) {
                auto pushResult = [ & ]( auto t ) {
                    const auto P = m_ray( t );
                    auto result = Result {
                        .geometry = geometry,
                        .t = t,
                        .point = P,
                    };
                    result.setFaceNormal( m_ray, normal( B, geometry->getWorld(), P ) );
                    m_results.add( result );
                };

                if ( t0 >= numbers::EPSILON ) {
                    pushResult( t0 );
                }

                if ( !isEqual( t0, t1 ) && t1 >= numbers::EPSILON ) {
                    pushResult( t1 );
                }
            }
            break;
        }

        default:
            break;
    }
}
