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
#include "Mathematics/tupleBuilder.hpp"
#include "Mathematics/tupleComponents.hpp"

namespace crimild {

    template< typename T >
    [[nodiscard]] inline constexpr T min( T x, T y ) noexcept
    {
        return x < y ? x : y;
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr T min( const TupleImpl< T > &t ) noexcept
    {
        auto ret = t[ 0 ];
        constexpr auto N = traits::tupleComponents< TupleImpl >();
        for ( auto i = 1l; i < N; ++i ) {
            ret = min( ret, t[ i ] );
        }
        return ret;
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr auto min( const TupleImpl< T > &t, const TupleImpl< T > &u ) noexcept
    {
        constexpr auto N = traits::tupleComponents< TupleImpl >();
        if constexpr ( N == 2 ) {
            return tuple2Builder< TupleImpl, T >(
                min( t.x, u.x ),
                min( t.y, u.y ) );
        } else if constexpr ( N == 3 ) {
            return tuple3Builder< TupleImpl, T >(
                min( t.x, u.x ),
                min( t.y, u.y ),
                min( t.z, u.z ) );
        } else {
            return tuple4Builder< TupleImpl, T >(
                min( t.x, u.x ),
                min( t.y, u.y ),
                min( t.z, u.z ),
                min( t.w, u.w ) );
        }
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr Size minDimension( const TupleImpl< T > &t ) noexcept
    {
        auto ret = Size( 0 );
        constexpr auto N = traits::tupleComponents< TupleImpl >();
        for ( auto i = 1l; i < N; ++i ) {
            if ( t[ i ] < t[ ret ] ) {
                ret = i;
            }
        }
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