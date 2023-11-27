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

#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Concepts.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Normal3.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Ray3.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector4.hpp"
#include "Mathematics/isZero.hpp"

namespace crimild {

    template< concepts::Arithmetic T >
    inline constexpr Bool isEqual( const T &x, const T &y ) noexcept
    {
        if constexpr ( traits::isReal< T >() ) {
            return isZero( x - y );
        } else {
            return x == y;
        }
    }

    template< concepts::Arithmetic T, concepts::Arithmetic U >
    inline constexpr Bool isEqual( const ColorRGBImpl< T > &a, const ColorRGBImpl< U > &b ) noexcept
    {
        return isEqual( a.r, b.r ) && isEqual( a.g, b.g ) && isEqual( a.b, b.b );
    }

    template< concepts::Arithmetic T, concepts::Arithmetic U >
    inline constexpr Bool isEqual( const ColorRGBAImpl< T > &a, const ColorRGBAImpl< U > &b ) noexcept
    {
        return isEqual( a.r, b.r ) && isEqual( a.g, b.g ) && isEqual( a.b, b.b ) && isEqual( a.a, b.a );
    }

    template< concepts::Arithmetic T, concepts::Arithmetic U >
    inline constexpr Bool isEqual( const Matrix4Impl< T > &a, const Matrix4Impl< U > &b ) noexcept
    {
        return isEqual( a.c0, b.c0 ) && isEqual( a.c1, b.c1 ) && isEqual( a.c2, b.c2 ) && isEqual( a.c3, b.c3 );
    }

    template< concepts::Arithmetic T, concepts::Arithmetic U >
    inline constexpr Bool isEqual( const Normal3Impl< T > &a, const Normal3Impl< U > &b ) noexcept
    {
        return isEqual( a.x, b.x ) && isEqual( a.y, b.y ) && isEqual( a.z, b.z );
    }

    template< concepts::Arithmetic T, concepts::Arithmetic U >
    inline constexpr Bool isEqual( const Point3Impl< T > &a, const Point3Impl< U > &b ) noexcept
    {
        return isEqual( a.x, b.x ) && isEqual( a.y, b.y ) && isEqual( a.z, b.z );
    }

    template< concepts::Arithmetic T, concepts::Arithmetic U >
    inline constexpr Bool isEqual( const Vector2Impl< T > &a, const Vector2Impl< U > &b ) noexcept
    {
        return isEqual( a.x, b.x ) && isEqual( a.y, b.y );
    }

    template< concepts::Arithmetic T, concepts::Arithmetic U >
    inline constexpr Bool isEqual( const Vector3Impl< T > &a, const Vector3Impl< U > &b ) noexcept
    {
        return isEqual( a.x, b.x ) && isEqual( a.y, b.y ) && isEqual( a.z, b.z );
    }

    template< concepts::Arithmetic T, concepts::Arithmetic U >
    inline constexpr Bool isEqual( const Vector4Impl< T > &a, const Vector4Impl< U > &b ) noexcept
    {
        return isEqual( a.x, b.x ) && isEqual( a.y, b.y ) && isEqual( a.z, b.z ) && isEqual( a.w, b.w );
    }

    [[nodiscard]] inline constexpr Bool isEqual( const Transformation &a, const Transformation &b ) noexcept
    {
        // Check for contents first, since it should be faster
        // TODO: checking for `invMat` should not be necessary
        return isEqual( a.contents, b.contents ) && isEqual( a.mat, b.mat ) && isEqual( a.invMat, b.invMat );
    }

    [[nodiscard]] inline constexpr Bool isEqual( const Ray3 a, const Ray3 &b ) noexcept
    {
        return isEqual( a.o, b.o ) && isEqual( a.d, b.d );
    }

}

#endif
