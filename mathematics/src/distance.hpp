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

#ifndef CRIMILD_MATHEMATICS_DISTANCE_
#define CRIMILD_MATHEMATICS_DISTANCE_

#include "Plane3.hpp"
#include "Point3.hpp"
#include "Ray3.hpp"
#include "Tuple2.hpp"
#include "abs.hpp"
#include "dot.hpp"
#include "length.hpp"

namespace crimild {

    template< template< ArithmeticType > class Derived, ArithmeticType T, ArithmeticType U >
    [[nodiscard]] inline constexpr real_t distanceSquared( const Tuple2< Derived, T > &u, const Tuple2< Derived, U > &v ) noexcept
    {
        return lengthSquared( u - v );
    }

    template< ArithmeticType T, ArithmeticType U >
    [[nodiscard]] inline constexpr real_t distanceSquared( const Point3Impl< T > &u, const Point3Impl< U > &v ) noexcept
    {
        return lengthSquared( v - u );
    }

    template< ArithmeticType T, ArithmeticType U >
    [[nodiscard]] inline constexpr real_t distance( const Point3Impl< T > &u, const Point3Impl< U > &v ) noexcept
    {
        return sqrt( lengthSquared( v - u ) );
    }

    [[nodiscard]] constexpr real_t distanceSquared( const Ray3 &R, const Point3f &P ) noexcept
    {
        const auto V = P - origin( R );
        const auto d = dot( V, direction( R ) );
        return ( dot( V, V ) - d * d ) / lengthSquared( direction( R ) );
    }

    [[nodiscard]] constexpr real_t distance( const Ray3 &R, const Point3f &P ) noexcept
    {
        return sqrt( distanceSquared( R, P ) );
    }

    [[nodiscard]] constexpr real_t distanceSigned( const Plane3 &A, const Point3f &P ) noexcept
    {
        return dot( normal( A ), Vector3 { P.x, P.y, P.z } ) + distance( A );
    }

    [[nodiscard]] constexpr real_t distance( const Plane3 &A, const Point3f &P ) noexcept
    {
        return abs( distanceSigned( A, P ) );
    }

}

#endif
