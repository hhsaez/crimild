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

#ifndef CRIMILD_MATHEMATICS_COLOR_RGBA_
#define CRIMILD_MATHEMATICS_COLOR_RGBA_

#include "Tuple4.hpp"
#include "concepts.hpp"
#include "types.hpp"

#include <cassert>

namespace crimild {

    template< ArithmeticType T >
    class ColorRGBAImpl {
    public:
        struct Constants;

    public:
        T r = {};
        T g = {};
        T b = {};
        T a = {};

    public:
        constexpr ColorRGBAImpl( void ) noexcept = default;

        constexpr ColorRGBAImpl( T r, T g, T b, T a ) noexcept
            : r( r ), g( g ), b( b ), a( a )
        {
            // do nothing
        }

        template< ArithmeticType U >
        constexpr explicit ColorRGBAImpl( U value ) noexcept
            : ColorRGBAImpl( value, value, value, value )
        {
            // do nothing
        }

        template< ArithmeticType U >
        constexpr ColorRGBAImpl( const ColorRGBAImpl< U > &other ) noexcept
            : ColorRGBAImpl( other.r, other.g, other.b, other.a )
        {
            // do nothing
        }

        template< template< ArithmeticType > class OtherDerived, ArithmeticType U >
        constexpr explicit ColorRGBAImpl( const Tuple4< OtherDerived, U > &other ) noexcept
            : ColorRGBAImpl( other.x, other.y, other.z, other.w )
        {
            // do nothing
        }

        ~ColorRGBAImpl( void ) noexcept = default;

        template< ArithmeticType U >
        inline constexpr ColorRGBAImpl< T > &operator=( const ColorRGBAImpl< U > &other ) noexcept
        {
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            return *this;
        }

        template< ArithmeticType U >
        inline constexpr bool operator==( const ColorRGBAImpl< U > &other ) const noexcept
        {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }

        template< ArithmeticType U >
        inline constexpr bool operator!=( const ColorRGBAImpl< U > &other ) const noexcept
        {
            return !( *this == other );
        }

        ///@{
        /**
         * @brief Index-based accessors for color components
         *
         * @details
         * Some rutines do find it more useful to access color components inside a loop. The non-const
         * overload returns a reference, allowing to set the values of each component while indexing.
         */
        [[nodiscard]] inline constexpr T operator[]( size_t index ) const noexcept
        {
            assert( index >= 0 && index <= 3 && "Invalid index" );
            switch ( index ) {
                case 0:
                    return r;
                case 1:
                    return g;
                case 2:
                    return b;
                case 3:
                default:
                    return a;
            }
        }

        [[nodiscard]] inline constexpr T &operator[]( size_t index ) noexcept
        {
            assert( index >= 0 && index <= 3 && "Invalid index" );
            switch ( index ) {
                case 0:
                    return r;
                case 1:
                    return g;
                case 2:
                    return b;
                case 3:
                default:
                    return a;
            }
        }
        ///@}

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator+( const ColorRGBAImpl< U > &u ) const noexcept
        {
            return ColorRGBAImpl< decltype( T {} + U {} ) > {
                r + u.r,
                g + u.g,
                b + u.b,
                a + u.a,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator-( const ColorRGBAImpl< U > &u ) const noexcept
        {
            return ColorRGBAImpl< decltype( T {} - U {} ) > {
                r - u.r,
                g - u.g,
                b - u.b,
                a - u.a,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator*( const U &s ) const noexcept
        {
            return ColorRGBAImpl< decltype( T {} * U {} ) > {
                r * s,
                g * s,
                b * s,
                a * s,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] friend inline constexpr auto operator*( const U &s, const ColorRGBAImpl< T > &u ) noexcept
        {
            return ColorRGBAImpl< decltype( T {} * U {} ) > {
                u.r * s,
                u.g * s,
                u.b * s,
                u.a * s,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator*( const ColorRGBAImpl< U > &u ) const noexcept
        {
            return ColorRGBAImpl< decltype( T {} * U {} ) > {
                r * u.r,
                g * u.g,
                b * u.b,
                a * u.a,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator/( const U &s ) const noexcept
        {
            const auto invS = real_t( 1 ) / s;
            return ( *this * invS );
        }
    };

    template< ArithmeticType T >
    struct ColorRGBAImpl< T >::Constants {
        static constexpr auto CLEAR = ColorRGBAImpl< T > { 0, 0, 0, 0 };
        static constexpr auto BLACK = ColorRGBAImpl< T > { 0, 0, 0, 1 };
        static constexpr auto WHITE = ColorRGBAImpl< T > { 1, 1, 1, 1 };
        static constexpr auto RED = ColorRGBAImpl< T > { 1, 0, 0, 1 };
        static constexpr auto GREEN = ColorRGBAImpl< T > { 0, 1, 0, 1 };
        static constexpr auto BLUE = ColorRGBAImpl< T > { 0, 0, 1, 1 };
    };

    using ColorRGBA = ColorRGBAImpl< real_t >;
    using ColorRGBAf = ColorRGBAImpl< float >;
    using ColorRGBAd = ColorRGBAImpl< double >;

}

#endif
