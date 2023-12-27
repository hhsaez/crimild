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

#include "Vector3.hpp"

#include "abs.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "io.hpp"
#include "isEqual.hpp"
#include "isNaN.hpp"
#include "length.hpp"
#include "max.hpp"
#include "min.hpp"
#include "orthonormalization.hpp"
#include "permutation.hpp"

#include "gtest/gtest.h"
#include <sstream>

using namespace crimild;

TEST( Vector3, construction )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };

    EXPECT_EQ( 10, u.x );
    EXPECT_EQ( 20, u.y );
    EXPECT_EQ( 30, u.z );
}

TEST( Vector3, fromPoint3f )
{
    constexpr auto p3 = Point3i( 1, 2, 3 );
    constexpr auto v3 = Vector3i( p3 );
    static_assert( v3 == Vector3( 1, 2, 3 ) );
    EXPECT_TRUE( true );
}

TEST( Vector3, size )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };

    static_assert( 3 * sizeof( real_t ) == sizeof( u ) );
    EXPECT_TRUE( true );
}

TEST( Vector3, index )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };

    static_assert( 10 == u[ 0 ], "index" );
    static_assert( 20 == u[ 1 ], "index" );
    static_assert( 30 == u[ 2 ], "index" );

    EXPECT_TRUE( true );
}

TEST( Vector3, equality )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 30, 40, 50 };
    constexpr auto w = crimild::Vector3 { 10, 20, 30 };

    static_assert( crimild::isEqual( u, w ), "equality" );
    static_assert( !crimild::isEqual( u, v ), "inequality" );

    EXPECT_TRUE( true );
}

TEST( Vector3, addition )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 30, 40, 50 };
    constexpr auto res = crimild::Vector3 { 40, 60, 80 };

    static_assert( crimild::isEqual( res, u + v ) );

    EXPECT_TRUE( true );
}

TEST( Vector3, subtraction )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 30, 40, 50 };
    constexpr auto res = crimild::Vector3 { -20, -20, -20 };

    static_assert( crimild::isEqual( res, u - v ) );

    EXPECT_TRUE( true );
}

TEST( Vector3, multiplication )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto s = real_t( 5 );
    constexpr auto res = crimild::Vector3 { 50, 100, 150 };

    static_assert( crimild::isEqual( res, u * s ) );
    static_assert( crimild::isEqual( res, s * u ) );

    EXPECT_TRUE( true );
}

TEST( Vector3, vectorMultiplication )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 2, 3, 4 };
    constexpr auto res = crimild::Vector3 { 20, 60, 120 };

    static_assert( crimild::isEqual( res, u * v ) );

    EXPECT_TRUE( true );
}

TEST( Vector3, division )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto s = real_t( 2 );
    constexpr auto res = crimild::Vector3 { 5, 10, 15 };

    static_assert( crimild::isEqual( res, u / s ) );

    EXPECT_TRUE( true );
}

TEST( Vector3, negation )
{
    constexpr auto u = crimild::Vector3 { 10, 20, 30 };
    constexpr auto res = crimild::Vector3 { -10, -20, -30 };

    static_assert( crimild::isEqual( res, -u ) );

    EXPECT_TRUE( true );
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
    constexpr auto expected = crimild::Vector3 { 10, 20, 30 };
    constexpr auto res = crimild::abs( u );
    static_assert( crimild::isEqual( res, expected ) );

    EXPECT_TRUE( true );
}

TEST( Vector3, dot )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 5 };
    constexpr auto v = crimild::Vector3 { 8, 10, -2 };

    EXPECT_EQ( 36, crimild::dot( u, v ) );
}

TEST( Vector3, cross )
{
    constexpr auto i = crimild::Vector3 { 1, 0, 0 };
    constexpr auto j = crimild::Vector3 { 0, 1, 0 };
    constexpr auto k = crimild::Vector3 { 0, 0, 1 };

    static_assert( crimild::isEqual( k, crimild::cross( i, j ) ) );
    static_assert( crimild::isEqual( i, crimild::cross( j, k ) ) );
    static_assert( crimild::isEqual( j, crimild::cross( k, i ) ) );

    EXPECT_TRUE( true );
}

TEST( Vector3, length )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 4 };

    EXPECT_EQ( real_t( 29 ), crimild::length2( u ) );
    EXPECT_EQ( real_t( 5.3851648071 ), crimild::length( u ) );
}

TEST( Vector3, normalize )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 4 };
    constexpr auto v = crimild::Vector3 {
        0.3713906764,
        0.5570860145,
        0.7427813527,
    };

    static_assert( crimild::isEqual( v, crimild::normalize( u ) ) );

    EXPECT_TRUE( crimild::isEqual( real_t( 1 ), crimild::length2( crimild::normalize( u ) ) ) );
    EXPECT_TRUE( crimild::isEqual( real_t( 1 ), crimild::length( crimild::normalize( u ) ) ) );
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

    static_assert( 2 == crimild::min( u ) );
    static_assert( crimild::isEqual( m, crimild::min( u, v ) ) );

    EXPECT_TRUE( true );
}

TEST( Vector3, max )
{
    constexpr auto u = crimild::Vector3 { 2, 3, 4 };
    constexpr auto v = crimild::Vector3 { 1, 5, 2 };
    constexpr auto m = crimild::Vector3 { 2, 5, 4 };

    static_assert( 4 == crimild::max( u ) );
    static_assert( crimild::isEqual( m, crimild::max( u, v ) ) );

    EXPECT_TRUE( true );
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

    crimild::orthonormalBasis( normalize( v1 ), v2, v3 );

    EXPECT_TRUE( isEqual( length( v2 ), 1 ) );
    EXPECT_TRUE( isEqual( length( v3 ), 1 ) );

    EXPECT_TRUE( crimild::isZero( crimild::dot( v1, v2 ) ) );
    EXPECT_TRUE( crimild::isZero( crimild::dot( v1, v3 ) ) );
    EXPECT_TRUE( crimild::isZero( crimild::dot( v2, v3 ) ) );
}

TEST( Vector3, permute )
{
    constexpr auto u = crimild::Vector3 { 4, 5, 6 };
    constexpr auto v = crimild::Vector3 { 6, 4, 5 };

    static_assert( crimild::isEqual( v, crimild::permute( u, 2, 0, 1 ) ) );

    EXPECT_TRUE( true );
}
