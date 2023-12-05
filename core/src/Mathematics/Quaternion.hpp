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

#include "Mathematics/Matrix3.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector4.hpp"
#include "Mathematics/cross.hpp"
#include "Mathematics/dot.hpp"
#include "Mathematics/swizzle.hpp"
#include "Numeric.hpp"

#include <iomanip>
#include <iostream>

namespace crimild {

    namespace impl {

        /**
                 \brief Template-based implementation for Quaternions

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
        template< typename PRECISION >
        class [[deprecated]] Quaternion {
        public:
            /**
                        \brief Creates a new quaternion representing a rotation
                 */
            static Quaternion createFromAxisAngle( const Vector3< PRECISION > &axis, PRECISION angle )
            {
                Quaternion q;
                q.fromAxisAngle( axis, angle );
                return q;
            }

            /**
            \brief Computes the quaternion that rotates from a to b

            This method was taken from "The Shortest Arc Quaternion" by Stan Melax
            in "Game Programming Gems". It calculates a quaternion that
            rotates from a to b, avoiding numerical instability.
         */
            static Quaternion createFromVectors( const Vector3< PRECISION > &v0, const Vector3< PRECISION > &v1 )
            {
                if ( v0 == -v1 ) {
                    return Quaternion::createFromAxisAngle( Vector3< PRECISION > { 1, 0, 0 }, Numeric< PRECISION >::PI );
                }

                Vector3 c = v0 ^ v1;
                PRECISION d = v0 * v1;
                PRECISION s = std::sqrt( ( 1.0 + d ) * 2.0 );

                return Quaternion( s / 2.0, c[ 0 ] / s, c[ 1 ] / s, c[ 2 ] / s );
            }

            static Quaternion createFromDirection( const Vector3< PRECISION > &direction, const Vector3< PRECISION > &up = Vector3< PRECISION > { 0, 1, 0 } )
            {
                Quaternion q;
                q.lookAt( direction, up );
                return q;
            }

            static Quaternion createFromEulerAngles( const Vector3< PRECISION > &angles )
            {
                return createFromEulerAngles( angles[ 0 ], angles[ 1 ], angles[ 2 ] );
            }

            static Quaternion createFromEulerAngles( PRECISION pitch, PRECISION yaw, PRECISION roll )
            {
                Quaternion q;
                q.fromEulerAngles( pitch, yaw, roll );
                return q;
            }

        public:
            /**
                        \brief Default constructor

                        The default constructor does not nothing. The user is responsible for
                        setting valid values for all quaternion components
                 */
            Quaternion( void )
                : _data { 0, 0, 0, 1 }
            {
            }

            /**
                        \brief Explicit constructor

                        Constructs a quaternion by specifying both the real
                        and the imaginary parts of it as a scalar and a vector
                        respectively.
                 */
            Quaternion( PRECISION r, const Vector3< PRECISION > &i )
                : _data( i[ 0 ], i[ 1 ], i[ 2 ], r )
            {
            }

            /**
                        \brief Explicit constructor
                 */
            Quaternion( PRECISION x, PRECISION y, PRECISION z, PRECISION w )
                : _data { x, y, z, w }
            {
            }

            explicit Quaternion( const Vector4< PRECISION > &data )
                : _data( data )
            {
            }

            /**
                        \brief Copy constructor
                 */
            Quaternion( const Quaternion &q )
                : _data( q._data )
            {
            }

            /**
                        \brief Destructor
                 */
            ~Quaternion( void )
            {
            }

            const Vector4< PRECISION > &getRawData( void ) const
            {
                return _data;
            }

            /**
                        \brief Assignment operator
                 */
            Quaternion operator=( const Quaternion &q )
            {
                _data = q._data;
                return *this;
            }

            bool operator==( const Quaternion &q ) const
            {
                return false; //( _data == q._data );
            }

            bool operator!=( const Quaternion &q ) const
            {
                return false; //( _data != q._data );
            }

            Vector3< PRECISION > getImaginary( void ) const
            {
                // return xyz( _data );
                return { _data.x, _data.y, _data.z };
            }

