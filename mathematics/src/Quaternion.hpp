/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_CORE_MATHEMATICS_QUATERNION_
#define CRIMILD_CORE_MATHEMATICS_QUATERNION_

#include "Matrix3.hpp"
#include "Matrix4.hpp"
#include "Numeric.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "sqrt.hpp"
#include "swizzle.hpp"
#include "trace.hpp"

namespace crimild {

    /**
       @brief Template-based implementation for Quaternions

       A Quaternion, as invented by Sir William Rowan Hamilton, is an
       extension to complex numbers.

       In mathematics, a quaternion q can be defined in the following ways, all equivalent:

       \code
       q = (qv, qw) = iqx + jqy + kqz + qw = qv + qw,
       qv = iqx + jqy + kqz = (qx, qy, qz),
       i2 = j2 = k2 = -1, jk = -kj = i, ki = -ik = j, ij = -ji = k
       \endcode

       The variable qw is called the real part of a quaternion. The imaginary part
       is qv, and i, j and k are called imaginary units

       For convention, a quaternion is written as [x y z w], which is not
       the same as the convention found in academic papers (which puts the
       `w` component first. This is so to be consistent with the represetation
       of homogeneous coordiantes for points and vectors.

       \section REFERENCES References

       - "Real-Time Rendering", 3rd Edition
       - "Mathematics for 3D Game Programming and Computer Graphics", 2nd Edition
       - Wikipedia (http://en.wikipedia.org/wiki/Quaternion)
    */
    struct Quaternion {
        Vector3 v = Vector3::Constants::ZERO;
        real_t w = real_t( 1 );

        constexpr Quaternion( void ) = default;

        constexpr explicit Quaternion( const Vector3 &v, real_t w ) noexcept
            : v( v ), w( w ) { }

        constexpr Quaternion( const Quaternion &other ) noexcept
            : v( other.v ), w( other.w ) { }

        constexpr Quaternion( real_t x, real_t y, real_t z, real_t w ) noexcept
            : v( x, y, z ), w( w ) { }

        template< ArithmeticType U >
        constexpr explicit Quaternion( const Point3Impl< U > &p ) noexcept
            : v( p ), w( 1 ) { }

        template< typename U >
        constexpr explicit Quaternion( const Vector4Impl< U > &v ) noexcept
            : v( v.x, v.y, v.z ), w( v.w ) { }

        /**
         * @brief Creates a quaternion from an orthogonal 4x4 matrix
         *
         * This algorithm is based on the one provided  by thee book
         * "Real-time Rendering", 3rd Edition.
         */
        template< ArithmeticType U >
        constexpr explicit Quaternion( const Matrix4Impl< U > &M ) noexcept
        {
            // Compute some key values that will determine which
            // of the component of the quaternion will be the
            // largest one.
            const real_t a = M[ 0 ][ 0 ] - M[ 1 ][ 1 ] - M[ 2 ][ 2 ]; // 4x^2 - 1
            const real_t b = M[ 1 ][ 1 ] - M[ 0 ][ 0 ] - M[ 2 ][ 2 ]; // 4y^2 - 1
            const real_t c = M[ 2 ][ 2 ] - M[ 0 ][ 0 ] - M[ 1 ][ 1 ]; // 4z^2 - 1
            const real_t d = M[ 0 ][ 0 ] + M[ 1 ][ 1 ] + M[ 2 ][ 2 ]; // 4w^2 - 1

            int biggestIndex = 3;
            real_t e = d; // Assumes w is highest one
            if ( a > e ) {
                e = a;
                biggestIndex = 0;
            }
            if ( b > e ) {
                e = b;
                biggestIndex = 1;
            }
            if ( c > e ) {
                e = c;
                biggestIndex = 2;
            }

            real_t s = sqrt( e + real_t( 1 ) ) * real_t( 0.5 );
            real_t t = real_t( 0.25 ) / s;

            if ( biggestIndex == 0 ) {
                // x is biggest
                v.x = s;
                v.y = ( M[ 0 ][ 1 ] + M[ 1 ][ 0 ] ) * t;
                v.z = ( M[ 2 ][ 0 ] + M[ 0 ][ 2 ] ) * t;
                w = ( M[ 1 ][ 2 ] - M[ 2 ][ 1 ] ) * t;
            } else if ( biggestIndex == 1 ) {
                // y is biggest
                v.x = ( M[ 0 ][ 1 ] + M[ 1 ][ 0 ] ) * t;
                v.y = s;
                v.z = ( M[ 1 ][ 2 ] + M[ 2 ][ 1 ] ) * t;
                w = ( M[ 2 ][ 0 ] - M[ 0 ][ 2 ] ) * t;
            } else if ( biggestIndex == 2 ) {
                // z is biggest
                v.x = ( M[ 2 ][ 0 ] + M[ 0 ][ 2 ] ) * t;
                v.y = ( M[ 1 ][ 2 ] + M[ 2 ][ 1 ] ) * t;
                v.z = s;
                w = ( M[ 0 ][ 1 ] - M[ 1 ][ 0 ] ) * t;
            } else {
                // w is biggest
                v.x = ( M[ 1 ][ 2 ] - M[ 2 ][ 1 ] ) * t;
                v.y = ( M[ 2 ][ 0 ] - M[ 0 ][ 2 ] ) * t;
                v.z = ( M[ 0 ][ 1 ] - M[ 1 ][ 0 ] ) * t;
                w = s;
            }
        }

