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

#ifndef CRIMILD_MATHEMATICS_POINT_3_
#define CRIMILD_MATHEMATICS_POINT_3_

#include "Tuple3.hpp"
#include "Tuple4.hpp"
#include "Types.hpp"
#include "Vector3.hpp"

#include <limits>

namespace crimild {

    template< ArithmeticType T >
    class Point3Impl : public Tuple3< Point3Impl, T > {
    public:
        struct Constants {
            static constexpr Point3Impl ZERO = { 0, 0, 0 };
            static constexpr Point3Impl ONE = { 1, 1, 1 };
            static constexpr Point3Impl POSITIVE_INFINITY = {
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity()
            };
            static constexpr Point3Impl NEGATIVE_INFINITY = {
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity()
            };
        };

    public:
        using Tuple3< Point3Impl, T >::x;
        using Tuple3< Point3Impl, T >::y;
        using Tuple3< Point3Impl, T >::z;

        constexpr Point3Impl( void ) noexcept = default;

        constexpr Point3Impl( T x, T y, T z ) noexcept
            : Tuple3< Point3Impl, T >( x, y, z )
        {
        }

        template< ArithmeticType U >
        constexpr explicit Point3Impl( U value ) noexcept
            : Tuple3< Point3Impl, T >( value ) { }

        template< ArithmeticType U >
        constexpr Point3Impl( const Point3Impl< U > &other ) noexcept
            : Tuple3< Point3Impl, T >( other ) { }

        template< template< ArithmeticType > class OtherDerived, ArithmeticType U >
        constexpr explicit Point3Impl( const Tuple3< OtherDerived, U > &other ) noexcept
            : Tuple3< Point3Impl, T >( other.x, other.y, other.z ) { }

        template< template< ArithmeticType > class OtherDerived, ArithmeticType U >
        constexpr Point3Impl( const Tuple4< OtherDerived, U > &other ) noexcept
            : Tuple3< Point3Impl, T >( other.x, other.y, other.z ) { }

        ~Point3Impl( void ) noexcept = default;

        template< ArithmeticType U >
        inline constexpr Point3Impl< T > &operator=( const Point3Impl< U > &other ) noexcept
        {
            x = other.x;
            y = other.y;
            z = other.z;
            return *this;
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator+( const Point3Impl< U > &v ) const noexcept
        {
            return Tuple3< Point3Impl, T >::operator+( v );
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator+( const Vector3Impl< U > &v ) const noexcept
        {
            return Point3Impl< decltype( T {} + U {} ) > {
                x + v.x,
                y + v.y,
                z + v.z,
            };
        }

        template< ArithmeticType U >
        inline constexpr auto &operator+=( const Vector3Impl< U > &other ) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        // Special case: Subtracting two points always results in a vector.
        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator-( const Point3Impl< U > &p ) const noexcept
        {
            return Vector3Impl< decltype( T {} - U {} ) > {
                x - p.x,
                y - p.y,
                z - p.z,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator-( const Vector3Impl< U > &v ) const noexcept
        {
            return Point3Impl< decltype( T {} - U {} ) > {
                x - v.x,
                y - v.y,
                z - v.z,
            };
        }

        template< ArithmeticType U >
        inline constexpr auto &operator-=( const Vector3Impl< U > &other ) noexcept
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        [[nodiscard]] inline constexpr auto operator-( void ) const noexcept
        {
            return Point3Impl< T > {
                -x,
                -y,
                -z
            };
        }
    };

    using Point3 = Point3Impl< real_t >;
    using Point3f = Point3Impl< float >;
    using Point3d = Point3Impl< double >;
    using Point3i = Point3Impl< int32_t >;
    using Point3ui = Point3Impl< uint32_t >;

}

#endif