            void setImaginary( const Vector3< PRECISION > &value )
            {
                _data[ 0 ] = value[ 0 ];
                _data[ 1 ] = value[ 1 ];
                _data[ 2 ] = value[ 2 ];
            }

            PRECISION getReal( void ) const
            {
                return _data[ 3 ];
            }

            void setReal( PRECISION value )
            {
                _data[ 3 ] = value;
            }

            template< typename U >
            friend Quaternion< U > operator-( const Quaternion< U > &q );

            template< typename U >
            friend Quaternion< U > operator+( const Quaternion< U > &q, const Quaternion< U > &r );

            template< typename U >
            friend Quaternion< U > operator-( const Quaternion< U > &q, const Quaternion< U > &r );

            template< typename U >
            friend Quaternion< U > operator*( const Quaternion< U > &q, const Quaternion< U > &r );

            template< typename U >
            friend Vector3< U > operator*( const Quaternion< U > &q, const Vector3< U > &v );

            template< typename U >
            friend Vector3< U > operator*( const Vector3< U > &v, const Quaternion< U > &q );

            template< typename U, typename V >
            friend Quaternion< U > operator*( const Quaternion< U > &q, V s );

            template< typename U, typename V >
            friend Quaternion< U > operator*( V s, const Quaternion< U > &q );

            template< typename U, typename V >
            friend Quaternion< U > operator/( const Quaternion< U > &q, V s );

            template< typename U >
            friend Quaternion< U > &operator+=( Quaternion< U > &q, const Quaternion< U > &r );

            template< typename U >
            friend Quaternion< U > &operator-=( Quaternion< U > &q, const Quaternion< U > &r );

            template< typename U >
            friend Quaternion< U > &operator*=( Quaternion< U > &q, const Quaternion< U > &r );

            template< typename U, typename V >
            friend Quaternion< U > &operator*=( Quaternion< U > &q, V s );

            template< typename U, typename V >
            friend Quaternion< U > &operator/=( Quaternion< U > &q, V s );

            template< typename U >
            friend std::ostream &operator<<( std::ostream &output, const Quaternion &input );

            /**
                        \breif Calculates the conjugate for this quaternion

                        The conjugate is calculated as:
                        \code
                        q* = (qv, qw)* = (-qv, qw)
                        \endcode
                 */
            Quaternion getConjugate( void ) const
            {
                return *this; // Quaternion( -_data[ 0 ], -_data[ 1 ], -_data[ 2 ], _data[ 3 ] );
            }

            Quaternion &makeIdentity( void )
            {
                _data = Vector4< PRECISION > { 0, 0, 0, 1 };
                return *this;
            }

            double getSquaredNorm( void ) const
            {
                return dot( _data, _data );
            }

            double getNorm( void ) const
            {
                return std::sqrt( dot( _data, _data ) );
            }

            Quaternion getInverse( void ) const
            {
                double squaredN = getSquaredNorm();
                Quaternion conjugate = getConjugate();
                if ( Numeric< PRECISION >::equals( squaredN, 1 ) ) {
                    // avoid a division if the norm is 1
                    return conjugate;
                }

                return conjugate / squaredN;
            }

            Quaternion &normalize( void )
            {
                double n = getNorm();
                if ( n == 0 ) {
                    return *this;
                }
                *this = *this / n;
                return *this;
            }

            Quaternion &fromAxisAngle( const Vector3< PRECISION > &axis, PRECISION angle )
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

            Vector3< PRECISION > toEulerAngles( void ) const
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

                return Vector3( ( PRECISION ) pitch, ( PRECISION ) yaw, ( PRECISION ) roll );
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

