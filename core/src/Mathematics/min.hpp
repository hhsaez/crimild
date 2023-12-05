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

#ifndef CRIMILD_MATHEMATICS_MIN_
#define CRIMILD_MATHEMATICS_MIN_

#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Traits.hpp"

namespace crimild {

    template< concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto min( T x, U y ) noexcept
    {
        return x < y ? x : y;
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto min( const Tuple2< Derived, T > &v ) noexcept
    {
        return min( v.x, v.y );
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto min( const Tuple2< Derived, T > &u, const Tuple2< Derived, U > &v ) noexcept
    {
        return Derived< decltype( min( T {}, U {} ) ) > {
            min( u.x, v.x ),
            min( u.y, v.y ),
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto minDimension( const Tuple2< Derived, T > &t ) noexcept
    {
        size_t ret = 0;
        ret = t[ 1 ] < t[ ret ] ? 1 : ret;
        return ret;
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto min( const Tuple3< Derived, T > &v ) noexcept
    {
        return min( v.x, min( v.yv.z ) );
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto min( const Tuple3< Derived, T > &u, const Tuple3< Derived, U > &v ) noexcept
    {
        return Derived< decltype( min( T {}, U {} ) ) > {
            min( u.x, v.x ),
            min( u.y, v.y ),
            min( u.z, v.z ),
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto minDimension( const Tuple3< Derived, T > &t ) noexcept
    {
        size_t ret = 0;
        ret = t[ 1 ] < t[ ret ] ? 1 : ret;
        ret = t[ 2 ] < t[ ret ] ? 2 : ret;
        return ret;
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto min( const Tuple4< Derived, T > &v ) noexcept
    {
        return min( v.x, min( v.y, min( v.z, v.w ) ) );
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto min( const Tuple4< Derived, T > &u, const Tuple4< Derived, U > &v ) noexcept
    {
        return Derived< decltype( min( T {}, U {} ) ) > {
            min( u.x, v.x ),
            min( u.y, v.y ),
            min( u.z, v.z ),
            min( u.w, v.w ),
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto minDimension( const Tuple4< Derived, T > &t ) noexcept
    {
        size_t ret = 0;
        ret = t[ 1 ] < t[ ret ] ? 1 : ret;
        ret = t[ 2 ] < t[ ret ] ? 2 : ret;
        ret = t[ 3 ] < t[ ret ] ? 3 : ret;
        return ret;
    }

    template< typename T >
    [[nodiscard]] inline constexpr T min( const ColorRGBImpl< T > &c ) noexcept
    {
        return min( c.r, min( c.g, c.b ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr ColorRGBImpl< T > min( const ColorRGBImpl< T > &a, const ColorRGBImpl< T > &b ) noexcept
    {
        return ColorRGBImpl< T > {
            min( a.r, b.r ),
            min( a.g, b.g ),
            min( a.b, b.b ),
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Size minDimension( const ColorRGBImpl< T > &c ) noexcept
    {
        auto ret = Size( 0 );
        for ( auto i = 1l; i < 3; ++i ) {
            if ( c[ i ] < c[ ret ] ) {
                ret = i;
            }
        }
        return ret;
    }

    template< typename T >
    [[nodiscard]] inline constexpr T min( const ColorRGBAImpl< T > &c ) noexcept
    {
        return min( c.r, min( c.g, min( c.b, c.a ) ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr ColorRGBAImpl< T > min( const ColorRGBAImpl< T > &a, const ColorRGBAImpl< T > &b ) noexcept
    {
        return ColorRGBAImpl< T > {
            min( a.r, b.r ),
            min( a.g, b.g ),
            min( a.b, b.b ),
            min( a.a, b.a ),
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Size minDimension( const ColorRGBAImpl< T > &c ) noexcept
    {
        auto ret = Size( 0 );
        for ( auto i = 1l; i < 4; ++i ) {
            if ( c[ i ] < c[ ret ] ) {
                ret = i;
            }
        }
        return ret;
    }

}

#endif
