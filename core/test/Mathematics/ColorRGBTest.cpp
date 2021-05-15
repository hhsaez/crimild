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

#include "Mathematics/ColorRGB.hpp"

#include "gtest/gtest.h"
#include <sstream>

TEST( ColorRGB, construction )
{
    constexpr auto u = crimild::ColorRGB( 10, 20, 30 );

    EXPECT_EQ( 10, u.r() );
    EXPECT_EQ( 20, u.g() );
    EXPECT_EQ( 30, u.b() );
}

TEST( ColorRGB, index )
{
    constexpr auto u = crimild::ColorRGB { 10, 20, 30 };

    EXPECT_EQ( 10, u[ 0 ] );
    EXPECT_EQ( 20, u[ 1 ] );
    EXPECT_EQ( 30, u[ 2 ] );
}

TEST( ColorRGB, equality )
{
    constexpr auto u = crimild::ColorRGB { 10, 20, 30 };
    constexpr auto v = crimild::ColorRGB { 30, 40, 50 };
    constexpr auto w = crimild::ColorRGB { 10, 20, 30 };

    EXPECT_EQ( u, w );
    EXPECT_NE( u, v );
    EXPECT_NE( v, w );
}

TEST( ColorRGB, addition )
{
    constexpr auto u = crimild::ColorRGB { 10, 20, 30 };
    constexpr auto v = crimild::ColorRGB { 30, 40, 50 };
    constexpr auto res = crimild::ColorRGB { 40, 60, 80 };

    EXPECT_EQ( res, u + v );
}

TEST( ColorRGB, subtraction )
{
    constexpr auto u = crimild::ColorRGB { 10, 20, 30 };
    constexpr auto v = crimild::ColorRGB { 30, 40, 50 };
    constexpr auto res = crimild::ColorRGB { -20, -20, -20 };

    EXPECT_EQ( res, u - v );
}

TEST( ColorRGB, multiplication )
{
    constexpr auto u = crimild::ColorRGB { 10, 20, 30 };
    constexpr auto s = crimild::Real( 5 );
    constexpr auto res = crimild::ColorRGB { 50, 100, 150 };

    EXPECT_EQ( res, u * s );
    EXPECT_EQ( res, s * u );
}

TEST( ColorRGB, vectorMultiplication )
{
    constexpr auto u = crimild::ColorRGB { 10, 20, 30 };
    constexpr auto v = crimild::ColorRGB { 2, 3, 4 };
    constexpr auto res = crimild::ColorRGB { 20, 60, 120 };

    EXPECT_EQ( res, u * v );
}

TEST( ColorRGB, division )
{
    constexpr auto u = crimild::ColorRGB { 10, 20, 30 };
    constexpr auto s = crimild::Real( 2 );
    constexpr auto res = crimild::ColorRGB { 5, 10, 15 };

    EXPECT_EQ( res, u / s );
}

TEST( ColorRGB, isNaN )
{
    constexpr auto u = crimild::ColorRGB { 10, 20, 30 };
    constexpr auto v = crimild::ColorRGB { NAN, NAN, NAN };
    constexpr auto w = crimild::ColorRGB { NAN, NAN, 0 };

    EXPECT_TRUE( crimild::isNaN( v ) );
    EXPECT_FALSE( crimild::isNaN( u ) );
    EXPECT_TRUE( crimild::isNaN( w ) );
}

TEST( ColorRGB, dot )
{
    constexpr auto u = crimild::ColorRGB { 2, 3, 5 };
    constexpr auto v = crimild::ColorRGB { 8, 10, -2 };

    EXPECT_EQ( 36, crimild::dot( u, v ) );
}

TEST( ColorRGB, min )
{
    constexpr auto u = crimild::ColorRGB { 2, 3, 4 };
    constexpr auto v = crimild::ColorRGB { 1, 5, 2 };
    constexpr auto m = crimild::ColorRGB { 1, 3, 2 };

    EXPECT_EQ( 2, crimild::min( u ) );
    EXPECT_EQ( m, crimild::min( u, v ) );
}

