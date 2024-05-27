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

#include "Transformation.hpp"

#include "Normal3.hpp"
#include "Point3.hpp"
#include "Transformation.hpp"
#include "Vector3.hpp"
#include "easing.hpp"
#include "io.hpp"
#include "isEqual.hpp"
#include "lookAt.hpp"
#include "normalize.hpp"
#include "rotation.hpp"
#include "scale.hpp"
#include "translation.hpp"

#include "gtest/gtest.h"

TEST( Transformation, defaultIsIdentity )
{
    constexpr auto T = crimild::Transformation {};

    static_assert( crimild::isIdentity( T ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, identity )
{
    constexpr auto T = crimild::Transformation::Constants::IDENTITY;

    static_assert( crimild::isIdentity( T ) );
    static_assert( isEqual( T( T ), T ) );
    // static_asset( crimild::isEqual( crimild::inverse( T ), T ) );
    EXPECT_EQ( crimild::inverse( T ), T );

    EXPECT_TRUE( true );
}

TEST( Transformation, translation )
{
    constexpr auto I = crimild::Transformation::Constants::IDENTITY;
    constexpr auto T0 = crimild::Transformation { .translate = crimild::Point3 { 0, 0, 0 } };
    constexpr auto T1 = crimild::Transformation { .translate = crimild::Point3 { 1, 2, 3 } };
    constexpr auto T2 = crimild::Transformation { .translate = crimild::Point3 { 4, 5, 6 } };
    constexpr auto T3 = crimild::Transformation { .translate = crimild::Point3 { 5, 7, 9 } };
    constexpr auto T4 = crimild::Transformation { .translate = crimild::Point3 { -1, -2, -3 } };

    static_assert( !crimild::hasTranslation( T0 ) );
    static_assert( crimild::hasTranslation( T1 ) );

    static_assert( crimild::isEqual( I, T0 ) );
    static_assert( crimild::isEqual( T1( T2 ), T3 ) );
    static_assert( crimild::isEqual( T1( T2 ), T2( T1 ) ) );
    static_assert( crimild::isEqual( crimild::inverse( T1 ), T4 ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, translatePoint )
{
    constexpr auto P = crimild::Point3f { 10, 20, 30 };
    constexpr auto T = crimild::Transformation { .translate = crimild::Point3 { 5, 12, 134 } };

    static_assert( crimild::isEqual( crimild::Point3f { 15, 32, 164 }, T( P ) ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, translateVector )
{
    constexpr auto V = crimild::Vector3 { 10, 20, 30 };
    constexpr auto T = crimild::Transformation { .translate = crimild::Point3 { 5, 12, 134 } };

    static_assert( crimild::isEqual( V, T( V ) ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, translateNormal )
{
    constexpr auto N = crimild::Normal3 { 1, 2, 3 };
    constexpr auto T = crimild::Transformation { .translate = crimild::Point3 { 5, 10, 120 } };

    static_assert( crimild::isEqual( N, T( N ) ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, translateRay3 )
{
    constexpr auto R = crimild::Ray3 {
        crimild::Point3 { 1, 2, 3 },
        crimild::Vector3 { 0, 1, 0 },
    };

    constexpr auto T = crimild::Transformation {
        .translate = { 3, 4, 5 },
    };

    constexpr auto R1 = crimild::Ray3 {
        crimild::Point3 { 4, 6, 8 },
        crimild::Vector3 { 0, 1, 0 },
    };

    static_assert( crimild::isEqual( R1, T( R ) ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, scaleProperties )
{
    constexpr auto I = crimild::Transformation::Constants::IDENTITY;
    constexpr auto S = crimild::Transformation { .scale = { 1, 1, 1 } };
    constexpr auto S1 = crimild::Transformation { .scale = { 1, 2, 3 } };
    constexpr auto S2 = crimild::Transformation { .scale = { 4, 5, 6 } };
    constexpr auto S3 = crimild::Transformation { .scale = { 4, 10, 18 } };
    constexpr auto S4 = crimild::Transformation { .scale = { 1.0, 1.0 / 2.0, 1.0 / 3.0 } };

    static_assert( !crimild::hasScale( S ) );

    static_assert( crimild::isEqual( I, S ) );
    static_assert( crimild::isEqual( S1( S2 ), S3 ) );
    static_assert( crimild::isEqual( S1( S2 ), S2( S1 ) ) );
    static_assert( crimild::isEqual( crimild::inverse( S1 ), S4 ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, scalePoint )
{
    constexpr auto P = crimild::Point3f { 10, 20, 30 };
    constexpr auto S = crimild::Transformation { .scale = { 5, 10, 2 } };

    static_assert( crimild::isEqual( crimild::Point3f { 50, 200, 60 }, S( P ) ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, uniformScalePoint )
{
    constexpr auto P = crimild::Point3f { 10, 20, 30 };
    constexpr auto S = crimild::Transformation { .scale = { 5, 5, 5 } };

    static_assert( crimild::isEqual( crimild::Point3f { 50, 100, 150 }, S( P ) ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, scaleVector )
{
    constexpr auto V = crimild::Vector3 { 10, 20, 30 };
    constexpr auto S = crimild::Transformation { .scale = { 5, 10, 2 } };

    static_assert( crimild::isEqual( crimild::Vector3 { 50, 200, 60 }, S( V ) ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, negative_scale_to_reflect_vector )
{
    constexpr auto V = crimild::Vector3 { 10, 20, 30 };
    constexpr auto S = crimild::Transformation { .scale = { -5, -10, -2 } };

    static_assert( crimild::isEqual( crimild::Vector3 { -50, -200, -60 }, S( V ) ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, negative_scale_to_reflect_normal )
{
    constexpr auto V = crimild::Normal3 { 10, 20, 30 };
    constexpr auto S = crimild::Transformation { .scale = { -5, -10, -2 } };

    static_assert( crimild::swapsHandedness( S ) );

    static_assert( crimild::isEqual( crimild::Normal3 { -10, -20, -30 }, S( V ) ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, swaps_handedness )
{
    constexpr auto S1 = crimild::Transformation { .scale = { 5, 10, 2 } };
    constexpr auto S2 = crimild::Transformation { .scale = { -5, -10, -2 } };

    static_assert( !crimild::swapsHandedness( S1 ) );
    static_assert( crimild::swapsHandedness( S2 ) );

    EXPECT_TRUE( true );
}

TEST( Transformation, scaleRay3 )
{
    constexpr auto R = crimild::Ray3 {
        crimild::Point3 { 1, 2, 3 },
        crimild::Vector3 { 0, 1, 0 },
    };

    constexpr auto S = crimild::Transformation { .scale = { 2, 3, 4 } };

    static_assert( S( R ) == crimild::Ray3 { crimild::Point3 { 2, 6, 12 }, crimild::Vector3 { 0, 3, 0 } } );

    EXPECT_TRUE( true );
}

TEST( Transformation, rotationX )
{
    constexpr auto I = crimild::Transformation::Constants::IDENTITY;
    const auto Rx0 = crimild::rotationX( 0 );
    const auto Rx1 = crimild::rotationX( 0.1 );
    const auto Rx2 = crimild::rotationX( 0.2 );
    const auto Rx3 = crimild::rotationX( 0.3 );
    const auto Rx4 = crimild::rotationX( -0.1 );

    EXPECT_TRUE( !crimild::hasRotation( Rx0 ) );

    EXPECT_TRUE( crimild::isEqual( I, Rx0 ) );
    EXPECT_TRUE( crimild::isEqual( Rx1( Rx2 ), Rx3 ) );
    EXPECT_TRUE( crimild::isEqual( Rx1( Rx2 ), Rx2( Rx1 ) ) );
    EXPECT_TRUE( crimild::isEqual( inverse( Rx1 ), Rx4 ) );
}

TEST( Transformation, rotationY )
{
    constexpr auto I = crimild::Transformation::Constants::IDENTITY;
    const auto Ry0 = crimild::rotationY( 0 );
    const auto Ry1 = crimild::rotationY( 0.1 );
    const auto Ry2 = crimild::rotationY( 0.2 );
    const auto Ry3 = crimild::rotationY( 0.3 );
    const auto Ry4 = crimild::rotationY( -0.1 );

    EXPECT_TRUE( !crimild::hasRotation( Ry0 ) );

    EXPECT_TRUE( crimild::isEqual( I, Ry0 ) );
    EXPECT_TRUE( crimild::isEqual( Ry1( Ry2 ), Ry3 ) );
    EXPECT_TRUE( crimild::isEqual( Ry1( Ry2 ), Ry2( Ry1 ) ) );
    EXPECT_TRUE( crimild::isEqual( inverse( Ry1 ), Ry4 ) );
}

TEST( Transformation, rotationZ )
{
    const auto I = crimild::Transformation {};
    const auto Rz0 = crimild::rotationZ( 0 );
    const auto Rz1 = crimild::rotationZ( 0.1 );
    const auto Rz2 = crimild::rotationZ( 0.2 );
    const auto Rz3 = crimild::rotationZ( 0.3 );
    const auto Rz4 = crimild::rotationZ( -0.1 );

    EXPECT_TRUE( !crimild::hasRotation( Rz0 ) );

    EXPECT_TRUE( crimild::isEqual( I, Rz0 ) );
    EXPECT_TRUE( crimild::isEqual( Rz1( Rz2 ), Rz3 ) );
    EXPECT_TRUE( crimild::isEqual( Rz1( Rz2 ), Rz2( Rz1 ) ) );
    EXPECT_TRUE( crimild::isEqual( inverse( Rz1 ), Rz4 ) );
}

TEST( Transformation, rotation )
{
    const auto I = crimild::Transformation {};
    const auto R0 = crimild::rotation( crimild::normalize( crimild::Vector3 { 1, 2, 3 } ), 0 );
    const auto R1 = crimild::rotation( crimild::normalize( crimild::Vector3 { 1, 2, 3 } ), 0.1 );
    const auto R2 = crimild::rotation( crimild::normalize( crimild::Vector3 { 1, 2, 3 } ), 0.2 );
    const auto R3 = crimild::rotation( crimild::normalize( crimild::Vector3 { 1, 2, 3 } ), 0.3 );
    const auto R4 = crimild::rotation( crimild::normalize( crimild::Vector3 { 1, 2, 3 } ), -0.1 );

    EXPECT_TRUE( !crimild::hasRotation( R0 ) );

    EXPECT_TRUE( crimild::isEqual( I, R0 ) );
    EXPECT_TRUE( crimild::isEqual( R1( R2 ), R3 ) );
    EXPECT_TRUE( crimild::isEqual( R1( R2 ), R2( R1 ) ) );
    EXPECT_TRUE( crimild::isEqual( crimild::inverse( R1 ), R4 ) );

    EXPECT_TRUE( isEqual( crimild::rotationX( 1.234 ), crimild::rotation( crimild::Vector3::Constants::UNIT_X, 1.234 ) ) );
    EXPECT_TRUE( isEqual( crimild::rotationY( 1.234 ), crimild::rotation( crimild::Vector3::Constants::UNIT_Y, 1.234 ) ) );
    EXPECT_EQ( crimild::rotationZ( 1.234 ), crimild::rotation( crimild::Vector3::Constants::UNIT_Z, 1.234 ) );
}

TEST( Transformation, evaluation )
{
    constexpr auto T0 = crimild::translation( 2, 2, 1 );
    constexpr auto T1 = crimild::translation( 2, 2, 1 );

    {
        constexpr auto T = T0( T1 );
        static_assert( !crimild::isIdentity( T ) );
        static_assert( crimild::hasTranslation( T ) );
        static_assert( !crimild::hasRotation( T ) );
        static_assert( !crimild::hasScale( T ) );
        static_assert( crimild::origin( T ) == crimild::Point3f { 4, 4, 2 } );
    }

    {
        constexpr auto T = T0( crimild::Transformation::Constants::IDENTITY );
        static_assert( !crimild::isIdentity( T ) );
        static_assert( crimild::hasTranslation( T ) );
        static_assert( !crimild::hasRotation( T ) );
        static_assert( !crimild::hasScale( T ) );
        static_assert( crimild::origin( T ) == crimild::Point3f { 2, 2, 1 } );
    }

    {
        constexpr auto T = crimild::Transformation::Constants::IDENTITY( T0 );
        static_assert( !crimild::isIdentity( T ) );
        static_assert( crimild::hasTranslation( T ) );
        static_assert( !crimild::hasRotation( T ) );
        static_assert( !crimild::hasScale( T ) );
        static_assert( crimild::origin( T ) == crimild::Point3f { 2, 2, 1 } );
    }

    EXPECT_TRUE( true );
}

TEST( Transformation, lookAt_default_orientation )
{
    const auto T = crimild::lookAt(
        crimild::Point3 { 0, 0, 0 },
        crimild::Point3 { 0, 0, -1 },
        crimild::Vector3 { 0, 1, 0 }
    );

    EXPECT_TRUE( crimild::isIdentity( T ) );
    EXPECT_TRUE( !crimild::hasTranslation( T ) );
    EXPECT_TRUE( !crimild::hasRotation( T ) );
    EXPECT_TRUE( !crimild::hasScale( T ) );
    EXPECT_EQ( crimild::Transformation::Constants::IDENTITY, T );
}

TEST( Transformation, lookAt_positive_z_direction )
{
    const auto T = crimild::lookAt(
        crimild::Point3 { 0, 0, 0 },
        crimild::Point3 { 0, 0, 1 },
        crimild::Vector3 { 0, 1, 0 }
    );

    // like looking backwards
    EXPECT_TRUE( isEqual( crimild::rotationY( crimild::numbers::PI ), T ) );
}

TEST( Transformation, lookAt_moves_the_world )
{
    const auto T = crimild::lookAt(
        crimild::Point3 { 0, 0, 8 },
        crimild::Point3 { 0, 0, 0 },
        crimild::Vector3 { 0, 1, 0 }
    );

    EXPECT_EQ( crimild::translation( 0, 0, 8 ), T );
    EXPECT_EQ( crimild::inverse( crimild::translation( 0, 0, 8 ) ), crimild::inverse( T ) );

    EXPECT_EQ( ( crimild::Point3f { 0, 0, -8 } ), crimild::inverse( T )( crimild::Point3f { 0, 0, 0 } ) );
}

TEST( Transformation, lookAt_arbitrary )
{
    const auto T = crimild::lookAt(
        crimild::Point3 { 5, 5, 5 },
        crimild::Point3 { 0, 1, 0 },
        crimild::Vector3 { 0, 1, 0 }
    );

    EXPECT_TRUE( isEqual( crimild::origin( T ), ( crimild::Point3 { 5, 5, 5 } ) ) );
    EXPECT_TRUE( isEqual( crimild::forward( T ), crimild::normalize( crimild::Vector3 { 0, 1, 0 } - crimild::Vector3 { 5, 5, 5 } ) ) );

    EXPECT_TRUE( crimild::hasTranslation( T ) );
    EXPECT_TRUE( crimild::hasRotation( T ) );
    EXPECT_TRUE( !crimild::hasScale( T ) );
}

TEST( Transformation, lookAt_handles_invalid_up )
{
    // Looking down
    const auto T = crimild::lookAt(
        crimild::Point3 { 0, 0, 0 },
        crimild::Point3 { 0, -1, 0 },
        crimild::Vector3 { 0, 1, 0 }
    );

    EXPECT_TRUE( true );
}

TEST( Transformation, lookAt_looking_down )
{
    // Looking down
    const auto T = crimild::lookAt(
        crimild::Point3 { 0, 0, 0 },
        crimild::Point3 { 0, -1, 0 },
        crimild::Vector3 { 0, 0, -1 }
    );

    EXPECT_EQ( ( crimild::Vector3 { 0, -1, 0 } ), forward( T ) );
    EXPECT_EQ( ( crimild::Vector3 { 0, 0, -1 } ), up( T ) );
    EXPECT_EQ( ( crimild::Vector3 { 1, 0, 0 } ), right( T ) );
    EXPECT_EQ( ( crimild::Point3f { 0, 0, 0 } ), origin( T ) );
}

TEST( Transformation, transforms_after_lookAt )
{
    // Looking down
    const auto T = crimild::lookAt(
        crimild::Point3 { 0, 2, 0 },
        crimild::Point3 { 0, 0.6, 0 },
        crimild::Vector3 { 0, 0, -1 }
    );

    EXPECT_EQ( ( crimild::Vector3 { 0, -1, 0 } ), forward( T ) );
    EXPECT_EQ( ( crimild::Vector3 { 0, 0, -1 } ), up( T ) );
    EXPECT_EQ( ( crimild::Vector3 { 1, 0, 0 } ), right( T ) );
    EXPECT_EQ( ( crimild::Point3f { 0, 2, 0 } ), origin( T ) );

    EXPECT_TRUE( isEqual( ( crimild::Point3f { 0, 1, -2 } ), T( crimild::Point3 { 0, 2, -1 } ) ) );
    EXPECT_TRUE( isEqual( ( crimild::Point3f { 1, 2, -2 } ), T( crimild::Point3 { 1, 2, 0 } ) ) );
    EXPECT_TRUE( isEqual( ( crimild::Point3f { 0, 2, -1 } ), T( crimild::Point3 { 0, 1, 0 } ) ) );

    // Vectors are not affected by position
    EXPECT_TRUE( isEqual( ( crimild::Vector3 { 0, -1, -2 } ), T( crimild::Vector3 { 0, 2, -1 } ) ) );
    EXPECT_TRUE( isEqual( ( crimild::Vector3 { 1, 0, -2 } ), T( crimild::Vector3 { 1, 2, 0 } ) ) );
    EXPECT_TRUE( isEqual( ( crimild::Vector3 { 0, 0, -1 } ), T( crimild::Vector3 { 0, 1, 0 } ) ) );
}

TEST( Transfromation, origin )
{
    constexpr auto I = crimild::Transformation::Constants::IDENTITY;
    static_assert( crimild::origin( I ) == crimild::Point3f::Constants::ZERO );

    constexpr auto T = crimild::translation( 5, 12, 134 );
    static_assert( crimild::Point3f { 5, 12, 134 } == crimild::origin( T ) );

    EXPECT_TRUE( true );
}

TEST( Transfromation, right )
{
    constexpr auto I = crimild::Transformation::Constants::IDENTITY;
    static_assert( crimild::right( I ) == crimild::Vector3::Constants::RIGHT );

    const auto R = crimild::rotationZ( crimild::numbers::PI_DIV_2 );
    EXPECT_TRUE( isEqual( ( crimild::Vector3::Constants::UP ), crimild::right( R ) ) );
}

TEST( Transfromation, up )
{
    constexpr auto I = crimild::Transformation::Constants::IDENTITY;
    static_assert( crimild::up( I ) == crimild::Vector3::Constants::UP );

    const auto R = crimild::rotationX( -crimild::numbers::PI_DIV_2 );
    EXPECT_TRUE( isEqual( crimild::Vector3::Constants::FORWARD, crimild::up( R ) ) );
}

TEST( Transfromation, forward )
{
    constexpr auto I = crimild::Transformation::Constants::IDENTITY;
    static_assert( crimild::forward( I ) == crimild::Vector3::Constants::FORWARD );

    const auto R = crimild::rotationY( -crimild::numbers::PI_DIV_2 );
    EXPECT_TRUE( isEqual( crimild::Vector3::Constants::RIGHT, crimild::forward( R ) ) );
}

TEST( Transformation, easing )
{
    constexpr auto T0 = crimild::translation( 1, 2, 3 );
    constexpr auto T1 = crimild::translation( 2, 4, 6 );
    constexpr auto T2 = crimild::translation( 1.5, 3, 4.5 );
    EXPECT_TRUE( crimild::isEqual( crimild::lerp( T0, T1, 0.5 ), T2 ) );

    constexpr auto S0 = crimild::scale( 1 );
    constexpr auto S1 = crimild::scale( 2 );
    EXPECT_TRUE( crimild::isEqual( crimild::lerp( S0, S1, 0.5 ), crimild::scale( 1.5 ) ) );

    EXPECT_TRUE(
        crimild::isEqual(
            crimild::lerp(
                crimild::rotationX( 0 ),
                crimild::rotationX( 1 ),
                0.5
            ),
            crimild::rotationX( 0.5 )
        )
    );
}

TEST( Transformation, to_matrix )
{
    constexpr auto T = crimild::translation( 1, 2, 3 );
    constexpr auto M = crimild::Matrix4( T );
    EXPECT_TRUE( crimild::isEqual( M[ 3 ], crimild::Vector4( 1, 2, 3, 1 ) ) );

    constexpr auto S = crimild::scale( 1, 2, 3 );
    constexpr auto MS = crimild::Matrix4( S );
    EXPECT_EQ( MS[ 0 ][ 0 ], 1 );
    EXPECT_EQ( MS[ 1 ][ 1 ], 2 );
    EXPECT_EQ( MS[ 2 ][ 2 ], 3 );
    EXPECT_EQ( MS[ 3 ][ 3 ], 1 );
}
