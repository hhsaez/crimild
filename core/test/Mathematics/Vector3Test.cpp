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

#include "Mathematics/Vector3.hpp"

#include "gtest/gtest.h"
#include <sstream>

TEST( Vector3, construction )
{
    constexpr auto u = crimild::Vector3( 10, 20, 30 );

    EXPECT_EQ( 10, u.x() );
    EXPECT_EQ( 20, u.y() );
    EXPECT_EQ( 30, u.z() );
}

TEST( Vector3, index )
{
    constexpr auto u = crimild::Vector3f { 10, 20, 30 };

    EXPECT_EQ( 10, u[ 0 ] );
    EXPECT_EQ( 20, u[ 1 ] );
    EXPECT_EQ( 30, u[ 2 ] );
}

TEST( Vector3, equality )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 30, 40, 50 };
    constexpr auto w = crimild::Vector3 { 10, 20, 30 };

    EXPECT_EQ( u, w );
    EXPECT_NE( u, v );
    EXPECT_NE( v, w );
}

TEST( Vector3, addition )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 30, 40, 50 };
    constexpr auto res = crimild::Vector3 { 40, 60, 80 };

    EXPECT_EQ( res, u + v );
}

TEST( Vector3, subtraction )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 30, 40, 50 };
    constexpr auto res = crimild::Vector3 { -20, -20, -20 };

    EXPECT_EQ( res, u - v );
}

TEST( Vector3, multiplication )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto s = crimild::Real( 5 );
    constexpr auto res = crimild::Vector3 { 50, 100, 150 };

    EXPECT_EQ( res, u * s );
    EXPECT_EQ( res, s * u );
}

TEST( Vector3, vectorMultiplication )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 2, 3, 4 };
    constexpr auto res = crimild::Vector3 { 20, 60, 120 };

    EXPECT_EQ( res, u * v );
}

TEST( Vector3, division )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto s = crimild::Real( 2 );
    constexpr auto res = crimild::Vector3 { 5, 10, 15 };

    EXPECT_EQ( res, u / s );
}

TEST( Vector3, negation )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto res = crimild::Vector3 { -10, -20, -30 };

    EXPECT_EQ( res, -u );
}

TEST( Vector3, isNaN )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { NAN, NAN, NAN };
    constexpr auto w = crimild::Vector3 { NAN, NAN, 0 };

    EXPECT_TRUE( crimild::isNaN( v ) );
    EXPECT_FALSE( crimild::isNaN( u ) );
    EXPECT_TRUE( crimild::isNaN( w ) );
}

TEST( Vector3, abs )
{
    constexpr auto u = crimild::Vector3 { -10, -20, -30 };
    constexpr auto res = crimild::Vector3 { 10, 20, 30 };

    EXPECT_EQ( res, crimild::abs( u ) );
}

TEST( Vector3, dot )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 5 };
    constexpr auto v = crimild::Vector3 { 8, 10, -2 };

    EXPECT_EQ( 36, crimild::dot( u, v ) );
}

TEST( Vector3, absDot )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 5 };
    constexpr auto v = crimild::Vector3 { -8, -10, -2 };

    EXPECT_EQ( 56, crimild::absDot( u, v ) );
}

TEST( Vector3, cross )
{
    constexpr auto i = crimild::Vector3 { 1, 0, 0 };
    constexpr auto j = crimild::Vector3 { 0, 1, 0 };
    constexpr auto k = crimild::Vector3 { 0, 0, 1 };

    EXPECT_EQ( k, crimild::cross( i, j ) );
    EXPECT_EQ( i, crimild::cross( j, k ) );
    EXPECT_EQ( j, crimild::cross( k, i ) );
}

TEST( Vector3, length )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 4 };

    EXPECT_EQ( crimild::Real( 29 ), crimild::lengthSquared( u ) );
    EXPECT_EQ( crimild::Real( 5.3851648071 ), crimild::length( u ) );
}

TEST( Vector3, normalize )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 4 };
    constexpr auto v = crimild::Vector3 {
        0.3713906764,
        0.5570860145,
        0.7427813527,
    };

    EXPECT_EQ( v, crimild::normalize( u ) );
    EXPECT_TRUE( crimild::isEqual( crimild::Real( 1 ), crimild::lengthSquared( crimild::normalize( u ) ) ) );
    EXPECT_TRUE( crimild::isEqual( crimild::Real( 1 ), crimild::length( crimild::normalize( u ) ) ) );
}

TEST( Vector3, normalizeNaN )
{
    EXPECT_TRUE( crimild::isNaN( crimild::normalize( crimild::Vector3 {} ) ) );
}

TEST( Vector3, min )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 4 };
    constexpr auto v = crimild::Vector3 { 1, 5, 2 };
    constexpr auto m = crimild::Vector3 { 1, 3, 2 };

    EXPECT_EQ( 2, crimild::min( u ) );
    EXPECT_EQ( m, crimild::min( u, v ) );
}

TEST( Vector3, max )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 4 };
    constexpr auto v = crimild::Vector3 { 1, 5, 2 };
    constexpr auto m = crimild::Vector3 { 2, 5, 4 };

    EXPECT_EQ( 4, crimild::max( u ) );
    EXPECT_EQ( m, crimild::max( u, v ) );
}