            Quaternion &lookAt( const Vector3< PRECISION > &direction, const Vector3< PRECISION > &up = Vector3< PRECISION > { 0, 1, 0 } )
            {
                Vector3< PRECISION > forward( 0, 0, -1 );
                Vector3< PRECISION > right( 1, 0, 0 );

                Vector3< PRECISION > axis = up;

                Vector3< PRECISION > u = forward ^ direction;
                if ( Numeric< PRECISION >::isZero( u.getSquaredMagnitude() ) ) {
                    u = right ^ direction;
                }
                u.normalize();

                Vector3< PRECISION > v = u ^ up;
                if ( Numeric< PRECISION >::isZero( v.getSquaredMagnitude() ) ) {
                    axis = u;
                } else {
                    v.normalize();
                    u = v ^ u;

                    // Oh, Dark Lork, I summon thee!!!
                    axis = Vector3< PRECISION > { -u[ 1 ], u[ 0 ], u[ 2 ] };
                }

                float angle = std::acos( forward * direction );

                return fromAxisAngle( axis, angle );
            }

        private:
            Vector4< PRECISION > _data;
        };

        template< typename U >
        Quaternion< U > operator-( const Quaternion< U > &q )
        {
            return q; // Quaternion< U >( -q._data );
        }

        template< typename U >
        Quaternion< U > operator+( const Quaternion< U > &q, const Quaternion< U > &r )
        {
            return q; // Quaternion< U >( q._data + r._data );
        }

        template< typename U >
        Quaternion< U > operator-( const Quaternion< U > &q, const Quaternion< U > &r )
        {
            return Quaternion< U >( q._data - r._data );
        }

        template< typename U >
        Quaternion< U > operator*( const Quaternion< U > &q, const Quaternion< U > &r )
        {
            return q;
            /*
            impl::Vector3< U > qImaginary = q.getImaginary();
            U qReal = q.getReal();
            impl::Vector3< U > rImaginary = r.getImaginary();
            U rReal = r.getReal();

            // TODO: this should be replaced by a faster method
            return Quaternion< U >( qReal * rReal - dot( qImaginary, rImaginary ),
                                    ( qReal * rImaginary ) + ( qImaginary * rReal ) + ( crimild::cross( qImaginary, rImaginary ) ) );
            */
        }

        template< typename U >
        Vector3< U > operator*( const Quaternion< U > &q, const Vector3< U > &v )
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

            Vector3< U > result = {
                ix * qw + iw * -qx + iy * -qz - iz * -qy,
                iy * qw + iw * -qy + iz * -qx - ix * -qz,
                iz * qw + iw * -qz + ix * -qy - iy * -qx,
            };
            return result;
        }

        template< typename U >
        Vector3< U > operator*( const Vector3< U > &v, const Quaternion< U > &q )
        {
            return q * v;
        }

        template< typename U, typename V >
        Quaternion< U > operator*( const Quaternion< U > &q, V s )
        {
            return q; // Quaternion< U >( q._data * s );
        }

        template< typename U, typename V >
        Quaternion< U > operator*( V s, const Quaternion< U > &q )
        {
            return Quaternion< U >( q._data * s );
        }

        template< typename U, typename V >
        Quaternion< U > operator/( const Quaternion< U > &q, V s )
        {
            return q;
            /*
            if ( s == 0 ) {
                s = Numeric< U >::ZERO_TOLERANCE;
            }

            return Quaternion< U >( q._data / s );
            */
        }

        template< typename U >
        Quaternion< U > &operator+=( Quaternion< U > &q, const Quaternion< U > &r )
        {
            q = q + r;
            return q;
        }

        template< typename U >
        Quaternion< U > &operator-=( Quaternion< U > &q, const Quaternion< U > &r )
        {
            q = q - r;
            return q;
        }

        template< typename U >
        Quaternion< U > &operator*=( Quaternion< U > &q, const Quaternion< U > &r )
        {
            q = q * r;
            return q;
        }

        template< typename U, typename V >
        Quaternion< U > &operator*=( Quaternion< U > &q, V s )
        {
            q = q * s;
            return q;
        }

        template< typename U, typename V >
        Quaternion< U > *operator/=( Quaternion< U > &q, V s )
        {
            q = q / s;
            return q;
        }

        template< typename U >
        std::ostream &operator<<( std::ostream &out, const Quaternion< U > &q )
        {
            /*
            out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
                << std::setprecision( 10 )
                << "[r = " << q.getReal() << ", i = " << q.getImaginary() << "]";
            */
            return out;
        }

    }

    using Quaternion = impl::Quaternion< Real >;

}

#endif
