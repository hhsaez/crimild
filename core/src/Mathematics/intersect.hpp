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

#ifndef CRIMILD_MATHEMATICS_INTERSECT_
#define CRIMILD_MATHEMATICS_INTERSECT_

#include "Mathematics/Box.hpp"
#include "Mathematics/Plane3.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Ray3.hpp"
#include "Mathematics/Sphere.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_inverse.hpp"
#include "Mathematics/dot.hpp"
#include "Mathematics/max.hpp"
#include "Mathematics/min.hpp"
#include "Mathematics/pow.hpp"
#include "Mathematics/sqrt.hpp"

#include <limits>

namespace crimild {

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Sphere &S, Real &t0, Real &t1 ) noexcept
    {
        const auto CO = origin( R ) - center( S );
        const auto a = dot( direction( R ), direction( R ) );
        const auto b = Real( 2 ) * dot( direction( R ), CO );
        const auto c = dot( CO, CO ) - pow( radius( S ), 2 );

        const auto d = b * b - Real( 4 ) * a * c;

        if ( d < 0 ) {
            return false;
        }

        const auto sqrtD = sqrt( d );

        t0 = ( -b - sqrtD ) / ( 2 * a );
        t1 = ( -b + sqrtD ) / ( 2 * a );

        return true;
    }

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Sphere &S, const Transformation &sphereWorld, Real &t0, Real &t1 ) noexcept
    {
        // For better performance, use the inverse matrix
        return intersect( inverse( sphereWorld )( R ), S, t0, t1 );
    }

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Plane3 &P, Real &t ) noexcept
    {
        const auto nv = dot( normal( P ), direction( R ) );
        if ( isZero( nv ) ) {
            return false;
        }

        t = ( dot( -normal( P ), vector3( origin( R ) ) ) + distance( P ) ) / nv;
        return true;
    }

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Plane3 &P, const Transformation &planeWorld, Real &t ) noexcept
    {
        // For better performance, use the inverse matrix
        return intersect( inverse( planeWorld )( R ), P, t );
    }

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Box &B, Real &t0, Real &t1 ) noexcept
    {
        Real tMin = 0;
        Real tMax = 0;
        Bool ret = false;

        auto checkAxis = []( Real o, Real d, Real &tMin, Real &tMax ) -> Bool {
            auto tMinNum = ( -1 - o );
            auto tMaxNum = ( 1 - o );

            if ( abs( d ) >= numbers::EPSILON ) {
                tMin = tMinNum / d;
                tMax = tMaxNum / d;
            } else {
                tMin = tMinNum * numbers::POSITIVE_INFINITY;
                tMax = tMaxNum * numbers::POSITIVE_INFINITY;
            }

            if ( tMin > tMax ) {
                auto t = tMin;
                tMin = tMax;
                tMax = t;
            }

            return true;
        };

        if ( checkAxis( origin( R ).x, direction( R ).x, tMin, tMax ) ) {
            t0 = tMin;
            t1 = tMax;
            ret = true;
        }

        if ( checkAxis( origin( R ).y, direction( R ).y, tMin, tMax ) ) {
            t0 = ret ? max( t0, tMin ) : tMin;
            t1 = ret ? min( t1, tMax ) : tMax;
            ret = true;
        }

        if ( checkAxis( origin( R ).z, direction( R ).z, tMin, tMax ) ) {
            t0 = ret ? max( t0, tMin ) : tMin;
            t1 = ret ? min( t1, tMax ) : tMax;
            ret = true;
        }

        if ( !ret || t0 > t1 ) {
            return false;
        }

        return true;
    }

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Box &B, const Transformation &world, Real &t0, Real &t1 ) noexcept
    {
        // For better performance, use the inverse matrix
        return intersect( inverse( world )( R ), B, t0, t1 );
    }

}

#endif
