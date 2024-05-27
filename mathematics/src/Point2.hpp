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

#ifndef CRIMILD_MATHEMATICS_POINT_2_
#define CRIMILD_MATHEMATICS_POINT_2_

#include "Tuple2.hpp"
#include "Vector2.hpp"
#include "types.hpp"

namespace crimild {

    template< ArithmeticType T >
    class Point2Impl : public Tuple2< Point2Impl, T > {
    public:
        struct Constants {
            static constexpr Point2Impl ZERO = { 0, 0 };
            static constexpr Point2Impl ONE = { 1, 1 };
            static constexpr Point2Impl POSITIVE_INFINITY = {
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity()
            };
            static constexpr Point2Impl NEGATIVE_INFINITY = {
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity()
            };
        };

    public:
        using Tuple2< Point2Impl, T >::x;
        using Tuple2< Point2Impl, T >::y;

        constexpr Point2Impl( void ) noexcept = default;

        constexpr Point2Impl( T x, T y ) noexcept
            : Tuple2< Point2Impl, T >( x, y )
        {
        }

        template< ArithmeticType U >
        constexpr explicit Point2Impl( U value ) noexcept
            : Tuple2< Point2Impl, T >( value ) { }

        ~Point2Impl( void ) noexcept = default;

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator+( const Point2Impl< U > &v ) const noexcept
        {
            return Tuple2< Point2Impl, T >::operator+( v );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator+( const Vector2Impl< U > &v ) const noexcept
        {
            return Point2Impl< decltype( T {} + U {} ) > {
                x + v.x,
                y + v.y,
            };
        }

        template< ArithmeticType U >
        inline constexpr auto &operator+=( const Vector2Impl< U > &other ) noexcept
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        // Special case: Subtracting two points always results in a vector.
        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator-( const Point2Impl< U > &p ) const noexcept
        {
            return Vector2Impl< decltype( T {} - U {} ) > {
                x - p.x,
                y - p.y,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator-( const Vector2Impl< U > &v ) const noexcept
        {
            return Point2Impl< decltype( T {} - U {} ) > {
                x - v.x,
                y - v.y,
            };
        }

        template< ArithmeticType U >
        inline constexpr auto &operator-=( const Vector2Impl< U > &other ) noexcept
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }
    };

    using Point2 = Point2Impl< real_t >;
    using Point2f = Point2Impl< float >;
    using Point2d = Point2Impl< double >;
    using Point2i = Point2Impl< int32_t >;
    using Point2ui = Point2Impl< uint32_t >;

}

#endif
