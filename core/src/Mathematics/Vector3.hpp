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

#ifndef CRIMILD_MATHEMATICS_VECTOR_3_
#define CRIMILD_MATHEMATICS_VECTOR_3_

#include "Foundation/Types.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Utils.hpp"

#include <iomanip>
#include <sstream>

namespace crimild {

    template< typename T >
    struct Vector3Impl {
        T x = 0;
        T y = 0;
        T z = 0;

        [[nodiscard]] inline constexpr const T &operator[]( Size index ) const noexcept
        {
            if ( index == 0 )
                return x;
            if ( index == 1 )
                return y;
            return z;
        }
    };

    template< typename T >
    [[nodiscard]] inline constexpr Bool isNaN( const Vector3Impl< T > &u ) noexcept
    {
        return isNaN( u.x ) || isNaN( u.y ) || isNaN( u.z );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool operator==( const Vector3Impl< T > &u, const Vector3Impl< T > &v ) noexcept
    {
        return isEqual( u.x, v.x ) && isEqual( u.y, v.y ) && isEqual( u.z, v.z );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool operator!=( const Vector3Impl< T > &u, const Vector3Impl< T > &v ) noexcept
    {
        return !isEqual( u.x, v.x ) || !isEqual( u.y, v.y ) || !isEqual( u.z, v.z );
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector3Impl< T > operator+( const Vector3Impl< T > &u, const Vector3Impl< U > &v ) noexcept
    {
        return Vector3Impl< T > {
            .x = u.x + v.x,
            .y = u.y + v.y,
            .z = u.z + v.z,
        };
    }

    template< typename T, typename U >
    inline constexpr Vector3Impl< T > &operator+=( Vector3Impl< T > &u, const Vector3Impl< U > &v ) noexcept
    {
        u.x += v.x;
        u.y += v.y;
        u.z += v.z;
        return u;
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector3Impl< T > operator-( const Vector3Impl< T > &u, const Vector3Impl< U > &v ) noexcept
    {
        return Vector3Impl< T > {
            .x = u.x - v.x,
            .y = u.y - v.y,
            .z = u.z - v.z,
        };
    }

    template< typename T, typename U >
    inline constexpr Vector3Impl< T > &operator-=( Vector3Impl< T > &u, const Vector3Impl< U > &v ) noexcept
    {
        u.x -= v.x;
        u.y -= v.y;
        u.z -= v.z;
        return u;
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector3Impl< T > operator*( const Vector3Impl< T > &u, U scalar ) noexcept
    {
        return Vector3Impl< T > {
            .x = u.x * scalar,
            .y = u.y * scalar,
            .z = u.z * scalar,
        };
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector3Impl< T > operator*( U scalar, const Vector3Impl< T > &u ) noexcept
    {
        return Vector3Impl< T > {
            .x = u.x * scalar,
            .y = u.y * scalar,
            .z = u.z * scalar,
        };
    }

    template< typename T, typename U >
    inline constexpr Vector3Impl< T > &operator*=( Vector3Impl< T > &u, U scalar ) noexcept
    {
        u.x *= scalar;
        u.y *= scalar;
        u.z *= scalar;
        return u;
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector3Impl< T > operator*( const Vector3Impl< T > &u, const Vector3Impl< U > &v ) noexcept
    {
        return Vector3Impl< T > {
            .x = u.x * v.x,
            .y = u.y * v.y,
            .z = u.z * v.z,
        };
    }

    template< typename T, typename U >
    inline constexpr Vector3Impl< T > &operator*( Vector3Impl< T > &u, const Vector3Impl< U > &v ) noexcept
    {
        u.x *= v.x;
        u.y *= v.y;
        u.z *= v.z;
        return u;
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector3Impl< T > operator/( const Vector3Impl< T > &u, U scalar ) noexcept
    {
        return Vector3Impl< T > {
            .x = u.x / scalar,
            .y = u.y / scalar,
            .z = u.z / scalar,
        };
    }

    template< typename T, typename U >
    inline constexpr Vector3Impl< T > &operator/=( Vector3Impl< T > &u, U scalar ) noexcept
    {
        u.x /= scalar;
        u.y /= scalar;
        u.z /= scalar;
        return u;
    }

    template< typename T >
    [[nodiscard]] inline constexpr Vector3Impl< T > operator-( const Vector3Impl< T > &u ) noexcept
    {
        return Vector3Impl< T > {
            .x = -u.x,
            .y = -u.y,
            .z = -u.z,
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Vector3Impl< T > abs( const Vector3Impl< T > &u ) noexcept
    {
        return Vector3Impl< T > {
            .x = abs( u.x ),
            .y = abs( u.y ),
            .z = abs( u.z ),
        };
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr T dot( const Vector3Impl< T > &u, const Vector3Impl< U > &v ) noexcept
    {
        return u.x * v.x + u.y * v.y + u.z * v.z;
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr T absDot( const Vector3Impl< T > &u, const Vector3Impl< U > &v ) noexcept
    {
        return abs( dot( u, v ) );
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr Vector3Impl< T > cross( const Vector3Impl< T > &u, const Vector3Impl< U > &v ) noexcept
    {
        return Vector3Impl< T > {
            .x = u.y * v.z - u.z * v.y,
            .y = u.z * v.x - u.x * v.z,
            .z = u.x * v.y - u.y * v.x,
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Real lengthSquared( const Vector3Impl< T > &u ) noexcept
    {
        return dot( u, u );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Real length( const Vector3Impl< T > &u ) noexcept
    {
        return sqrt( lengthSquared( u ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Vector3Impl< T > normalize( const Vector3Impl< T > &u ) noexcept
    {
        return u / length( u );
    }

    template< typename T >
    [[nodiscard]] inline constexpr T min( const Vector3Impl< T > &u ) noexcept
    {
        return min( u.x, min( u.y, u.z ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Vector3Impl< T > min( const Vector3Impl< T > &u, const Vector3Impl< T > &v ) noexcept
    {
        return Vector3Impl< T > {
            .x = min( u.x, v.x ),
            .y = min( u.y, v.y ),
            .z = min( u.z, v.z ),
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr T minDimension( const Vector3Impl< T > &u ) noexcept
    {
        return ( u.x < u.y ? ( u.x < u.z ? 0 : 2 ) : ( u.y > u.z ? 1 : 2 ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr T max( const Vector3Impl< T > &u ) noexcept
    {
        return max( u.x, max( u.y, u.z ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Vector3Impl< T > max( const Vector3Impl< T > &u, const Vector3Impl< T > &v ) noexcept
    {
        return Vector3Impl< T > {
            .x = max( u.x, v.x ),
            .y = max( u.y, v.y ),
            .z = max( u.z, v.z ),
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr T maxDimension( const Vector3Impl< T > &u ) noexcept
    {
        return ( u.x > u.y ? ( u.x > u.z ? 0 : 2 ) : ( u.y > u.z ? 1 : 2 ) );
    }

    template< typename T >
    std::ostream &operator<<( std::ostream &out, const Vector3Impl< T > &u )
    {
        out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
            << std::setprecision( 6 );
        out << "("
            << u.x << ", "
            << u.y << ", "
            << u.z
            << ")";
        return out;
    }

    /**
       \brief Creates a orthonormal coordinate system from a vector

       \remarks The input vector v1 is assumed to be already normalized.
     */
    template< typename T >
    void orthonormalBasis( const Vector3Impl< T > &v1, Vector3Impl< T > &v2, Vector3Impl< T > &v3 ) noexcept
    {
        if ( abs( v1.x ) > abs( v1.y ) ) {
            v2 = normalize( Vector3Impl< T > { -v1.z, 0, v1.x } );
        } else {
            v2 = normalize( Vector3Impl< T > { 0, v1.z, -v1.y } );
        }
        v3 = cross( v1, v2 );
    }

    using Vector3 = Vector3Impl< Real >;
    using Vector3f = Vector3Impl< Real32 >;
    using Vector3d = Vector3Impl< Real64 >;
    using Vector3i = Vector3Impl< Int32 >;
    using Vector3ui = Vector3Impl< UInt32 >;

}

#endif
