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

#ifndef CRIMILD_MATHEMATICS_UTILS_
#define CRIMILD_MATHEMATICS_UTILS_

#include "Foundation/Types.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Traits.hpp"

#include <cmath>

namespace crimild {

    template< typename T >
    inline constexpr T abs( const T &x ) noexcept
    {
        if constexpr ( traits::isReal< T >() ) {
            // std::fabs cannot be used in constexpr
            return x > 0 ? x : -x;
        } else {
            return std::abs( x );
        }
    }

    template< typename T >
    inline constexpr Bool isZero( const T &x ) noexcept
    {
        if constexpr ( traits::isReal< T >() ) {
            return abs( x ) < numbers::EPSILON;
        } else {
            return x == 0;
        }
    }

    template< typename T >
    inline constexpr Bool isEqual( const T &x, const T &y ) noexcept
    {
        if constexpr ( traits::isReal< T >() ) {
            return isZero( x - y );
        } else {
            return x == y;
        }
    }

    template< typename T >
    inline constexpr Bool isNaN( const T &value ) noexcept
    {
        // std::isnan cannot be used in constexpr

        // This might not be 100% correct in all platforms, but so far it's good
        return value != value;
    }

    template< typename T >
    [[nodiscard]] inline constexpr T min( T x, T y ) noexcept
    {
        return x < y ? x : y;
    }

    template< typename T >
    [[nodiscard]] inline constexpr T max( T x, T y ) noexcept
    {
        return x > y ? x : y;
    }

    namespace impl {

        template< typename T >
        constexpr T sqrt( T x, T curr, T prev ) noexcept
        {
            return (
                curr == prev
                    ? curr
                    : sqrt< T >( x, 0.5 * ( curr + x / curr ), curr ) );
        }

    }

    /**
       \brief Square root aproximation using constant expresions
       Precision is good enough for float types.
       Error is 2.22045e-16 when compared with std::sqrt
    */
    template< typename T >
    [[nodiscard]] inline constexpr T sqrt( T x ) noexcept
    {
        if constexpr ( traits::isHighPrecision< Real >() ) {
            return std::sqrt( x );
        } else {
            return impl::sqrt< T >( x, x, 0 );
        }
    }

    template< typename T >
    [[nodiscard]] inline constexpr T floor( T x ) noexcept
    {
        return std::floor( x );
    }

    template< typename T >
    [[nodiscard]] inline constexpr T ceil( T x ) noexcept
    {
        return std::ceil( x );
    }

    template< typename T >
    [[nodiscard]] inline constexpr T lerp( T x, T y, Real t ) noexcept
    {
        return ( Real( 1 ) - t ) * x + t * y;
    }

    [[nodiscard]] inline constexpr Radians radians( Degrees deg ) noexcept
    {
        return deg * numbers::PI / 180.0;
    }

    [[nodiscard]] inline constexpr Degrees degrees( Radians rad ) noexcept
    {
        return rad * 180.0 / numbers::PI;
    }

    // TODO: Make this function constexpr
    [[nodiscard]] inline Real cos( Radians rad ) noexcept
    {
        return std::cos( rad );
    }

    // TODO: Make this function constexpr
    [[nodiscard]] inline Real sin( Radians rad ) noexcept
    {
        return std::sin( rad );
    }

}

#endif
