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

#ifndef CRIMILD_MATHEMATICS_IS_ZERO_
#define CRIMILD_MATHEMATICS_IS_ZERO_

#include "ColorRGB.hpp"
#include "Concepts.hpp"
#include "Numbers.hpp"
#include "Tuple2.hpp"
#include "Tuple3.hpp"
#include "Tuple4.hpp"
#include "abs.hpp"

namespace crimild {

    template< ArithmeticType T >
    inline constexpr bool isZero( const T &x ) noexcept
    {
        if constexpr ( std::is_floating_point< T >::value ) {
            return abs( x ) < numbers::EPSILON;
        } else {
            return x == 0;
        }
    }

    template< template< ArithmeticType > class Derived, ArithmeticType T >
    [[nodiscard]] inline constexpr bool isZero( const Tuple2< Derived, T > &t ) noexcept
    {
        return isZero( t.x ) && isZero( t.y );
    }

    template< template< ArithmeticType > class Derived, ArithmeticType T >
    [[nodiscard]] inline constexpr bool isZero( const Tuple3< Derived, T > &t ) noexcept
    {
        return isZero( t.x ) && isZero( t.y ) && isZero( t.z );
    }

    template< template< ArithmeticType > class Derived, ArithmeticType T >
    [[nodiscard]] inline constexpr bool isZero( const Tuple4< Derived, T > &t ) noexcept
    {
        return isZero( t.x ) && isZero( t.y ) && isZero( t.z ) && isZero( t.w );
    }

    template< ArithmeticType T >
    [[nodiscard]] inline constexpr bool isZero( const ColorRGBImpl< T > &t ) noexcept
    {
        return isZero( t.r ) && isZero( t.g ) && isZero( t.b );
    }

}

#endif