TEST( ColorRGB, max )
{
    constexpr auto u = crimild::ColorRGB { 2, 3, 4 };
    constexpr auto v = crimild::ColorRGB { 1, 5, 2 };
    constexpr auto m = crimild::ColorRGB { 2, 5, 4 };

    EXPECT_EQ( 4, crimild::max( u ) );
    EXPECT_EQ( m, crimild::max( u, v ) );
}

TEST( ColorRGB, minDimension )
{
    constexpr auto u = crimild::ColorRGB { 2, 3, 4 };

    EXPECT_EQ( 0, crimild::minDimension( u ) );
}

TEST( ColorRGB, maxDimension )
{
    constexpr auto u = crimild::ColorRGB { 2, 3, 4 };

    EXPECT_EQ( 2, crimild::maxDimension( u ) );
}

TEST( ColorRGB, ostream )
{
    constexpr auto u = crimild::ColorRGB { 2, 3, 4 };

    std::stringstream ss;
    ss << u;

    EXPECT_EQ( ss.str(), "(2.000000, 3.000000, 4.000000)" );
}

TEST( ColorRGB, permute )
{
    constexpr auto u = crimild::ColorRGB { 4, 5, 6 };
    constexpr auto v = crimild::ColorRGB { 6, 4, 5 };

    EXPECT_EQ( v, crimild::permute( u, 2, 0, 1 ) );
}

TEST( ColorRGB, constexpr )
{
    constexpr auto u = crimild::ColorRGB { 10, 20, 30 };
    constexpr auto v = crimild::ColorRGB { 30, 40, 50 };
    constexpr auto w = crimild::ColorRGB { 10, 20, 30 };
    constexpr auto s = crimild::Real( 5 );

    static_assert( u == w, "equality" );
    static_assert( u != v, "inequality" );

    static_assert( 10 == u[ 0 ], "index" );
    static_assert( 20 == u[ 1 ], "index" );
    static_assert( 30 == u[ 2 ], "index" );

    static_assert( crimild::ColorRGB { 40, 60, 80 } == ( u + v ), "addition" );
    static_assert( crimild::ColorRGB { -20, -20, -20 } == ( u - v ), "subtraction" );

    static_assert( crimild::ColorRGB { 50, 100, 150 } == ( u * s ), "multiplication" );
    static_assert( crimild::ColorRGB { 50, 100, 150 } == ( s * u ), "multiplication" );
    static_assert( crimild::ColorRGB { 300, 800, 1500 } == ( u * v ), "multiplication" );
    static_assert( crimild::ColorRGB { 2, 4, 6 } == ( u / s ), "division" );

    static_assert( crimild::isNaN( u ) == false, "isNaN" );
    static_assert( crimild::isNaN( crimild::ColorRGB { NAN, NAN, NAN } ) == true, "isNaN" );

    static_assert( 36 == crimild::dot( crimild::ColorRGB { 2, 3, 5 }, crimild::ColorRGB { 8, 10, -2 } ), "dot" );

    static_assert( crimild::ColorRGB { 1, 3, 2 } == crimild::min( crimild::ColorRGB { 2, 3, 4 }, crimild::ColorRGB { 1, 5, 2 } ), "min" );
    static_assert( 2 == crimild::min( crimild::ColorRGB { 2, 3, 4 } ), "min" );
    static_assert( 0 == crimild::minDimension( crimild::ColorRGB { 2, 3, 4 } ), "minDimension" );

    static_assert( crimild::ColorRGB { 2, 5, 4 } == crimild::max( crimild::ColorRGB { 2, 3, 4 }, crimild::ColorRGB { 1, 5, 2 } ), "max" );
    static_assert( 4 == crimild::max( crimild::ColorRGB { 2, 3, 4 } ), "max" );
    static_assert( 2 == crimild::maxDimension( crimild::ColorRGB { 2, 3, 4 } ), "maxDimension" );

    EXPECT_TRUE( true );
}
