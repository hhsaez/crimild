/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_MATHEMATICS_NORMAL_
#define CRIMILD_MATHEMATICS_NORMAL_

#include "Box.hpp"
#include "Cylinder.hpp"
#include "Normal3.hpp"
#include "Plane3.hpp"
#include "Point3.hpp"
#include "Sphere.hpp"
#include "Transformation.hpp"
#include "Triangle.hpp"
#include "abs.hpp"
#include "edges.hpp"
#include "height.hpp"
#include "inverse.hpp"
#include "max.hpp"
#include "normalize.hpp"
#include "origin.hpp"
#include "radius.hpp"
#include "swizzle.hpp"

namespace crimild {

    template< ArithmeticType T >
    [[nodiscard]] inline constexpr Normal3 normal( const BoxImpl< T > &B, const Point3f &P ) noexcept
    {
        const auto absP = abs( P );
        const auto i = maxDimension( absP );
        Normal3 N = { 0, 0, 0 };
        N[ i ] = P[ i ] > 0 ? 1 : -1;
        return N;
    }

    template< ArithmeticType BoxType >
    [[nodiscard]] inline constexpr Normal3 normal( const BoxImpl< BoxType > &B, const Transformation &T, const Point3f &P ) noexcept
    {
        const auto N = normal( B, inverse( T )( P ) );
        return normalize( T( N ) );
    }

    [[nodiscard]] inline constexpr const Normal3 &normal( const Plane3 &p ) noexcept
    {
        return p.n;
    }

    [[nodiscard]] inline constexpr Normal3 normal( const Sphere &S, const Point3f &P ) noexcept
    {
        return normalize( Normal3( P - origin( S ) ) );
    }

    [[nodiscard]] inline constexpr Normal3 normal( const Sphere &S, const Transformation &T, const Point3f &P ) noexcept
    {
        const auto localP = inverse( T )( P );
        const auto N = Normal3( localP - origin( S ) );
        return normalize( T( N ) );
    }

    [[nodiscard]] inline constexpr Normal3 normal( const Triangle &T, const Point3f & ) noexcept
    {
        [[maybe_unused]] const auto [ E0, E1, E2 ] = edges( T );
        return normalize( Normal3( cross( E0, E1 ) ) );
    }

    [[nodiscard]] inline constexpr Normal3 normal( const Triangle &T, const Transformation &X, const Point3f &P ) noexcept
    {
        const auto N = normal( T, P );
        return normalize( X( N ) );
    }

    // Project the point in the XZ plane and normalize
    // TODO(hernan): Take into account the cylinder's center
    [[nodiscard]] inline constexpr Normal3 normal( const Cylinder &C, const Point3f &P ) noexcept
    {
        const real_t dist = P.x * P.x + P.z * P.z;
        const real_t r = radius( C );
        const real_t r2 = r * r;
        const real_t h = height( C );

        if ( dist < r2 && P.y >= -h - numbers::EPSILON ) {
            return Normal3 { 0, 1, 0 };
        }

        if ( dist < r2 && P.y <= h + numbers::EPSILON ) {
            return Normal3 { 0, -1, 0 };
        }

        return Normal3 { P.x, 0, P.z };
    }

    [[nodiscard]] inline constexpr Normal3 normal( const Cylinder &C, const Transformation &T, const Point3f &P ) noexcept
    {
        const auto localP = inverse( T )( P );
        return normalize( T( normal( C, localP ) ) );
    }

}

#endif
