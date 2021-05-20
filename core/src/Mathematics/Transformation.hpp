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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_

#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Normal3.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Ray3.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector4.hpp"

namespace crimild {

    class Transformation {
    public:
        constexpr Transformation( void ) noexcept
            : m_matrix( Matrix4::Constants::IDENTITY ),
              m_invMatrix( Matrix4::Constants::IDENTITY )
        {
        }

        constexpr Transformation( const Transformation &other ) noexcept
            : m_matrix( other.m_matrix ),
              m_invMatrix( other.m_invMatrix ),
              m_identity( other.m_identity )
        {
        }

        constexpr Transformation( const Transformation &&other ) noexcept
            : m_matrix( std::move( other.m_matrix ) ),
              m_invMatrix( std::move( other.m_invMatrix ) ),
              m_identity( other.m_identity )
        {
        }

        constexpr Transformation( const Matrix4 &m, const Matrix4 &inv ) noexcept
            : m_matrix( m ),
              m_invMatrix( inv ),
              m_identity( false )
        {
        }

        ~Transformation( void ) = default;

        inline constexpr Transformation &operator=( const Transformation &other ) noexcept
        {
            m_matrix = other.m_matrix;
            m_invMatrix = other.m_invMatrix;
            m_identity = other.m_identity;
            return *this;
        }

        inline constexpr Transformation &operator=( const Transformation &&other ) noexcept
        {
            m_matrix = std::move( other.m_matrix );
            m_invMatrix = std::move( other.m_invMatrix );
            m_identity = other.m_identity;
            return *this;
        }

        [[nodiscard]] inline constexpr Bool operator==( const Transformation &other ) const noexcept
        {
            // m_identity is an optimization flag and should not be included in
            // this comparison
            return m_matrix == other.m_matrix && m_invMatrix == other.m_invMatrix;
        }

        [[nodiscard]] inline constexpr Bool operator!=( const Transformation &other ) const noexcept
        {
            return m_matrix != other.m_matrix || m_invMatrix != other.m_invMatrix;
        }

        [[nodiscard]] inline constexpr const Matrix4 &getMatrix( void ) const noexcept { return m_matrix; }
        [[nodiscard]] inline constexpr const Matrix4 &getInverseMatrix( void ) const noexcept { return m_invMatrix; }

        friend constexpr Bool isIdentity( const Transformation &t ) noexcept;

        [[nodiscard]] constexpr Transformation operator*( const Transformation &other ) const noexcept
        {
            return Transformation {
                getMatrix() * other.getMatrix(),
                other.getInverseMatrix() * getInverseMatrix(),
            };
        }

        [[nodiscard]] constexpr Point3 operator()( const Point3 &p ) const noexcept
        {
            if ( isIdentity( *this ) ) {
                return p;
            }

            const auto V = m_matrix * p.xyzw();
            return Point3( V.xyz() );
        }

        [[nodiscard]] constexpr Vector3 operator()( const Vector3 &v ) const noexcept
        {
            if ( isIdentity( *this ) ) {
                return v;
            }

            const auto v0 = Vector4( v.x(), v.y(), v.z(), 0.0 );
            const auto v1 = m_matrix * v0;
            return Vector3 {
                v1.x(),
                v1.y(),
                v1.z(),
            };
        }

        [[nodiscard]] constexpr Normal3 operator()( const Normal3 &N ) const noexcept
        {
            if ( isIdentity( *this ) ) {
                return N;
            }

            const auto V = Vector4( N.x(), N.y(), N.z(), 0 );
            const auto R = transpose( m_invMatrix ) * V;
            return Normal3 {
                R.x(),
                R.y(),
                R.z(),
            };
        }

        [[nodiscard]] constexpr Ray3 operator()( const Ray3 &R ) const noexcept
        {
            if ( isIdentity( *this ) ) {
                return R;
            }

            return Ray3 {
                ( *this )( R.getOrigin() ),
                ( *this )( R.getDirection() ),
            };
        }

    private:
        Matrix4 m_matrix;
        Matrix4 m_invMatrix;
        Bool m_identity = true;
    };

    [[nodiscard]] constexpr Bool isIdentity( const Transformation &t ) noexcept { return t.m_identity; }

    [[nodiscard]] static constexpr Transformation inverse( const Transformation &t ) noexcept
    {
        return Transformation( t.getInverseMatrix(), t.getMatrix() );
    }

    [[nodiscard]] static constexpr Transformation translation( const Vector3 &delta ) noexcept
    {
        // clang-format off
        const auto m = Matrix4 {
            1, 0, 0, delta.x(),
            0, 1, 0, delta.y(),
            0, 0, 1, delta.z(),
            0, 0, 0, 1,
        };

        const auto inv = Matrix4 {
            1, 0, 0, -delta.x(),
            0, 1, 0, -delta.y(),
            0, 0, 1, -delta.z(),
            0, 0, 0, 1,
        };
        // clang-format on

        return Transformation( m, inv );
    }

    [[nodiscard]] static constexpr Transformation translation( Real x, Real y, Real z ) noexcept
    {
        return translation( Vector3 { x, y, z } );
    }

