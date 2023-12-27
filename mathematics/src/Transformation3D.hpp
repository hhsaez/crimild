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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_3D_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_3D_

#include "Matrix4.hpp"
#include "Normal3.hpp"
#include "Point3.hpp"
#include "Quaternion.hpp"
#include "Ray3.hpp"
#include "Vector3.hpp"
#include "normalize.hpp"

namespace crimild {

    struct EulerAngles {
        real_t yaw;
        real_t pitch;
        real_t roll;
    };

    /**
     * @brief A function from points to points and vectors to vectors
     *
     * There are many ways to represent transformations.
     * In the end, I chose the SRT (actually SQT) one because
     * it's more suited for interpolations and compositions.
     */
    struct Transformation3D {
        Point3 origin = Point3::Constants::ZERO;
        Quaternion orientation = Quaternion::Constants::IDENTITY;
        Vector3 scale = Vector3::Constants::ONE;

        constexpr Transformation3D( void ) = default;

        constexpr explicit Transformation3D( const Point3 &origin ) noexcept
            : origin( origin )
        {
        }

        constexpr explicit Transformation3D( radians_t yaw, radians_t pitch, radians_t roll ) noexcept
        {
            // TODO
        }

        constexpr explicit Transformation3D( const EulerAngles &angles ) noexcept
        {
            // TODO
        }

        template< ArithmeticType T >
        constexpr explicit Transformation3D( const Matrix4Impl< T > &M ) noexcept
            : origin( M[ 3 ] ),
              orientation(
                  // Force normalization
                  normalize( Quaternion( M ) )
              ),
              scale(
                  // TODO: there has to be a better way...
                  length( M * Vector4::Constants::UNIT_X ),
                  length( M * Vector4::Constants::UNIT_Y ),
                  length( M * Vector4::Constants::UNIT_Z )
              )
        {
        }

        constexpr Transformation3D( const Transformation3D &other ) noexcept
            : origin( other.origin ),
              orientation( other.orientation ),
              scale( other.scale )
        {
        }

        ~Transformation3D( void ) = default;

        [[nodiscard]] inline constexpr bool operator==( const Transformation3D &other ) const noexcept
        {
            return origin == other.origin && orientation == other.orientation && scale == other.scale;
        }

        [[nodiscard]] inline constexpr bool operator!=( const Transformation3D &other ) const noexcept
        {
            return !( *this == other );
        }

        [[nodiscard]] constexpr Point3f operator()( const Point3f &p ) const noexcept;
        [[nodiscard]] constexpr Vector3 operator()( const Vector3 &v ) const noexcept;
        [[nodiscard]] constexpr Normal3 operator()( const Normal3 &N ) const noexcept;
        [[nodiscard]] constexpr Ray3 operator()( const Ray3 &R ) const noexcept;

        /**
         * Transformation composition in the TRS form (for column vectors)
         */
        [[nodiscard]] constexpr Transformation3D operator()( const Transformation3D &other ) const noexcept
        {
            return Transformation3D {
                .origin = origin + scale * ( orientation * other.origin ),
                .orientation = other.orientation * orientation,
                .scale = scale * other.scale,
            };
        }
    };

    template<>
    constexpr Matrix4Impl< real_t >::Matrix4Impl( const Transformation &t ) noexcept
    {
        // Compute matrix from transformation
    }

    /**
     * @brief Composition of transformation
     *
     * Computes a new transformation in the form Tc = Ta * Tb;
     */
    constexpr Transformation3D operator*( const Transformation3D &t0, const Transformation3D &t1 ) noexcept
    {
        return Transformation3D {
            .origin = t0.origin + t0.scale * ( t0.orientation * t1.origin ),
            .orientation = t0.orientation * t1.orientation,
            .scale = t0.scale * t1.scale,
        };
    }

    /**
    \brief Computes the quaternion that rotates from a to b

    This method was taken from "The Shortest Arc Quaternion" by Stan Melax
    in "Game Programming Gems". It calculates a quaternion that
    rotates from a to b, avoiding numerical instability.
 */
    static Quaternion createFromVectors( const Vector3Impl< PRECISION > &v0, const Vector3Impl< PRECISION > &v1 )
    {
        if ( v0 == -v1 ) {
            return Quaternion::createFromAxisAngle( Vector3Impl< PRECISION > { 1, 0, 0 }, Numeric< PRECISION >::PI );
        }

        Vector3Impl< PRECISION > c = v0 ^ v1;
        PRECISION d = v0 * v1;
        PRECISION s = std::sqrt( ( 1.0 + d ) * 2.0 );

        return Quaternion( s / 2.0, c[ 0 ] / s, c[ 1 ] / s, c[ 2 ] / s );
    }

    static Quaternion createFromDirection( const Vector3Impl< PRECISION > &direction, const Vector3Impl< PRECISION > &up = Vector3Impl< PRECISION > { 0, 1, 0 } )
    {
        Quaternion q;
        q.lookAt( direction, up );
        return q;
    }

