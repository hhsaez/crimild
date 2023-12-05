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

#include "Mathematics/Tuple3.hpp"
#include "Mathematics/Vector3.hpp"

#include <limits>

namespace crimild {

    template< concepts::Arithmetic T >
    class Point3 : public Tuple3< Point3, T > {
    public:
        static const Point3 ZERO;
        static const Point3 ONE;
        static const Point3 POSITIVE_INFINITY;
        static const Point3 NEGATIVE_INFINITY;

    public:
        using Tuple3< Point3, T >::x;
        using Tuple3< Point3, T >::y;
        using Tuple3< Point3, T >::z;

        constexpr Point3( void ) noexcept = default;

        constexpr Point3( T x, T y, T z ) noexcept
            : Tuple3< Point3, T >( x, y, z )
        {
        }

        template< concepts::Arithmetic U >
        constexpr explicit Point3( U value ) noexcept
            : Tuple3< Point3, T >( value ) { }

        template< template< concepts::Arithmetic > class OtherTuple, concepts::Arithmetic U >
        constexpr explicit Point3( const OtherTuple< U > &other ) noexcept
            : Tuple3< Point3, T >( other.x, other.y, other.z ) { }

        ~Point3( void ) noexcept = default;

        template< concepts::Arithmetic U >
        inline constexpr Point3< T > &operator=( const Point3< U > &other ) noexcept
        {
            x = other.x;
            y = other.y;
            z = other.z;
            return *this;
        }

        template< concepts::Arithmetic U >
        [[nodiscard]] inline constexpr auto operator+( const Vector3< U > &v ) const noexcept
        {
            return Point3< decltype( T {} + U {} ) > {
                x + v.x,
                y + v.y,
                z + v.z,
            };
        }

        template< concepts::Arithmetic U >
        inline constexpr auto &operator+=( const Vector3< U > &other ) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        // Special case: Subtracting two points always results in a vector.
        template< concepts::Arithmetic U >
        [[nodiscard]] inline constexpr auto operator-( const Point3< U > &p ) const noexcept
        {
            return Vector3< decltype( T {} - U {} ) > {
                x - p.x,
                y - p.y,
                z - p.z,
            };
        }

        template< concepts::Arithmetic U >
        [[nodiscard]] inline constexpr auto operator-( const Vector3< T > &v ) noexcept
        {
            return Point3< decltype( T {} - U {} ) > {
                x - v.x,
                y - v.y,
                z - v.z,
            };
        }

        template< concepts::Arithmetic U >
        inline constexpr auto &operator-=( const Vector3< U > &other ) noexcept
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }
    };

    template< concepts::Arithmetic T >
    constexpr const Point3< T > Point3< T >::ZERO( 0 );

    template< concepts::Arithmetic T >
    constexpr const Point3< T > Point3< T >::ONE( 1 );

    template< concepts::Arithmetic T >
    constexpr const Point3< T > Point3< T >::POSITIVE_INFINITY(
        std::numeric_limits< T >::infinity(),
        std::numeric_limits< T >::infinity(),
        std::numeric_limits< T >::infinity()
    );

    template< concepts::Arithmetic T >
    constexpr const Point3< T > Point3< T >::NEGATIVE_INFINITY(
        -std::numeric_limits< T >::infinity(),
        -std::numeric_limits< T >::infinity(),
        -std::numeric_limits< T >::infinity()
    );

    using Point3f = Point3< Real32 >;
    using Point3d = Point3< Real64 >;
    using Point3i = Point3< Int32 >;
    using Point3ui = Point3< UInt32 >;

}

#endif
