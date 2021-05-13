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

//#include "Mathematics/Transformation.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Normal3.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Ray3.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector4.hpp"

#include "gtest/gtest.h"

/*

bool orthonormalize( const Vector3f &u, const Vector3f &v, Vector3f &i, Vector3f &j, Vector3f &k )
{
	Vector3f temp = u.getNormalized() ^ v.getNormalized();

	i = u;
	k = temp;
	j = temp ^ u;

	return true;
}

TEST ( Transformation, testTranslate )
{
	Transformation t;
	t.setTranslate( Vector3f( 1.0f, 2.0f, 3.0f ) );

	EXPECT_EQ( Vector3f( 1.0f, 2.0f, 3.0f ), t.getTranslate() );
}

TEST( Transformation, testLookAt )
{
	Transformation t;
	t.setTranslate( Vector3f( 1.0f, 2.0f, 3.0f ) );

	EXPECT_EQ( Vector3f( 1.0f, 2.0f, 3.0f ), t.getTranslate() );

	t.lookAt( Vector3f( 0.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );

	EXPECT_EQ( Vector3f( -1.0f, -2.0f, -3.0f ).getNormalized(), t.computeDirection() );
}

TEST( Transformation, constructFromMatrix )
{
	Matrix4f m( 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				3.0f, 4.0f, 5.0f, 1.0f );

	Transformation t( m );

	EXPECT_EQ( Vector3f( 3.0f, 4.0f, 5.0f ), t.getTranslate() );
	EXPECT_EQ( 1.0f, t.getScale() );
	EXPECT_EQ( Quaternion4f( 0.0f, 0.0f, 0.0f, 1.0f ), t.getRotate() );
}

TEST( Transformation, getInverse )
{
	Transformation t0;
	t0.setTranslate( 1.0f, 2.0f, 3.0f );
	t0.rotate().fromEulerAngles( 0.0f, 90.0f, 0.0f );

	Transformation t1 = t0.getInverse();

    Transformation t2;
    t2.computeFrom( t0, t1 );

    EXPECT_TRUE( Vector3f::ZERO == t2.getTranslate() );
    EXPECT_TRUE( Quaternion4f( 0.0f, 0.0f, 0.0f, 1.0f ) == t2.getRotate() );
    EXPECT_TRUE( Numericf::equals( 1.0f, t2.getScale() ) );
}

TEST( Transformation, translate )
{
	Transformation t;

	EXPECT_TRUE( t.isIdentity() );

	t.setTranslate( 0.5f, 0.0f, 0.0f );

	EXPECT_FALSE( t.isIdentity() );
	EXPECT_EQ( Vector3f( 0.5f, 0.0f, 0.0f ), t.getTranslate() );

	t.makeIdentity();

	EXPECT_TRUE( t.isIdentity() );
	EXPECT_EQ( Vector3f::ZERO, t.getTranslate() );

	t.translate() = Vector3f( 0.0f, 0.3f, 0.0f );

	EXPECT_FALSE( t.isIdentity() );
	EXPECT_EQ( Vector3f( 0.0f, 0.3f, 0.0f ), t.getTranslate() );
}

TEST( Transformation, rotate )
{
	Transformation t;

	EXPECT_TRUE( t.isIdentity() );

	t.setRotate( Vector3f::UNIT_Y, 0.5f );

	EXPECT_FALSE( t.isIdentity() );

	t.makeIdentity();

	EXPECT_TRUE( t.isIdentity() );

	t.rotate() = Quaternion4f::createFromAxisAngle( Vector3f::UNIT_X, 0.5f );

	EXPECT_FALSE( t.isIdentity() );
}

TEST( Transformation, scale )
{
	Transformation t;

	EXPECT_TRUE( t.isIdentity() );

	t.setScale( 0.5f );

	EXPECT_FALSE( t.isIdentity() );
	EXPECT_EQ( 0.5f, t.getScale() );

	t.makeIdentity();

	EXPECT_TRUE( t.isIdentity() );
	EXPECT_EQ( 1.0f, t.getScale() );

	t.scale() = 0.3f;

	EXPECT_FALSE( t.isIdentity() );
	EXPECT_EQ( 0.3f, t.getScale() );
}

*/

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

            const auto v0 = Vector4( p.x(), p.y(), p.z(), 1.0 );
            const auto v1 = m_matrix * v0;
            return Point3 {
                v1.x(),
                v1.y(),
                v1.z(),
            };
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

    [[nodiscard]] constexpr Transformation inverse( const Transformation &t ) noexcept
    {
        return Transformation( t.getInverseMatrix(), t.getMatrix() );
    }

    [[nodiscard]] constexpr Transformation translation( const Vector3 &delta ) noexcept
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

    [[nodiscard]] constexpr Transformation translation( Real x, Real y, Real z ) noexcept
    {
        return translation( Vector3 { x, y, z } );
    }

    [[nodiscard]] constexpr Transformation scale( const Vector3 &scale ) noexcept
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

    [[nodiscard]] constexpr Transformation scale( Real x, Real y, Real z ) noexcept
    {
        return scale( Vector3 { x, y, z } );
    }

    [[nodiscard]] constexpr Transformation scale( Real x ) noexcept
    {
        return scale( Vector3 { x, x, x } );
    }

    // TODO: make this function constexpr
    [[nodiscard]] Transformation rotationX( Radians angle ) noexcept
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
    [[nodiscard]] Transformation rotationY( Radians angle ) noexcept
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
    [[nodiscard]] Transformation rotationZ( Radians angle ) noexcept
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
    [[nodiscard]] Transformation rotation( const Vector3 &axis, Radians angle ) noexcept
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

    [[nodiscard]] constexpr Transformation lookAt( const Point3 &pos, const Point3 &target, const Vector3 &up ) noexcept
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