    static Quaternion createFromEulerAngles( const Vector3Impl< PRECISION > &angles )
    {
        return createFromEulerAngles( angles[ 0 ], angles[ 1 ], angles[ 2 ] );
    }

    static Quaternion createFromEulerAngles( PRECISION pitch, PRECISION yaw, PRECISION roll )
    {
        Quaternion q;
        q.fromEulerAngles( pitch, yaw, roll );
        return q;
    }

    Quaternion &fromAxisAngle( const Vector3Impl< PRECISION > &axis, PRECISION angle )
    {
        /*
        double sinTheta = std::sin( 0.5 * angle );
        double cosTheta = std::cos( 0.5 * angle );

        _data[ 0 ] = axis[ 0 ] * sinTheta;
        _data[ 1 ] = axis[ 1 ] * sinTheta;
        _data[ 2 ] = axis[ 2 ] * sinTheta;
        _data[ 3 ] = cosTheta;
        */

        return *this;
    }

    Quaternion &fromEulerAngles( PRECISION pitch, PRECISION yaw, PRECISION roll )
    {
        /*
        // from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

        Quaternion q;
        double t0 = std::cos( roll * 0.5 );
        double t1 = std::sin( roll * 0.5 );
        double t2 = std::cos( pitch * 0.5 );
        double t3 = std::sin( pitch * 0.5 );
        double t4 = std::cos( yaw * 0.5 );
        double t5 = std::sin( yaw * 0.5 );

        auto w = t0 * t2 * t4 + t1 * t3 * t5;
        auto x = t0 * t3 * t4 - t1 * t2 * t5;
        auto y = t0 * t2 * t5 + t1 * t3 * t4;
        auto z = t1 * t2 * t4 - t0 * t3 * t5;

        _data[ 0 ] = ( PRECISION ) x;
        _data[ 1 ] = ( PRECISION ) y;
        _data[ 2 ] = ( PRECISION ) z;
        _data[ 3 ] = ( PRECISION ) w;
        */

        return *this;
    }

    Vector3Impl< PRECISION > toEulerAngles( void ) const
    {
        // from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

        double x = getImaginary().z();
        double y = getImaginary().x();
        double z = getImaginary().y();
        double w = getReal();

        double ysqr = y * y;

        // roll (x-axis rotation)
        double t0 = +2.0f * ( w * x + y * z );
        double t1 = +1.0f - 2.0f * ( x * x + ysqr );
        double roll = std::atan2( t0, t1 );

        // pitch (y-axis rotation)
        double t2 = +2.0f * ( w * y - z * x );
        t2 = t2 > 1.0f ? 1.0f : t2;
        t2 = t2 < -1.0f ? -1.0f : t2;
        double pitch = std::asin( t2 );

        // yaw (z-axis rotation)
        double t3 = +2.0f * ( w * z + x * y );
        double t4 = +1.0f - 2.0f * ( ysqr + z * z );
        double yaw = std::atan2( t3, t4 );

        return Vector3Impl< PRECISION >( ( PRECISION ) pitch, ( PRECISION ) yaw, ( PRECISION ) roll );
    }

    /**
                \brief Computes the rotation from the compositions of two quaternions
         */
    Quaternion rotate( Quaternion q1 ) const
    {
        PRECISION w0 = getReal();
        PRECISION x0 = getImaginary()[ 0 ];
        PRECISION y0 = getImaginary()[ 1 ];
        PRECISION z0 = getImaginary()[ 2 ];

        PRECISION w1 = q1.getReal();
        PRECISION x1 = q1.getImaginary()[ 0 ];
        PRECISION y1 = q1.getImaginary()[ 1 ];
        PRECISION z1 = q1.getImaginary()[ 2 ];

        Quaternion q( w0 * w1 - x0 * x1 - y0 * y1 - z0 * z1, w0 * x1 + x0 * w1 + y0 * z1 - z0 * y1, w0 * y1 + y0 * w1 + z0 * x1 - x0 * z1, w0 * z1 + z0 * w1 + x0 * y1 - y0 * x1 );
        return q.normalize();
    }

    void getRotationMatrix( Matrix3Impl< PRECISION > &output )
    {
        float x = _data[ 0 ];
        float y = _data[ 1 ];
        float z = _data[ 2 ];
        float w = _data[ 3 ];

        float twoXX = 2 * x * x;
        float twoXY = 2 * x * y;
        float twoXZ = 2 * x * z;
        float twoXW = 2 * x * w;
        float twoYY = 2 * y * y;
        float twoYZ = 2 * y * z;
        float twoYW = 2 * y * w;
        float twoZZ = 2 * z * z;
        float twoZW = 2 * z * w;

        output[ 0 ] = 1 - twoYY - twoZZ;
        output[ 1 ] = twoXY - twoZW;
        output[ 2 ] = twoXZ + twoYW;
        output[ 3 ] = twoXY + twoZW;
        output[ 4 ] = 1 - twoXX - twoZZ;
        output[ 5 ] = twoYZ - twoXW;
        output[ 6 ] = twoXZ - twoYW;
        output[ 7 ] = twoYZ + twoXW;
        output[ 8 ] = 1 - twoXX - twoYY;
    }

