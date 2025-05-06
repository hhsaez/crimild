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

#ifndef CRIMILD_MATHEMATICS_COLOR_RGB_
#define CRIMILD_MATHEMATICS_COLOR_RGB_

#include "Tuple3.hpp"
#include "concepts.hpp"
#include "types.hpp"

namespace crimild {

    template< ArithmeticType T >
    class ColorRGBImpl {
    public:
        struct Constants;

    public:
        T r = {};
        T g = {};
        T b = {};

    public:
        constexpr ColorRGBImpl( void ) noexcept = default;

        constexpr ColorRGBImpl( T r, T g, T b ) noexcept
            : r( r ), g( g ), b( b )
        {
            // do nothing
        }

        template< ArithmeticType U >
        constexpr explicit ColorRGBImpl( U value ) noexcept
            : ColorRGBImpl( value, value, value )
        {
            // do nothing
        }

        template< ArithmeticType U >
        constexpr ColorRGBImpl( const ColorRGBImpl< U > &other ) noexcept
            : ColorRGBImpl( other.r, other.g, other.b )
        {
            // do nothing
        }

        template< template< typename > class OtherDerived, ArithmeticType U >
        constexpr explicit ColorRGBImpl( const Tuple3< OtherDerived, U > &other ) noexcept
            : ColorRGBImpl( other.x, other.y, other.z ) { }

        ~ColorRGBImpl( void ) noexcept = default;

        template< ArithmeticType U >
        inline constexpr ColorRGBImpl< T > &operator=( const ColorRGBImpl< U > &other ) noexcept
        {
            r = other.r;
            g = other.g;
            b = other.b;
            return *this;
        }

        template< ArithmeticType U >
        inline constexpr bool operator==( const ColorRGBImpl< U > &other ) const noexcept
        {
            return r == other.r && g == other.g && b == other.b;
        }

        template< ArithmeticType U >
        inline constexpr bool operator!=( const ColorRGBImpl< U > &other ) const noexcept
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
            assert( index >= 0 && index <= 2 && "Invalid index" );
            switch ( index ) {
                case 0:
                    return r;
                case 1:
                    return g;
                default:
                    return b;
            }
        }

        [[nodiscard]] inline constexpr T &operator[]( size_t index ) noexcept
        {
            assert( index >= 0 && index <= 2 && "Invalid index" );
            switch ( index ) {
                case 0:
                    return r;
                case 1:
                    return g;
                default:
                    return b;
            }
        }
        ///@}

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator+( const ColorRGBImpl< U > &u ) const noexcept
        {
            return ColorRGBImpl< decltype( T {} + U {} ) > {
                r + u.r,
                g + u.g,
                b + u.b,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator-( const ColorRGBImpl< U > &u ) const noexcept
        {
            return ColorRGBImpl< decltype( T {} - U {} ) > {
                r - u.r,
                g - u.g,
                b - u.b,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator*( const U &s ) const noexcept
        {
            return ColorRGBImpl< decltype( T {} * U {} ) > {
                r * s,
                g * s,
                b * s,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] friend inline constexpr auto operator*( const U &s, const ColorRGBImpl< T > &u ) noexcept
        {
            return ColorRGBImpl< decltype( T {} * U {} ) > {
                u.r * s,
                u.g * s,
                u.b * s,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator*( const ColorRGBImpl< U > &u ) const noexcept
        {
            return ColorRGBImpl< decltype( T {} * U {} ) > {
                r * u.r,
                g * u.g,
                b * u.b,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator/( U s ) const noexcept
        {
            const auto invS = real_t( 1 ) / s;
            return ( *this * invS );
        }
    };

    template< ArithmeticType T >
    struct ColorRGBImpl< T >::Constants {
        static constexpr auto BLACK = ColorRGBImpl< T > { 0, 0, 0 };
        static constexpr auto WHITE = ColorRGBImpl< T > { 1, 1, 1 };
        static constexpr auto RED = ColorRGBImpl< T > { 1, 0, 0 };
        static constexpr auto GREEN = ColorRGBImpl< T > { 0, 1, 0 };
        static constexpr auto BLUE = ColorRGBImpl< T > { 0, 0, 1 };
    };

    using ColorRGB = ColorRGBImpl< real_t >;
    using ColorRGBf = ColorRGBImpl< float >;
    using ColorRGBd = ColorRGBImpl< double >;

}

#endif