    [[nodiscard]] static constexpr Transformation scale( const Vector3 &scale ) noexcept
    {
        // clang-format off
        const auto m = Matrix4 {
            scale.x(), 0, 0, 0,
            0, scale.y(), 0, 0,
            0, 0, scale.z(), 0,
            0, 0, 0, 1,
        };

        const auto inv = Matrix4 {
            1 / scale.x() , 0, 0, 0,
            0, 1 / scale.y(), 0, 0,
            0, 0, 1 / scale.z(), 0,
            0, 0, 0, 1,
        };
        // clang-format on

        return Transformation( m, inv );
    }

    [[nodiscard]] static constexpr Transformation scale( Real x, Real y, Real z ) noexcept
    {
        return scale( Vector3 { x, y, z } );
    }

    [[nodiscard]] static constexpr Transformation scale( Real x ) noexcept
    {
        return scale( Vector3 { x, x, x } );
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotationX( Radians angle ) noexcept
    {
        // clang-format off
        const auto m = Matrix4 {
            1, 0, 0, 0,
            0, cos( angle ), -sin( angle ), 0,
            0, sin( angle ), cos( angle ), 0,
            0, 0, 0, 1
        };

        const auto inv = Matrix4 {
            1, 0, 0, 0,
            0, cos( -angle ), -sin( -angle ), 0,
            0, sin( -angle ), cos( -angle ), 0,
            0, 0, 0, 1
        };
        // clang-format on

        return Transformation( m, inv );
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotationY( Radians angle ) noexcept
    {
        // clang-format off
        const auto m = Matrix4 {
            cos( angle ), 0, sin( angle ), 0,
            0, 1, 0, 0,
            -sin( angle ), 0, cos( angle ), 0,
            0, 0, 0, 1
        };

        const auto inv = Matrix4 {
            cos( -angle ), 0, sin( -angle ), 0,
            0, 1, 0, 0,
            -sin( -angle ), 0, cos( -angle ), 0,
            0, 0, 0, 1
        };
        // clang-format on

        return Transformation( m, inv );
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotationZ( Radians angle ) noexcept
    {
        // clang-format off
        const auto m = Matrix4 {
            cos( angle ), -sin( angle ), 0, 0,
            sin( angle ), cos( angle ), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };

        const auto inv = Matrix4 {
            cos( -angle ), -sin( -angle ), 0, 0,
            sin( -angle ), cos( -angle ), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
        // clang-format on

        return Transformation( m, inv );
    }

    // TODO: make this function constexpr
    [[nodiscard]] static Transformation rotation( const Vector3 &axis, Radians angle ) noexcept
    {
        const auto x = axis.x();
        const auto y = axis.y();
        const auto z = axis.z();
        const auto xx = axis.x() * axis.x();
        const auto xy = axis.x() * axis.y();
        const auto xz = axis.x() * axis.z();
        const auto yy = axis.y() * axis.y();
        const auto yz = axis.y() * axis.z();
        const auto zz = axis.z() * axis.z();
        const auto cosTheta = cos( angle );
        const auto sinTheta = sin( angle );
        const auto oneMinusCosTheta = Real( 1 ) - cosTheta;
        const auto oneMinusSinTheta = Real( 1 ) - sinTheta;

        // clang-format off
        const auto m = Matrix4 {
            xx * oneMinusCosTheta + cosTheta,
            xy * oneMinusCosTheta - z * sinTheta,
            xz * oneMinusCosTheta + y * sinTheta,
            0,

            xy * oneMinusCosTheta + z * sinTheta,
            yy * oneMinusCosTheta + cosTheta,
            yz * oneMinusCosTheta - x * sinTheta,
            0,

            xz * oneMinusCosTheta - y * sinTheta,
            yz * oneMinusCosTheta + x * sinTheta,
            zz * oneMinusCosTheta + cosTheta,
            0,

            0,
            0,
            0,
            1,
        };

        // cos(-theta) == cos(theta)
        // sin(-theta) == -sin(theta)
        const auto inv = Matrix4 {
            xx * oneMinusCosTheta + cosTheta,
            xy * oneMinusCosTheta + z * sinTheta,
            xz * oneMinusCosTheta - y * sinTheta,
            0,

            xy * oneMinusCosTheta - z * sinTheta,
            yy * oneMinusCosTheta + cosTheta,
            yz * oneMinusCosTheta + x * sinTheta,
            0,

            xz * oneMinusCosTheta + y * sinTheta,
            yz * oneMinusCosTheta - x * sinTheta,
            zz * oneMinusCosTheta + cosTheta,
            0,

            0,
            0,
            0,
            1,
        };
        // clang-format on

        return Transformation( m, inv );
    }

    [[nodiscard]] static constexpr Transformation lookAt( const Point3 &pos, const Point3 &target, const Vector3 &up ) noexcept
    {
        const auto T = Vector3 { target.x(), target.y(), target.z() };
        const auto F = normalize( target - pos );
        const auto R = normalize( cross( normalize( up ), F ) );
        const auto U = cross( F, R );

        // clang-format off
        const auto cameraToWorld = Matrix4 {
            R.x(), R.y(), R.z(), -dot( T, R ),
            U.x(), U.y(), U.z(), -dot( T, U ),
            F.x(), F.y(), F.z(), -dot( T, F ),
            0, 0, 0, 1,
        };
        // clang-format on

        return Transformation { cameraToWorld, inverse( cameraToWorld ) };
    }

}

#endif