TEST( Vector3, minDimension )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 4 };

    EXPECT_EQ( 0, crimild::minDimension( u ) );
}

TEST( Vector3, maxDimension )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 4 };

    EXPECT_EQ( 2, crimild::maxDimension( u ) );
}

TEST( Vector3, ostream )
{
    {
        constexpr auto u = crimild::Vector3 { 2, 3, 4 };

        std::stringstream ss;
        ss << u;

        EXPECT_EQ( ss.str(), "(2.000000, 3.000000, 4.000000)" );
    }

    {
        constexpr auto u = crimild::Vector3i { 2, 3, 4 };

        std::stringstream ss;
        ss << u;

        EXPECT_EQ( ss.str(), "(2, 3, 4)" );
    }
}

TEST( Vector3, orthonormalBasis )
{
    constexpr auto v1 = crimild::Vector3 { 4, 5, 6 };
    crimild::Vector3 v2, v3;

    crimild::orthonormalBasis( v1, v2, v3 );

    EXPECT_TRUE( crimild::isZero( crimild::dot( v1, v2 ) ) );
    EXPECT_TRUE( crimild::isZero( crimild::dot( v1, v3 ) ) );
    EXPECT_TRUE( crimild::isZero( crimild::dot( v2, v3 ) ) );
}

TEST( Vector3, permute )
{
    constexpr auto u = crimild::Vector3 { 4, 5, 6 };
    constexpr auto v = crimild::Vector3 { 6, 4, 5 };

    EXPECT_EQ( v, crimild::permute( u, 2, 0, 1 ) );
}

TEST( Vector3, constexpr )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 30, 40, 50 };
    constexpr auto w = crimild::Vector3 { 10, 20, 30 };
    constexpr auto s = crimild::Real( 5 );

    static_assert( u == w, "equality" );
    static_assert( u != v, "inequality" );

    static_assert( 10 == u[ 0 ], "index" );
    static_assert( 20 == u[ 1 ], "index" );
    static_assert( 30 == u[ 2 ], "index" );

    static_assert( crimild::Vector3 { 40, 60, 80 } == ( u + v ), "addition" );
    static_assert( crimild::Vector3 { -20, -20, -20 } == ( u - v ), "subtraction" );

    static_assert( crimild::Vector3 { 50, 100, 150 } == ( u * s ), "multiplication" );
    static_assert( crimild::Vector3 { 50, 100, 150 } == ( s * u ), "multiplication" );
    static_assert( crimild::Vector3 { 300, 800, 1500 } == ( u * v ), "multiplication" );
    static_assert( crimild::Vector3 { 2, 4, 6 } == ( u / s ), "division" );

    static_assert( crimild::Vector3 { -10, -20, -30 } == -u, "negation" );

    static_assert( crimild::isNaN( u ) == false, "isNaN" );
    static_assert( crimild::isNaN( crimild::Vector3 { NAN, NAN, NAN } ) == true, "isNaN" );

    static_assert( crimild::Vector3 { 10, 20, 30 } == crimild::abs( crimild::Vector3 { -10, -20, -30 } ), "abs" );

    static_assert( 36 == crimild::dot( crimild::Vector3 { 2, 3, 5 }, crimild::Vector3 { 8, 10, -2 } ), "dot" );
    static_assert( 56 == crimild::absDot( crimild::Vector3 { 2, 3, 5 }, crimild::Vector3 { -8, -10, -2 } ), "absDot" );

    static_assert( crimild::Vector3 { 0, 0, 1 } == crimild::cross( crimild::Vector3 { 1, 0, 0 }, crimild::Vector3 { 0, 1, 0 } ), "cross" );

    static_assert( crimild::Real( 29 ) == crimild::lengthSquared( crimild::Vector3 { 2, 3, 4 } ), "lengthSquared" );
    static_assert( crimild::Real( 5.3851648071 ) == crimild::length( crimild::Vector3 { 2, 3, 4 } ), "length" );

    static_assert( crimild::Vector3 { 0.3713906764, 0.5570860145, 0.7427813527 } == crimild::normalize( crimild::Vector3 { 2, 3, 4 } ), "normalize" );
    static_assert( crimild::isEqual( crimild::Real( 1 ), crimild::length( crimild::normalize( crimild::Vector3 { 2, 3, 4 } ) ) ), "normalize" );

    static_assert( crimild::Vector3 { 1, 3, 2 } == crimild::min( crimild::Vector3 { 2, 3, 4 }, crimild::Vector3 { 1, 5, 2 } ), "min" );
    static_assert( 2 == crimild::min( crimild::Vector3 { 2, 3, 4 } ), "min" );
    static_assert( 0 == crimild::minDimension( crimild::Vector3 { 2, 3, 4 } ), "minDimension" );

    static_assert( crimild::Vector3 { 2, 5, 4 } == crimild::max( crimild::Vector3 { 2, 3, 4 }, crimild::Vector3 { 1, 5, 2 } ), "max" );
    static_assert( 4 == crimild::max( crimild::Vector3 { 2, 3, 4 } ), "max" );
    static_assert( 2 == crimild::maxDimension( crimild::Vector3 { 2, 3, 4 } ), "maxDimension" );

    EXPECT_TRUE( true );
}
