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
#include "Mathematics/Cylinder.hpp"
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

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Box &B, Real &tMin, Real &tMax ) noexcept
    {
        const auto BMax = center( B ) + size( B );
        const auto BMin = center( B ) - size( B );

        tMin = numbers::NEGATIVE_INFINITY;
        tMax = numbers::POSITIVE_INFINITY;

        for ( auto a = 0l; a < 3; a++ ) {
            const auto invD = Real( 1 ) / direction( R )[ a ];
            auto t0 = ( BMin[ a ] - origin( R )[ a ] ) * invD;
            auto t1 = ( BMax[ a ] - origin( R )[ a ] ) * invD;
            if ( invD < 0.0f ) {
                const auto temp = t0;
                t0 = t1;
                t1 = temp;
            }

            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if ( tMax <= tMin ) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Box &B, const Transformation &world, Real &t0, Real &t1 ) noexcept
    {
        // For better performance, use the inverse matrix
        return intersect( inverse( world )( R ), B, t0, t1 );
    }

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Cylinder &C, Real &tMin, Real &tMax ) noexcept
    {
        const auto D = direction( R );

        const Real rdx = D.x;
        const Real rdx2 = rdx * rdx;
        const Real rdz = D.z;
        const Real rdz2 = rdz * rdz;

        const Real a = rdx2 + rdz2;
        if ( isZero( a ) ) {
            return false;
        }

        const Real rox = origin( R ).x;
        const Real rox2 = rox * rox;
        const Real roz = origin( R ).z;
        const Real roz2 = roz * roz;

        const Real r = radius( C );
        const Real r2 = r * r;

        const Real b = Real( 2 ) * ( rox * rdx + roz * rdz );
        const Real c = rox2 + roz2 - r;

        const Real disc = b * b - Real( 4 ) * a * c;
        if ( disc < 0 ) {
            return false;
        }

        const Real sqrt_disc = sqrt( disc );
        auto t0 = ( -b - sqrt_disc ) / ( Real( 2 ) * a );
        auto t1 = ( -b + sqrt_disc ) / ( Real( 2 ) * a );

        if ( t0 > t1 ) {
            std::swap( t0, t1 );
        }

        tMin = t0;
        tMax = t1;

        return true;

        auto hasResult = false;

        const auto y0 = origin( R ).y + t0 * D.y;
        if ( y0 <= height( C ) && y0 >= -height( C ) ) {
            tMin = t0;
            tMax = t0;
            hasResult = true;
        }

        const auto y1 = origin( R ).y + t1 * D.y;
        if ( y1 <= height( C ) && y1 >= -height( C ) ) {
            if ( !hasResult ) {
                tMin = t1;
            }
            tMax = t1;
            hasResult = true;
        }

        return hasResult;
    }

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Cylinder &C, const Transformation &world, Real &t0, Real &t1 ) noexcept
    {
        // For better performance, use the inverse matrix
        return intersect( inverse( world )( R ), C, t0, t1 );
    }

}

#endif
