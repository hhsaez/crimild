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

#ifndef CRIMILD_MATHEMATICS_VECTOR_2_
#define CRIMILD_MATHEMATICS_VECTOR_2_

#include "Foundation/Types.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Utils.hpp"

namespace crimild {

    template< typename T >
    struct Vector2Impl {
        T x = 0;
        T y = 0;

        [[nodiscard]] inline constexpr const T &operator[]( Size index ) const noexcept
        {
            return index == 0 ? x : y;
        }
    };

    template< typename T >
    [[nodiscard]] inline constexpr Bool isNaN( const Vector2Impl< T > &u ) noexcept
    {
        return isNaN( u.x ) || isNaN( u.y );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool operator==( const Vector2Impl< T > &u, const Vector2Impl< T > &v ) noexcept
    {
        return isEqual( u.x, v.x ) && isEqual( u.y, v.y );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool operator!=( const Vector2Impl< T > &u, const Vector2Impl< T > &v ) noexcept
    {
        return !isEqual( u.x, v.x ) || !isEqual( u.y, v.y );
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector2Impl< T > operator+( const Vector2Impl< T > &u, const Vector2Impl< U > &v ) noexcept
    {
        return Vector2Impl< T > {
            .x = u.x + v.x,
            .y = u.y + v.y,
        };
    }

    template< typename T, typename U >
    inline constexpr Vector2Impl< T > &operator+=( Vector2Impl< T > &u, const Vector2Impl< U > &v ) noexcept
    {
        u.x += v.x;
        u.y += v.y;
        return u;
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector2Impl< T > operator-( const Vector2Impl< T > &u, const Vector2Impl< U > &v ) noexcept
    {
        return Vector2Impl< T > {
            .x = u.x - v.x,
            .y = u.y - v.y,
        };
    }

    template< typename T, typename U >
    inline constexpr Vector2Impl< T > &operator-=( Vector2Impl< T > &u, const Vector2Impl< U > &v ) noexcept
    {
        u.x -= v.x;
        u.y -= v.y;
        return u;
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector2Impl< T > operator*( const Vector2Impl< T > &u, U scalar ) noexcept
    {
        return Vector2Impl< T > {
            .x = u.x * scalar,
            .y = u.y * scalar,
        };
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector2Impl< T > operator*( U scalar, const Vector2Impl< T > &u ) noexcept
    {
        return Vector2Impl< T > {
            .x = u.x * scalar,
            .y = u.y * scalar,
        };
    }

    template< typename T, typename U >
    inline constexpr Vector2Impl< T > &operator*=( Vector2Impl< T > &u, U scalar ) noexcept
    {
        u.x *= scalar;
        u.y *= scalar;
        return u;
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector2Impl< T > operator*( const Vector2Impl< T > &u, const Vector2Impl< U > &v ) noexcept
    {
        return Vector2Impl< T > {
            .x = u.x * v.x,
            .y = u.y * v.y,
        };
    }

    template< typename T, typename U >
    inline constexpr Vector2Impl< T > &operator*( Vector2Impl< T > &u, const Vector2Impl< U > &v ) noexcept
    {
        u.x *= v.x;
        u.y *= v.y;
        return u;
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector2Impl< T > operator/( const Vector2Impl< T > &u, U scalar ) noexcept
    {
        return Vector2Impl< T > {
            .x = u.x / scalar,
            .y = u.y / scalar,
        };
    }

    template< typename T, typename U >
    inline constexpr Vector2Impl< T > &operator/=( Vector2Impl< T > &u, U scalar ) noexcept
    {
        u.x /= scalar;
        u.y /= scalar;
        return u;
    }

    template< typename T >
    inline constexpr Vector2Impl< T > operator-( const Vector2Impl< T > &u ) noexcept
    {
        return Vector2Impl< T > {
            .x = -u.x,
            .y = -u.y,
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Vector2Impl< T > abs( const Vector2Impl< T > &u ) noexcept
    {
        return Vector2Impl< T > {
            .x = abs( u.x ),
            .y = abs( u.y ),
        };
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr T dot( const Vector2Impl< T > &u, const Vector2Impl< U > &v ) noexcept
    {
        return u.x * v.x + u.y * v.y;
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr T absDot( const Vector2Impl< T > &u, const Vector2Impl< U > &v ) noexcept
    {
        return abs( dot( u, v ) );
    }

    using Vector2 = Vector2Impl< Real >;
    using Vector2f = Vector2Impl< Real32 >;
    using Vector2d = Vector2Impl< Real64 >;
    using Vector2i = Vector2Impl< Int32 >;
    using Vector2ui = Vector2Impl< UInt32 >;

}

#endif