TEST( Transformation, identity )
{
    constexpr auto T = crimild::Transformation {};

    static_assert( crimild::isIdentity( T ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, notIdentity )
{
    constexpr auto T = crimild::Transformation( crimild::Matrix4::Constants::IDENTITY, crimild::Matrix4::Constants::IDENTITY );

    static_assert( !crimild::isIdentity( T ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, translation )
{
    constexpr auto I = crimild::Transformation();
    constexpr auto T0 = crimild::translation( 0, 0, 0 );
    constexpr auto T1 = crimild::translation( 1, 2, 3 );
    constexpr auto T2 = crimild::translation( 4, 5, 6 );
    constexpr auto T3 = crimild::translation( 5, 7, 9 );
    constexpr auto T4 = crimild::translation( -1, -2, -3 );

    static_assert( I == T0 );
    static_assert( ( T1 * T2 ) == T3 );
    static_assert( ( T1 * T2 ) == ( T2 * T1 ) );
    static_assert( inverse( T1 ) == T4 );

    EXPECT_TRUE( true );
}

TEST( Transformation, translatePoint )
{
    constexpr auto P = crimild::Point3 { 10, 20, 30 };
    constexpr auto T = crimild::translation( 5, 12, 134 );

    static_assert( crimild::Point3 { 15, 32, 164 } == T( P ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, translateVector )
{
    constexpr auto V = crimild::Vector3 { 10, 20, 30 };
    constexpr auto T = crimild::translation( 5, 12, 134 );

    static_assert( V == T( V ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, translateNormal )
{
    constexpr auto N = crimild::Normal3 { 1, 2, 3 };
    constexpr auto T = crimild::translation( 5, 10, 20 );

    static_assert( N == T( N ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, translateRay3 )
{
    constexpr auto R = crimild::Ray3 {
        crimild::Point3 { 1, 2, 3 },
        crimild::Vector3 { 0, 0, 1 },
    };

    constexpr auto T = crimild::translation( 5, 10, 20 );

    constexpr auto R1 = crimild::Ray3 {
        crimild::Point3 { 6, 12, 23 },
        crimild::Vector3 { 0, 0, 1 },
    };

    static_assert( R1 == T( R ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, scaleProperties )
{
    constexpr auto I = crimild::Transformation();
    constexpr auto S = crimild::scale( 1, 1, 1 );
    constexpr auto S1 = crimild::scale( 1, 2, 3 );
    constexpr auto S2 = crimild::scale( 4, 5, 6 );
    constexpr auto S3 = crimild::scale( 4, 10, 18 );
    constexpr auto S4 = crimild::scale( 1.0, 1.0 / 2.0, 1.0 / 3.0 );

    static_assert( I == S );
    static_assert( ( S1 * S2 ) == S3 );
    static_assert( ( S1 * S2 ) == ( S2 * S1 ) );
    static_assert( inverse( S1 ) == S4 );

    EXPECT_TRUE( true );
}

TEST( Transformation, scalePoint )
{
    constexpr auto P = crimild::Point3 { 10, 20, 30 };
    constexpr auto S = crimild::scale( 5, 10, 2 );

    static_assert( crimild::Point3 { 50, 200, 60 } == S( P ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, uniformScalePoint )
{
    constexpr auto P = crimild::Point3 { 10, 20, 30 };
    constexpr auto S = crimild::scale( 5 );

    static_assert( crimild::Point3 { 50, 100, 150 } == S( P ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, scaleVector )
{
    constexpr auto V = crimild::Vector3 { 10, 20, 30 };
    constexpr auto S = crimild::scale( 5, 10, 2 );

    static_assert( crimild::Vector3 { 50, 200, 60 } == S( V ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, rotationXProperties )
{
    const auto I = crimild::Transformation {};
    const auto Rx0 = crimild::rotationX( 0 );
    const auto Rx1 = crimild::rotationX( 0.1 );
    const auto Rx2 = crimild::rotationX( 0.2 );
    const auto Rx3 = crimild::rotationX( 0.3 );
    const auto Rx4 = crimild::rotationX( -0.1 );

    EXPECT_EQ( I, Rx0 );
    EXPECT_EQ( Rx1 * Rx2, Rx3 );
    EXPECT_EQ( Rx1 * Rx2, Rx2 * Rx1 );
    EXPECT_EQ( inverse( Rx1 ), Rx4 );
}

TEST( Transformation, rotationYProperties )
{
    const auto I = crimild::Transformation {};
    const auto Ry0 = crimild::rotationY( 0 );
    const auto Ry1 = crimild::rotationY( 0.1 );
    const auto Ry2 = crimild::rotationY( 0.2 );
    const auto Ry3 = crimild::rotationY( 0.3 );
    const auto Ry4 = crimild::rotationY( -0.1 );

    EXPECT_EQ( I, Ry0 );
    EXPECT_EQ( Ry1 * Ry2, Ry3 );
    EXPECT_EQ( Ry1 * Ry2, Ry2 * Ry1 );
    EXPECT_EQ( inverse( Ry1 ), Ry4 );
}

TEST( Transformation, rotationZProperties )
{
    const auto I = crimild::Transformation {};
    const auto Rz0 = crimild::rotationZ( 0 );
    const auto Rz1 = crimild::rotationZ( 0.1 );
    const auto Rz2 = crimild::rotationZ( 0.2 );
    const auto Rz3 = crimild::rotationZ( 0.3 );
    const auto Rz4 = crimild::rotationZ( -0.1 );

    EXPECT_EQ( I, Rz0 );
    EXPECT_EQ( Rz1 * Rz2, Rz3 );
    EXPECT_EQ( Rz1 * Rz2, Rz2 * Rz1 );
    EXPECT_EQ( inverse( Rz1 ), Rz4 );
}

TEST( Transformation, rotationProperties )
{
    const auto I = crimild::Transformation {};
    const auto R0 = crimild::rotation( crimild::normalize( crimild::Vector3 { 1, 2, 3 } ), 0 );
    const auto R1 = crimild::rotation( crimild::normalize( crimild::Vector3 { 1, 2, 3 } ), 0.1 );
    const auto R2 = crimild::rotation( crimild::normalize( crimild::Vector3 { 1, 2, 3 } ), 0.2 );
    const auto R3 = crimild::rotation( crimild::normalize( crimild::Vector3 { 1, 2, 3 } ), 0.3 );
    const auto R4 = crimild::rotation( crimild::normalize( crimild::Vector3 { 1, 2, 3 } ), -0.1 );

    EXPECT_EQ( I, R0 );
    EXPECT_EQ( R1 * R2, R3 );
    EXPECT_EQ( R1 * R2, R2 * R1 );
    EXPECT_EQ( inverse( R1 ), R4 );
}

TEST( Transformation, lookAt )
{
    constexpr auto I = crimild::Transformation {};
    constexpr auto T = crimild::lookAt(
        crimild::Point3 { 5, 5, 5 },
        crimild::Point3 { 0, 1, 0 },
        crimild::Vector3 { 0, 1, 0 } );

    static_assert( I != T );

    EXPECT_TRUE( true );
}
