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

#ifndef CRIMILD_MATHEMATICS_SWIZZLE_
#define CRIMILD_MATHEMATICS_SWIZZLE_

#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/tupleBuilder.hpp"

namespace crimild {

    template< typename T >
    [[nodiscard]] inline constexpr Point2Impl< T > point2( const Tuple2Impl< T > &t ) noexcept
    {
        return Point2Impl< T > { t.x, t.y };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Point3Impl< T > point3( const Tuple3Impl< T > &t ) noexcept
    {
        return Point3Impl< T > { t.x, t.y, t.z };
    }

    // template< typename T >
    // [[nodiscard]] inline constexpr Vector2Impl< T > vector2( const Tuple2Impl< T > &t ) noexcept
    // {
    //     return Vector2Impl< T > { t.x, t.y };
    // }

    template< typename T >
    [[nodiscard]] inline constexpr Vector3Impl< T > vector3( const Tuple3Impl< T > &t ) noexcept
    {
        return Vector3Impl< T > { t.x, t.y, t.z };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Vector3Impl< T > vector3( const Tuple4Impl< T > &t ) noexcept
    {
        return Vector3Impl< T > { t.x, t.y, t.z };
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector4Impl< T > vector4( const Tuple3Impl< T > &t, U w ) noexcept
    {
        return Vector4Impl< T > { t.x, t.y, t.z, T( w ) };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Normal3Impl< T > normal3( const Tuple3Impl< T > &t ) noexcept
    {
        return Normal3Impl< T > { t.x, t.y, t.z };
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr auto xyz( const TupleImpl< T > &t ) noexcept
    {
        return tuple3Builder< TupleImpl, T >( t.x, t.y, t.z );
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr auto xxx( const TupleImpl< T > &t ) noexcept
    {
        return tuple3Builder< TupleImpl, T >( t.x, t.x, t.x );
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr auto yyy( const TupleImpl< T > &t ) noexcept
    {
        return tuple3Builder< TupleImpl, T >( t.y, t.y, t.y );
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr auto xyz0( const TupleImpl< T > &t ) noexcept
    {
        return tuple4Builder< TupleImpl, T >( t.x, t.y, t.z, 0 );
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr auto xyz1( const TupleImpl< T > &t ) noexcept
    {
        return tuple4Builder< TupleImpl, T >( t.x, t.y, t.z, 1 );
    }

    [[nodiscard]] inline constexpr auto rgb( const ColorRGBA &c ) noexcept
    {
        return ColorRGB { c.r, c.g, c.b };
    }

    template< typename T >
    [[nodiscard]] inline constexpr auto rgb( const Tuple3Impl< T > &t ) noexcept
    {
        return ColorRGB { t.x, t.y, t.z };
    }

    [[nodiscard]] inline constexpr auto rgba( const ColorRGB &c ) noexcept
    {
        return ColorRGBA { c.r, c.g, c.b, 1.0 };
    }

    template< typename T >
    [[nodiscard]] inline constexpr auto rgba( const Tuple4Impl< T > &t ) noexcept
    {
        return ColorRGB { t.x, t.y, t.z, t.w };
    }

    template< typename T >
    [[nodiscard]] inline constexpr auto rgba( const Tuple3Impl< T > &t, Real a ) noexcept
    {
        return ColorRGBA { t.x, t.y, t.z, a };
    }

}

#endif