    /**
    \brief Computes a quaterion from a rotation matrix

    Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    article "Quaternion Calculus and Fast Animation".
 */
    Quaternion &fromRotationMatrix( const Matrix3Impl< PRECISION > &m )
    {
        float trace = 1.0f + m[ 0 ] + m[ 4 ] + m[ 8 ];
        float x, y, z, w;

        if ( trace > 0.0 ) {
            float root = 2.0f * std::sqrt( trace );
            float invRoot = 1.0f / root;
            x = ( m[ 5 ] - m[ 7 ] ) * invRoot;
            y = ( m[ 6 ] - m[ 2 ] ) * invRoot;
            z = ( m[ 1 ] - m[ 3 ] ) * invRoot;
            w = 0.25f * root;
        } else if ( m[ 0 ] > m[ 4 ] && m[ 0 ] > m[ 8 ] ) {
            float root = 2.0f * std::sqrt( 1.0f + m[ 0 ] - m[ 4 ] - m[ 8 ] );
            float invRoot = 1.0f / root;
            x = 0.25f * root;
            y = ( m[ 1 ] + m[ 3 ] ) * invRoot;
            z = ( m[ 6 ] + m[ 2 ] ) * invRoot;
            w = ( m[ 5 ] + m[ 7 ] ) * invRoot;
        } else if ( m[ 4 ] > m[ 8 ] ) {
            float root = 2.0f * std::sqrt( 1.0f + m[ 4 ] - m[ 0 ] - m[ 8 ] );
            float invRoot = 1.0f / root;
            x = ( m[ 1 ] + m[ 3 ] ) * invRoot;
            y = 0.25f * root;
            z = ( m[ 5 ] + m[ 7 ] ) * invRoot;
            w = ( m[ 6 ] + m[ 2 ] ) * invRoot;
        } else {
            float root = 2.0f * std::sqrt( 1.0f + m[ 8 ] - m[ 0 ] - m[ 4 ] );
            float invRoot = 1.0f / root;
            x = ( m[ 6 ] + m[ 2 ] ) * invRoot;
            y = ( m[ 5 ] + m[ 7 ] ) * invRoot;
            z = 0.25f * root;
            w = ( m[ 1 ] + m[ 3 ] ) * invRoot;
        }

        _data[ 0 ] = x;
        _data[ 1 ] = y;
        _data[ 2 ] = z;
        _data[ 3 ] = w;

        return *this;
    }

    Quaternion &lookAt( const Vector3Impl< PRECISION > &direction, const Vector3Impl< PRECISION > &up = Vector3Impl< PRECISION > { 0, 1, 0 } )
    {
        Vector3Impl< PRECISION > forward( 0, 0, -1 );
        Vector3Impl< PRECISION > right( 1, 0, 0 );

        Vector3Impl< PRECISION > axis = up;

        Vector3Impl< PRECISION > u = forward ^ direction;
        if ( Numeric< PRECISION >::isZero( u.getSquaredMagnitude() ) ) {
            u = right ^ direction;
        }
        u.normalize();

        Vector3Impl< PRECISION > v = u ^ up;
        if ( Numeric< PRECISION >::isZero( v.getSquaredMagnitude() ) ) {
            axis = u;
        } else {
            v.normalize();
            u = v ^ u;

            // Oh, Dark Lork, I summon thee!!!
            axis = Vector3Impl< PRECISION > { -u[ 1 ], u[ 0 ], u[ 2 ] };
        }

        float angle = std::acos( forward * direction );

        return fromAxisAngle( axis, angle );
    }

    template< typename U >
    Vector3Impl< U > operator*( const Quaternion< U > &q, const Vector3Impl< U > &v )
    {
        U x = v[ 0 ];
        U y = v[ 1 ];
        U z = v[ 2 ];
        U qx = q.getImaginary()[ 0 ];
        U qy = q.getImaginary()[ 1 ];
        U qz = q.getImaginary()[ 2 ];
        U qw = q.getReal();

        U ix = qw * x + qy * z - qz * y;
        U iy = qw * y + qz * x - qx * z;
        U iz = qw * z + qx * y - qy * x;
        U iw = -qx * x - qy * y - qz * z;

        Vector3Impl< U > result = {
            ix * qw + iw * -qx + iy * -qz - iz * -qy,
            iy * qw + iw * -qy + iz * -qx - ix * -qz,
            iz * qw + iw * -qz + ix * -qy - iy * -qx,
        };
        return result;
    }

}

#endif
