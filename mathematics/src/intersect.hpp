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

#include "Box.hpp"
#include "Cylinder.hpp"
#include "Plane3.hpp"
#include "Point3.hpp"
#include "Ray3.hpp"
#include "Sphere.hpp"
#include "Transformation.hpp"
#include "Transformation_apply.hpp"
#include "Transformation_inverse.hpp"
#include "Triangle.hpp"
#include "Triangle_edges.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "max.hpp"
#include "min.hpp"
#include "pow.hpp"
#include "sqrt.hpp"

#include <algorithm>
#include <limits>

namespace crimild {

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Sphere &S, real_t &t0, real_t &t1 ) noexcept
    {
        const auto CO = origin( R ) - center( S );
        const auto a = dot( direction( R ), direction( R ) );
        const auto b = real_t( 2 ) * dot( direction( R ), CO );
        const auto c = dot( CO, CO ) - pow( radius( S ), 2 );

        const auto d = b * b - real_t( 4 ) * a * c;

        if ( d < 0 ) {
            return false;
        }

        const auto sqrtD = sqrt( d );

        t0 = ( -b - sqrtD ) / ( 2 * a );
        t1 = ( -b + sqrtD ) / ( 2 * a );

        return true;
    }

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Sphere &S, const Transformation &sphereWorld, real_t &t0, real_t &t1 ) noexcept
    {
        // For better performance, use the inverse matrix
        return intersect( inverse( sphereWorld )( R ), S, t0, t1 );
    }

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Sphere &S, const Matrix4 &invWorld, real_t &t0, real_t &t1 ) noexcept
    {
        return intersect( invWorld * R, S, t0, t1 );
    }

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Plane3 &P, real_t &t ) noexcept
    {
        const auto nv = dot( normal( P ), direction( R ) );
        if ( isZero( nv ) ) {
            return false;
        }

        t = ( dot( -normal( P ), Vector3( origin( R ) ) ) + distance( P ) ) / nv;
        return true;
    }

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Plane3 &P, const Transformation &planeWorld, real_t &t ) noexcept
    {
        // For better performance, use the inverse matrix
        return intersect( inverse( planeWorld )( R ), P, t );
    }

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Plane3 &P, const Matrix4 &invWorld, real_t &t ) noexcept
    {
        return intersect( invWorld * R, P, t );
    }

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Box &B, real_t &tMin, real_t &tMax ) noexcept
    {
        const auto BMax = center( B ) + size( B );
        const auto BMin = center( B ) - size( B );

        tMin = numbers::NEGATIVE_INFINITY;
        tMax = numbers::POSITIVE_INFINITY;

        for ( auto a = 0l; a < 3; a++ ) {
            if ( isZero( direction( R )[ a ] ) ) {
                // Ray is parallel to plane in this direction
                if ( ( origin( R )[ a ] < BMin[ a ] ) || ( origin( R )[ a ] > BMax[ a ] ) ) {
                    // outside of limits. No intersection possible
                    return false;
                }
            }

            const auto invD = real_t( 1 ) / direction( R )[ a ];
            auto t0 = ( BMin[ a ] - origin( R )[ a ] ) * invD;
            auto t1 = ( BMax[ a ] - origin( R )[ a ] ) * invD;
            if ( t0 > t1 ) {
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

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Box &B, const Transformation &world, real_t &t0, real_t &t1 ) noexcept
    {
        // For better performance, use the inverse matrix
        return intersect( inverse( world )( R ), B, t0, t1 );
    }

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Box &B, const Matrix4 &invWorld, real_t &t0, real_t &t1 ) noexcept
    {
        return intersect( invWorld * R, B, t0, t1 );
    }

    namespace internal {

        [[nodiscard]] static constexpr bool checkCylinderCap( const Ray3 &R, real_t t ) noexcept
        {
            const auto x = R.origin.x + t * R.direction.x;
            const auto z = R.origin.z + t * R.direction.z;
            return ( x * x + z * z ) <= 1;
        };

        [[nodiscard]] static constexpr bool intersectCylinderCaps( const Ray3 &R, const Cylinder &C, real_t &tMin, real_t &tMax ) noexcept
        {
            if ( !isClosed( C ) || isZero( R.direction.y ) ) {
                return false;
            }

            auto hasResult = false;
            auto t = ( -C.height - R.origin.y ) / R.direction.y;
            if ( checkCylinderCap( R, t ) ) {
                if ( t < tMin ) {
                    tMax = tMin;
                    tMin = t;
                } else if ( t < tMax ) {
                    tMax = t;
                }
                hasResult = true;
            }

            t = ( C.height - R.origin.y ) / R.direction.y;
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

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Cylinder &C, real_t &tMin, real_t &tMax ) noexcept
    {
        tMin = numbers::POSITIVE_INFINITY;
        tMax = numbers::POSITIVE_INFINITY;

        const auto Rdir = direction( R );
        const real_t a = ( Rdir.x * Rdir.x ) + ( Rdir.z * Rdir.z );
        if ( isZero( a ) ) {
            return internal::intersectCylinderCaps( R, C, tMin, tMax );
        }

        const real_t b = ( real_t( 2 ) * R.origin.x * Rdir.x ) + ( real_t( 2 ) * R.origin.z * Rdir.z );
        const real_t c = ( R.origin.x * R.origin.x ) + ( R.origin.z * R.origin.z ) - real_t( 1 );

        const real_t disc = b * b - real_t( 4 ) * a * c;
        if ( disc < 0 ) {
            return false;
        }
        const real_t disc_root = sqrt( disc );

        real_t t0 = ( -b - disc_root ) / ( real_t( 2 ) * a );
        real_t t1 = ( -b + disc_root ) / ( real_t( 2 ) * a );
        if ( t0 < t1 ) {
            std::swap( t0, t1 );
        }

        auto hasResult = false;

        const auto y0 = R.origin.y + t0 * Rdir.y;
        if ( y0 < height( C ) && y0 > -height( C ) ) {
            tMin = t0;
            tMax = t0;
            hasResult = true;
        }

        const auto y1 = R.origin.y + t1 * Rdir.y;
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

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Cylinder &C, const Transformation &world, real_t &t0, real_t &t1 ) noexcept
    {
        // For better performance, use the inverse matrix
        return intersect( inverse( world )( R ), C, t0, t1 );
    }

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Cylinder &C, const Matrix4 &invWorld, real_t &t0, real_t &t1 ) noexcept
    {
        return intersect( invWorld * R, C, t0, t1 );
    }

    /**
       \brief Ray-Triangle intersection

       Implements Möller-Trumbore intersection algorithm
     */
    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Triangle &T, real_t &t ) noexcept
    {
        const auto E0 = edge0( T );
        const auto E1 = edge1( T );

        const auto dirCrossE1 = cross( direction( R ), E1 );
        const real_t det = dot( edge0( T ), dirCrossE1 );
        if ( isZero( det ) ) {
            return false;
        }

        const real_t f = 1 / det;

        const auto p0ToOrigin = origin( R ) - T.p0;
        const real_t u = f * dot( p0ToOrigin, dirCrossE1 );
        if ( u < 0 || u > 1 ) {
            return false;
        }

        const auto originCrossE0 = cross( p0ToOrigin, E0 );
        const real_t v = f * dot( direction( R ), originCrossE0 );
        if ( v < 0 || ( u + v ) > 1 ) {
            return false;
        }

        t = f * dot( E1, originCrossE0 );

        return true;
    }

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Triangle &T, const Transformation &world, real_t &t ) noexcept
    {
        // For better performance, use the inverse matrix
        return intersect( inverse( world )( R ), T, t );
    }

    [[nodiscard]] static constexpr bool intersect( const Ray3 &R, const Triangle &T, const Matrix4 &invWorld, real_t &t ) noexcept
    {
        return intersect( invWorld * R, T, t );
    }

}

#endif
