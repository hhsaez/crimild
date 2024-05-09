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

#ifndef CRIMILD_MATHEMATICS_IS_EQUAL_
#define CRIMILD_MATHEMATICS_IS_EQUAL_

#include "ColorRGB.hpp"
#include "ColorRGBA.hpp"
#include "Concepts.hpp"
#include "Matrix4.hpp"
#include "Normal3.hpp"
#include "Point3.hpp"
#include "Quaternion.hpp"
#include "Ray3.hpp"
#include "Transformation.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "isZero.hpp"

namespace crimild {

    template< ArithmeticType T, ArithmeticType U >
    inline constexpr bool isEqual( const T &x, const U &y ) noexcept
    {
        if constexpr ( std::is_floating_point< T >::value ) {
            return isZero( x - y );
        } else {
            return x == y;
        }
    }

    template< template< ArithmeticType > class Derived, ArithmeticType T, ArithmeticType U >
    inline constexpr bool isEqual( const Tuple2< Derived, T > &a, const Tuple2< Derived, U > &b ) noexcept
    {
        return isEqual( a.x, b.x ) && isEqual( a.y, b.y );
    }

    template< template< ArithmeticType > class Derived, ArithmeticType T, ArithmeticType U >
    inline constexpr bool isEqual( const Tuple3< Derived, T > &a, const Tuple3< Derived, U > &b ) noexcept
    {
        return isEqual( a.x, b.x ) && isEqual( a.y, b.y ) && isEqual( a.z, b.z );
    }

    template< template< ArithmeticType > class Derived, ArithmeticType T, ArithmeticType U >
    inline constexpr bool isEqual( const Tuple4< Derived, T > &a, const Tuple4< Derived, U > &b ) noexcept
    {
        return isEqual( a.x, b.x ) && isEqual( a.y, b.y ) && isEqual( a.z, b.z ) && isEqual( a.z, b.z );
    }

    template< ArithmeticType T, ArithmeticType U >
    inline constexpr bool isEqual( const ColorRGBImpl< T > &a, const ColorRGBImpl< U > &b ) noexcept
    {
        return isEqual( a.r, b.r ) && isEqual( a.g, b.g ) && isEqual( a.b, b.b );
    }

    template< ArithmeticType T, ArithmeticType U >
    inline constexpr bool isEqual( const ColorRGBAImpl< T > &a, const ColorRGBAImpl< U > &b ) noexcept
    {
        return isEqual( a.r, b.r ) && isEqual( a.g, b.g ) && isEqual( a.b, b.b ) && isEqual( a.a, b.a );
    }

    template< ArithmeticType T, ArithmeticType U >
    inline constexpr bool isEqual( const Matrix4Impl< T > &a, const Matrix4Impl< U > &b ) noexcept
    {
        return isEqual( a.columns[ 0 ], b.columns[ 0 ] ) && isEqual( a.columns[ 1 ], b.columns[ 1 ] ) && isEqual( a.columns[ 2 ], b.columns[ 2 ] ) && isEqual( a.columns[ 3 ], b.columns[ 3 ] );
    }

    [[nodiscard]] inline constexpr bool isEqual( const Transformation &a, const Transformation &b ) noexcept
    {
        // Check for contents first, since it should be faster
        // TODO: checking for `invMat` should not be necessary
        return isEqual( a.contents, b.contents ) && isEqual( a.mat, b.mat ) && isEqual( a.invMat, b.invMat );
    }

    [[nodiscard]] inline constexpr bool isEqual( const Ray3 a, const Ray3 &b ) noexcept
    {
        return isEqual( a.origin, b.origin ) && isEqual( a.direction, b.direction );
    }

    [[nodiscard]] inline constexpr bool isEqual( const Quaternion &q, const Quaternion r ) noexcept
    {
        return isEqual( q.v, r.v ) && isEqual( q.w, r.w );
    }

}

#endif