        ~Quaternion( void ) = default;

        constexpr Quaternion &operator=( const Quaternion &q ) noexcept
        {
            v = q.v;
            w = q.w;
            return *this;
        }

        constexpr inline bool operator==( const Quaternion &q ) const noexcept
        {
            return v == q.v && w == q.w;
        }

        constexpr inline bool operator!=( const Quaternion &q ) const noexcept
        {
            return !( *this == q );
        }

        constexpr inline real_t &operator[]( size_t index ) noexcept
        {
            return index == 3 ? w : v[ index ];
        }

        constexpr inline real_t operator[]( size_t index ) const noexcept
        {
            return index == 3 ? w : v[ index ];
        }

        /**
         * @name Arithmetic Operations
         */
        //@{

        [[nodiscard]] inline constexpr Quaternion operator-( void ) const noexcept
        {
            return Quaternion( -v, -w );
        }

        [[nodiscard]] inline constexpr Quaternion operator+( const Quaternion &other ) const noexcept
        {
            return Quaternion( v + other.v, w + other.w );
        }

        inline constexpr Quaternion &operator+=( const Quaternion &other ) noexcept
        {
            v += other.v;
            w += other.w;
            return *this;
        }

        [[nodiscard]] inline constexpr Quaternion operator*( const Quaternion &r ) const noexcept
        {
            // Quaternion( cross( v, r.v ) + r.w * v + q.w * r.v, q.w * r.w - dot( q.v, r.v ) );
            const auto &q = *this;
            return Quaternion(
                q.v.y * r.v.z - q.v.z * r.v.y + r.w * q.v.x + q.w * r.v.x,
                q.v.z * r.v.x - q.v.x * r.v.z + r.w * q.v.y + q.w * r.v.y,
                q.v.x * r.v.y - q.v.y * r.v.x + r.w * q.v.z + q.w * r.v.z,
                q.w * r.w - q.v.x * r.v.x - q.v.y * r.v.y - q.v.z * r.v.z
            );
        }

        inline constexpr Quaternion &operator*=( const Quaternion &r ) noexcept
        {
            const auto &q = *this;
            v = Vector3 {
                q.v.y * r.v.z - q.v.z * r.v.y + r.w * q.v.x + q.w * r.v.x,
                q.v.z * r.v.x - q.v.x * r.v.z + r.w * q.v.y + q.w * r.v.y,
                q.v.x * r.v.y - q.v.y * r.v.x + r.w * q.v.z + q.w * r.v.z,
            };
            w = q.w * r.w - q.v.x * r.v.x - q.v.y * r.v.y - q.v.z * r.v.z;
            return *this;
        }

        [[nodiscard]] inline constexpr Quaternion operator*( real_t scalar ) const noexcept
        {
            return Quaternion {
                v * scalar,
                w * scalar,
            };
        }

        [[nodiscard]] friend inline constexpr Quaternion operator*( real_t scalar, const Quaternion &q ) noexcept
        {
            return Quaternion {
                q.v * scalar,
                q.w * scalar,
            };
        }

        inline constexpr Quaternion &operator*=( real_t scalar ) noexcept
        {
            v *= scalar;
            w *= scalar;
            return *this;
        }

        [[nodiscard]] inline constexpr Quaternion operator/( real_t scalar ) const noexcept
        {
            return Quaternion {
                v / scalar,
                w / scalar,
            };
        }

        inline constexpr Quaternion &operator/=( real_t scalar ) noexcept
        {
            v /= scalar;
            w /= scalar;
            return *this;
        }

        //@{
    };

    /**
     * @brief Creates a 4x4 Matrix from a unit Quaternion
     *
     * @remarks This method assumes the argument is a unit quaternion
     */
    template<>
    constexpr Matrix4Impl< real_t >::Matrix4Impl( const Quaternion &q ) noexcept
    {
        const real_t qxx = q.v.x * q.v.x;
        const real_t qxy = q.v.x * q.v.y;
        const real_t qxz = q.v.x * q.v.z;
        const real_t qyy = q.v.y * q.v.y;
        const real_t qyz = q.v.y * q.v.z;
        const real_t qzz = q.v.z * q.v.z;
        const real_t qwx = q.v.x * q.w;
        const real_t qwy = q.v.y * q.w;
        const real_t qwz = q.v.z * q.w;

        columns[ 0 ] = {
            1 - 2 * ( qyy + qzz ),
            2 * ( qxy + qwz ),
            2 * ( qxz - qwy ),
            0,
        };
        columns[ 1 ] = {
            2 * ( qxy - qwz ),
            1 - 2 * ( qxx + qzz ),
            2 * ( qyz + qwx ),
            0,
        };
        columns[ 2 ] = {
            2 * ( qxz + qwy ),
            2 * ( qyz - qwx ),
            1 - 2 * ( qxx + qyy ),
            0,
        };
        columns[ 3 ] = {
            0,
            0,
            0,
            1,
        };
    }

}

#endif
