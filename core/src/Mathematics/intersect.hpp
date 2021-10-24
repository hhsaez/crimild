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

    namespace internal {

        [[nodiscard]] static constexpr Bool checkCylinderCap( const Ray3 &R, Real t ) noexcept
        {
            const auto x = R.o.x + t * R.d.x;
            const auto z = R.o.z + t * R.d.z;
            return ( x * x + z * z ) <= 1;
        };

        [[nodiscard]] static constexpr Bool intersectCylinderCaps( const Ray3 &R, const Cylinder &C, Real &tMin, Real &tMax ) noexcept
        {
            if ( !isClosed( C ) || isZero( R.d.y ) ) {
                return false;
            }

            auto hasResult = false;
            auto t = ( -C.height - R.o.y ) / R.d.y;
            if ( checkCylinderCap( R, t ) ) {
                if ( t < tMin ) {
                    tMax = tMin;
                    tMin = t;
                } else if ( t < tMax ) {
                    tMax = t;
                }
                hasResult = true;
            }

            t = ( C.height - R.o.y ) / R.d.y;
            if ( checkCylinderCap( R, t ) ) {
                if ( t < tMin ) {
                    tMax = tMin;
                    tMin = t;
                } else if ( t < tMax ) {
                    tMax = t;
                }
                hasResult = true;
            }

            return hasResult;
        };

    }

    [[nodiscard]] static constexpr Bool intersect( const Ray3 &R, const Cylinder &C, Real &tMin, Real &tMax ) noexcept
    {
        tMin = numbers::POSITIVE_INFINITY;
        tMax = numbers::POSITIVE_INFINITY;

        const Real a = ( R.d.x * R.d.x ) + ( R.d.z * R.d.z );
        if ( isZero( a ) ) {
            return internal::intersectCylinderCaps( R, C, tMin, tMax );
        }

        const Real b = ( Real( 2 ) * R.o.x * R.d.x ) + ( Real( 2 ) * R.o.z * R.d.z );
        const Real c = ( R.o.x * R.o.x ) + ( R.o.z * R.o.z ) - Real( 1 );

        const Real disc = b * b - Real( 4 ) * a * c;
        if ( disc < 0 ) {
            return false;
        }
        const Real disc_root = sqrt( disc );

        Real t0 = ( -b - disc_root ) / ( Real( 2 ) * a );
        Real t1 = ( -b + disc_root ) / ( Real( 2 ) * a );
        if ( t0 < t1 ) {
            std::swap( t0, t1 );
        }

        auto hasResult = false;

        const auto y0 = R.o.y + t0 * R.d.y;
        if ( y0 < height( C ) && y0 > -height( C ) ) {
            tMin = t0;
            tMax = t0;
            hasResult = true;
        }

        const auto y1 = R.o.y + t1 * R.d.y;
        if ( y1 < height( C ) && y1 > -height( C ) ) {
            if ( !hasResult ) {
                tMin = t1;
            }
            tMax = t1;
            hasResult = true;
        }

        if ( internal::intersectCylinderCaps( R, C, tMin, tMax ) ) {
            hasResult = true;
        }

        if ( tMin > tMax ) {
            std::swap( tMin, tMax );
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
