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

#include "Mathematics/Ray_apply.hpp"
#include "Mathematics/Sphere_normal.hpp"
#include "Mathematics/intersect.hpp"
#include "SceneGraph/Geometry.hpp"

using namespace crimild;

void IntersectWorld::traverse( Node *node ) noexcept
{
    NodeVisitor::traverse( node );

    m_results.sort( []( auto &a, auto &b ) { return a.t < b.t; } );
}

void IntersectWorld::visitGeometry( Geometry *geometry ) noexcept
{
    // Use world bounds for intersection test, wihch is cheaper
    if ( !geometry->getWorldBound()->testIntersection( m_ray ) ) {
        return;
    }

    // If bounds test passes, check individual primitives using
    // the inverse world transform to convert the ray
    // from world to local space
    const auto S = Sphere {};
    Real t0, t1;
    if ( intersect( m_ray, S, geometry->getWorld(), t0, t1 ) ) {
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
}
