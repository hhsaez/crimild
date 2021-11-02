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
#include "Mathematics/Cylinder_normal.hpp"
#include "Mathematics/Ray_apply.hpp"
#include "Mathematics/Sphere_normal.hpp"
#include "Mathematics/Triangle_normal.hpp"
#include "Mathematics/intersect.hpp"
#include "Mathematics/isNaN.hpp"
#include "Primitives/Primitive.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"

#include <unordered_set>

using namespace crimild;

void IntersectWorld::traverse( Node *node ) noexcept
{
    NodeVisitor::traverse( node );

    m_results.sort( []( auto &a, auto &b ) { return a.t < b.t; } );
}

void IntersectWorld::visitGroup( Group *group ) noexcept
{
    if ( group->getLayer() == Node::Layer::SKYBOX ) {
        return;
    }

    const auto R = inverse( group->getWorld() )( m_ray );
    if ( !group->getWorldBound()->testIntersection( m_ray ) ) {
        return;
    }

    NodeVisitor::visitGroup( group );
}

void IntersectWorld::visitGeometry( Geometry *geometry ) noexcept
{
    if ( geometry->getLayer() == Node::Layer::SKYBOX ) {
        return;
    }

    if ( geometry->getCullMode() != Node::CullMode::NEVER ) {
        // Use world bounds for intersection test, wihch is cheaper
        // The ray must be transformed since we're using the local bound.
        // I think there is a bug here. We should be using worldBound
        // instead, but that results in a wrong intersection test
        // TODO(hernan): Seems like a bug. Fix it.
        const auto R = inverse( geometry->getWorld() )( m_ray );
        if ( !geometry->getLocalBound()->testIntersection( R ) ) {
            return;
        }
    }

    geometry->forEachPrimitive(
        [ & ]( auto primitive ) {
            intersect( geometry, primitive );
        } );
}

void IntersectWorld::visitCSGNode( CSGNode *csg ) noexcept
{
    // TODO: test intersection (needs worldstateupdate)
    // TODO: not sure if intersection test should use local or world coordinate system
    // TODO: if the former, remember to transform the ray!!

    auto beforeLeftSize = m_results.size();
    if ( auto left = csg->getLeft() ) {
        left->accept( *this );
    }
    auto afterLeftSize = m_results.size();

    auto beforeRightSize = m_results.size();
    if ( auto right = csg->getRight() ) {
        right->accept( *this );
    }
    auto afterRightSize = m_results.size();

    auto leftIntersections = [ & ] {
        std::vector< Result > res( afterLeftSize - beforeLeftSize );
        for ( auto i = 0l; i < res.size(); ++i ) {
            res[ i ] = m_results[ beforeLeftSize + i ];
        }
        return res;
    }();

    auto rightIntersections = [ & ] {
        std::vector< Result > res( afterRightSize - beforeRightSize );
        for ( auto i = 0l; i < res.size(); ++i ) {
            res[ i ] = m_results[ beforeRightSize + i ];
        }
        return res;
    }();

    auto allIntersections = [ & ] {
        std::vector< Result > res( leftIntersections.size() + rightIntersections.size() );
        auto i = Index( 0 );
        for ( auto &x : leftIntersections ) {
            res[ i++ ] = x;
        }
        for ( auto &x : rightIntersections ) {
            res[ i++ ] = x;
        }
        std::sort(
            std::begin( res ),
            std::end( res ),
            []( auto &a, auto &b ) { return a.t < b.t; } );
        return res;
    }();

    auto intersectionAllowed = []( auto op, auto lHit, auto inL, auto inR ) {
        switch ( op ) {
            case CSGNode::Operator::UNION:
                return ( lHit && !inR ) || ( !lHit && !inL );
            case CSGNode::Operator::INTERSECTION:
                return ( lHit && inR ) || ( !lHit && inL );
            case CSGNode::Operator::DIFFERENCE:
                return ( lHit && !inR ) || ( !lHit && inL );
            default:
                return false;
        }
    };

    auto filteredIntersections = [ & ] {
        std::vector< Result > res;

        auto inR = false;
        auto inL = false;

        for ( auto &i : allIntersections ) {
            auto lHit = std::find_if(
                            std::begin( leftIntersections ),
                            std::end( leftIntersections ),
                            [ & ]( auto &other ) {
                                return i.geometry == other.geometry;
                            } )
                        != std::end( leftIntersections );
            if ( intersectionAllowed( csg->getOperator(), lHit, inL, inR ) ) {
                res.push_back( i );
            }

            if ( lHit ) {
                inL = !inL;
            } else {
                inR = !inR;
            }
        }

        return res;
    }();

    m_results.resize( beforeLeftSize + filteredIntersections.size() );
    auto i = Index( 0 );
    for ( auto &x : filteredIntersections ) {
        m_results[ beforeLeftSize + ( i++ ) ] = x;
    }
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

        case Primitive::Type::OPEN_CYLINDER:
        case Primitive::Type::CYLINDER: {
            const auto C = Cylinder { .closed = primitive->getType() != Primitive::Type::OPEN_CYLINDER };
            Real t0, t1;
            if ( crimild::intersect( m_ray, C, geometry->getWorld(), t0, t1 ) ) {
                auto pushResult = [ & ]( auto t ) {
                    if ( isEqual( t, numbers::POSITIVE_INFINITY ) ) {
                        return;
                    }
                    const auto P = m_ray( t );
                    auto result = Result {
                        .geometry = geometry,
                        .t = t,
                        .point = P,
                    };
                    result.setFaceNormal( m_ray, normal( C, geometry->getWorld(), P ) );
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

        case Primitive::Type::TRIANGLES: {
            auto positions = [ & ] {
                BufferAccessor *positions = nullptr;
                primitive->getVertexData().each(
                    [ & ]( auto vertices ) {
                        if ( positions == nullptr ) {
                            positions = vertices->get( VertexAttribute::Name::POSITION );
                        }
                    } );
                return positions;
            }();

            if ( positions == nullptr ) {
                return;
            }

            if ( auto indices = primitive->getIndices() ) {
                //const auto N = indices->getIndexCount() / 3;
                const auto N = indices->getIndexCount();
                for ( auto i = 0; i < N; i += 3 ) {
                    const auto T = Triangle {
                        positions->get< Point3 >( indices->getIndex( i + 0 ) ),
                        positions->get< Point3 >( indices->getIndex( i + 1 ) ),
                        positions->get< Point3 >( indices->getIndex( i + 2 ) ),
                    };

                    Real t;
                    if ( crimild::intersect( m_ray, T, geometry->getWorld(), t ) ) {
                        if ( !isZero( t ) && !isNaN( t ) && !isEqual( t, numbers::POSITIVE_INFINITY ) ) {
                            const auto P = m_ray( t );
                            auto result = Result {
                                .geometry = geometry,
                                .t = t,
                                .point = P,
                            };
                            result.setFaceNormal( m_ray, normal( T, geometry->getWorld(), P ) );
                            m_results.add( result );
                        }
                    }
                }
            }

            break;
        }

        default:
            break;
    }
}
