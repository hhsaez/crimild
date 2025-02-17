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

#include "Normal3.hpp"

#include "Vector3.hpp"
#include "Vector4.hpp"
#include "dot.hpp"
#include "io.hpp"
#include "isEqual.hpp"
#include "isNaN.hpp"
#include "length.hpp"
#include "max.hpp"
#include "min.hpp"
#include "normalize.hpp"
#include "swizzle.hpp"

#include "gtest/gtest.h"
#include <sstream>

using namespace crimild;

TEST( Normal3, construction )
{
    constexpr auto u = crimild::Normal3 {
        10,
        20,
        30,
    };

    EXPECT_EQ( 10, u.x );
    EXPECT_EQ( 20, u.y );
    EXPECT_EQ( 30, u.z );
}

TEST( Normal3, index )
{
    constexpr auto u = crimild::Normal3 { 10, 20, 30 };

    static_assert( crimild::isEqual( real_t( 10 ), u[ 0 ] ) );
    static_assert( crimild::isEqual( real_t( 20 ), u[ 1 ] ) );
    static_assert( crimild::isEqual( real_t( 30 ), u[ 2 ] ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, equality )
{
    constexpr auto u = crimild::Normal3 { 10, 20, 30 };
    constexpr auto v = crimild::Normal3 { 30, 40, 50 };
    constexpr auto w = crimild::Normal3 { 10, 20, 30 };

    static_assert( crimild::isEqual( u, w ) );
    static_assert( !crimild::isEqual( u, v ) );
    static_assert( !crimild::isEqual( v, w ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, addition )
{
    constexpr auto u = crimild::Normal3 { 10, 20, 30 };
    constexpr auto v = crimild::Normal3 { 30, 40, 50 };
    constexpr auto res = crimild::Normal3 { 40, 60, 80 };

    static_assert( crimild::isEqual( res, u + v ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, subtraction )
{
    constexpr auto u = crimild::Normal3 { 10, 20, 30 };
    constexpr auto v = crimild::Normal3 { 30, 40, 50 };
    constexpr auto res = crimild::Normal3 { -20, -20, -20 };

    static_assert( crimild::isEqual( res, u - v ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, multiplication )
{
    constexpr auto u = crimild::Normal3 { 10, 20, 30 };
    constexpr auto s = real_t( 5 );
    constexpr auto res = crimild::Normal3 { 50, 100, 150 };

    static_assert( crimild::isEqual( res, u * s ) );
    static_assert( crimild::isEqual( res, s * u ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, division )
{
    constexpr auto u = crimild::Normal3 { 10, 20, 30 };
    constexpr auto s = real_t( 2 );
    constexpr auto res = crimild::Normal3 { 5, 10, 15 };

    static_assert( crimild::isEqual( res, u / s ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, negation )
{
    constexpr auto u = crimild::Normal3 { 10, 20, 30 };
    constexpr auto res = crimild::Normal3 { -10, -20, -30 };

    static_assert( crimild::isEqual( res, -u ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, isNaN )
{
    constexpr auto u = crimild::Normal3 { 10, 20, 30 };
    constexpr auto v = crimild::Normal3 { NAN, NAN, NAN };
    constexpr auto w = crimild::Normal3 { NAN, NAN, 0 };

    EXPECT_FALSE( crimild::isNaN( u ) );
    EXPECT_TRUE( crimild::isNaN( v ) );
    EXPECT_TRUE( crimild::isNaN( w ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, abs )
{
    constexpr auto u = crimild::Normal3 { -10, -20, -30 };
    constexpr auto res = crimild::Normal3 { 10, 20, 30 };

    static_assert( crimild::isEqual( res, crimild::abs( u ) ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, dot )
{
    constexpr auto u = crimild::Normal3 { 2, 3, 5 };
    constexpr auto v = crimild::Normal3 { 8, 10, -2 };

    EXPECT_EQ( 36, crimild::dot( u, v ) );
}

TEST( Normal3, length )
{
    constexpr auto u = crimild::Normal3 { 2, 3, 4 };

    EXPECT_EQ( real_t( 29 ), crimild::length2( u ) );
    EXPECT_EQ( real_t( 5.3851648071 ), crimild::length( u ) );
}

TEST( Normal3, normalize )
{
    constexpr auto u = crimild::Normal3 { 2, 3, 4 };
    constexpr auto v = crimild::Normal3 {
        0.3713906764,
        0.5570860145,
        0.7427813527,
    };

    static_assert( crimild::isEqual( v, crimild::normalize( u ) ) );

    EXPECT_TRUE( crimild::isEqual( real_t( 1 ), crimild::length2( crimild::normalize( u ) ) ) );
    EXPECT_TRUE( crimild::isEqual( real_t( 1 ), crimild::length( crimild::normalize( u ) ) ) );
}

TEST( Normal3, normalizeNaN )
{
    EXPECT_TRUE( crimild::isNaN( crimild::normalize( crimild::Normal3 {} ) ) );
}

TEST( Normal3, min )
{
    constexpr auto u = crimild::Normal3 { 2, 3, 4 };
    constexpr auto v = crimild::Normal3 { 1, 5, 2 };
    constexpr auto m = crimild::Normal3 { 1, 3, 2 };

    static_assert( 2 == crimild::min( u ) );

    static_assert( crimild::isEqual( m, crimild::min( u, v ) ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, max )
{
    constexpr auto u = crimild::Normal3 { 2, 3, 4 };
    constexpr auto v = crimild::Normal3 { 1, 5, 2 };
    constexpr auto m = crimild::Normal3 { 2, 5, 4 };

    static_assert( 4 == crimild::max( u ) );

    static_assert( crimild::isEqual( m, crimild::max( u, v ) ) );
}

TEST( Normal3, minDimension )
{
    constexpr auto u = crimild::Normal3 { 2, 3, 4 };

    EXPECT_EQ( 0, crimild::minDimension( u ) );
}

TEST( Normal3, maxDimension )
{
    constexpr auto u = crimild::Normal3 { 2, 3, 4 };

    EXPECT_EQ( 2, crimild::maxDimension( u ) );
}

TEST( Normal3, ostream )
{
    constexpr auto u = crimild::Normal3 { 2, 3, 4 };

    std::stringstream ss;
    ss << u;

    EXPECT_EQ( ss.str(), "(2.000000, 3.000000, 4.000000)" );
}

TEST( Normal3, conversion )
{
    constexpr auto N = crimild::Normal3 { 1, 2, 3 };
    [[maybe_unused]] constexpr auto V = crimild::Vector3( N );
    constexpr auto V4 = crimild::Vector4f( N );

    static_assert( crimild::isEqual( V4, crimild::Vector4 { 1, 2, 3, 0 } ) );

    constexpr auto N3 = crimild::normalize( crimild::Normal3( V4 ) );

    EXPECT_TRUE( true );
}

TEST( Normal3, constexpr )
{
    constexpr auto u = crimild::Normal3 { 10, 20, 30 };
    [[maybe_unused]] constexpr auto v = crimild::Normal3 { 30, 40, 50 };
    [[maybe_unused]] constexpr auto w = crimild::Normal3 { 10, 20, 30 };
    [[maybe_unused]] constexpr auto s = real_t( 5 );

    static_assert( 10 == u[ 0 ], "index" );
    static_assert( 20 == u[ 1 ], "index" );
    static_assert( 30 == u[ 2 ], "index" );

    static_assert( crimild::isNaN( u ) == false, "isNaN" );

    EXPECT_TRUE( true );
}
