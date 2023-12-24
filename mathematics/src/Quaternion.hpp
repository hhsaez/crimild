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

#include <iomanip>
#include <iostream>

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

}

#endif
