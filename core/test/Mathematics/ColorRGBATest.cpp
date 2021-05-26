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

#include "Mathematics/ColorRGBA.hpp"

#include "Mathematics/ColorRGBAOps.hpp"
#include "Mathematics/dot.hpp"
#include "Mathematics/io.hpp"
#include "Mathematics/isEqual.hpp"
#include "Mathematics/isNaN.hpp"
#include "Mathematics/max.hpp"
#include "Mathematics/min.hpp"
#include "Mathematics/permutation.hpp"

#include "gtest/gtest.h"
#include <sstream>

TEST( ColorRGBA, construction )
{
    constexpr auto u = crimild::ColorRGBA { 10, 20, 30, 40 };

    EXPECT_EQ( 10, u.r );
    EXPECT_EQ( 20, u.g );
    EXPECT_EQ( 30, u.b );
    EXPECT_EQ( 40, u.a );
}

TEST( ColorRGBA, index )
{
    constexpr auto u = crimild::ColorRGBA { 10, 20, 30, 40 };

    EXPECT_EQ( 10, u[ 0 ] );
    EXPECT_EQ( 20, u[ 1 ] );
    EXPECT_EQ( 30, u[ 2 ] );
    EXPECT_EQ( 40, u[ 3 ] );
}

TEST( ColorRGBA, equality )
{
    constexpr auto u = crimild::ColorRGBA { 10, 20, 30, 40 };
    constexpr auto v = crimild::ColorRGBA { 30, 40, 50, 60 };
    constexpr auto w = crimild::ColorRGBA { 10, 20, 30, 40 };

    EXPECT_TRUE( crimild::isEqual( u, w ) );
    EXPECT_FALSE( crimild::isEqual( u, v ) );
    EXPECT_FALSE( crimild::isEqual( v, w ) );
}

TEST( ColorRGBA, addition )
{
    constexpr auto u = crimild::ColorRGBA { 10, 20, 30, 40 };
    constexpr auto v = crimild::ColorRGBA { 30, 40, 50, 60 };
    constexpr auto res = crimild::ColorRGBA { 40, 60, 80, 100 };

    static_assert( crimild::isEqual( res, u + v ) );

    EXPECT_TRUE( true );
}

TEST( ColorRGBA, subtraction )
{
    constexpr auto u = crimild::ColorRGBA { 10, 20, 30, 40 };
    constexpr auto v = crimild::ColorRGBA { 30, 40, 50, 60 };
    constexpr auto res = crimild::ColorRGBA { -20, -20, -20, -20 };

    static_assert( crimild::isEqual( res, u - v ) );

    EXPECT_TRUE( true );
}

TEST( ColorRGBA, multiplication )
{
    constexpr auto u = crimild::ColorRGBA { 10, 20, 30, 40 };
    constexpr auto s = crimild::Real( 5 );
    constexpr auto res = crimild::ColorRGBA { 50, 100, 150, 200 };

    EXPECT_TRUE( crimild::isEqual( res, u * s ) );
    EXPECT_TRUE( crimild::isEqual( res, s * u ) );
}

TEST( ColorRGBA, vectorMultiplication )
{
    constexpr auto u = crimild::ColorRGBA { 10, 20, 30, 40 };
    constexpr auto v = crimild::ColorRGBA { 2, 3, 4, 5 };
    constexpr auto res = crimild::ColorRGBA { 20, 60, 120, 200 };

    EXPECT_TRUE( crimild::isEqual( res, u * v ) );
}

TEST( ColorRGBA, division )
{
    constexpr auto u = crimild::ColorRGBA { 10, 20, 30, 40 };
    constexpr auto s = crimild::Real( 2 );
    constexpr auto res = crimild::ColorRGBA { 5, 10, 15, 20 };

    EXPECT_TRUE( crimild::isEqual( res, u / s ) );
}

TEST( ColorRGBA, isNaN )
{
    constexpr auto u = crimild::ColorRGBA { 10, 20, 30, 40 };
    constexpr auto v = crimild::ColorRGBA { NAN, NAN, NAN, NAN };
    constexpr auto w = crimild::ColorRGBA { NAN, NAN, 0, NAN };

    EXPECT_TRUE( crimild::isNaN( v ) );
    EXPECT_FALSE( crimild::isNaN( u ) );
    EXPECT_TRUE( crimild::isNaN( w ) );
}

TEST( ColorRGBA, dot )
{
    constexpr auto u = crimild::ColorRGBA { 2, 3, 5, 10 };
    constexpr auto v = crimild::ColorRGBA { 8, 10, -2, -1 };

    EXPECT_EQ( 26, crimild::dot( u, v ) );
}

TEST( ColorRGBA, min )
{
    constexpr auto u = crimild::ColorRGBA { 2, 3, 4, 1 };
    constexpr auto v = crimild::ColorRGBA { 1, 5, 2, 10 };
    constexpr auto m = crimild::ColorRGBA { 1, 3, 2, 1 };

    static_assert( 1 == crimild::min( u ) );
    static_assert( crimild::isEqual( m, crimild::min( u, v ) ) );

    EXPECT_TRUE( true );
}

TEST( ColorRGBA, max )
{
    constexpr auto u = crimild::ColorRGBA { 2, 3, 4, 1 };
    constexpr auto v = crimild::ColorRGBA { 1, 5, 2, 10 };
    constexpr auto m = crimild::ColorRGBA { 2, 5, 4, 10 };

    static_assert( 4 == crimild::max( u ) );
    static_assert( crimild::isEqual( m, crimild::max( u, v ) ) );

    EXPECT_TRUE( true );
}

TEST( ColorRGBA, minDimension )
{
    constexpr auto u = crimild::ColorRGBA { 2, 3, 4, 1 };

    EXPECT_EQ( 3, crimild::minDimension( u ) );
}

TEST( ColorRGBA, maxDimension )
{
    constexpr auto u = crimild::ColorRGBA { 2, 3, 4, 1 };

    static_assert( 2 == crimild::maxDimension( crimild::ColorRGBA { 2, 3, 4, 2 } ), "maxDimension" );

    EXPECT_TRUE( true );
}

TEST( ColorRGBA, ostream )
{
    constexpr auto u = crimild::ColorRGBA { 2, 3, 4, 1 };

    std::stringstream ss;
    ss << u;

    EXPECT_EQ( ss.str(), "(2.000000, 3.000000, 4.000000, 1.000000)" );
}

TEST( ColorRGBA, permute )
{
    static_assert( crimild::isEqual( crimild::ColorRGBA { 6, 4, 1, 5 }, crimild::permute( crimild::ColorRGBA { 4, 5, 6, 1 }, 2, 0, 3, 1 ) ) );
    EXPECT_TRUE( true );
}

TEST( ColorRGBA, constexpr )
{
    constexpr auto u = crimild::ColorRGBA { 10, 20, 30, 40 };
    constexpr auto v = crimild::ColorRGBA { 30, 40, 50, 60 };
    constexpr auto w = crimild::ColorRGBA { 10, 20, 30, 40 };
    constexpr auto s = crimild::Real( 5 );

    static_assert( crimild::isEqual( u, w ), "equality" );
    static_assert( !crimild::isEqual( u, v ), "inequality" );

    static_assert( 10 == u[ 0 ], "index" );
    static_assert( 20 == u[ 1 ], "index" );
    static_assert( 30 == u[ 2 ], "index" );

    static_assert( crimild::isEqual( crimild::ColorRGBA { 40, 60, 80, 100 }, ( u + v ) ), "addition" );
    static_assert( crimild::isEqual( crimild::ColorRGBA { -20, -20, -20, -20 }, ( u - v ) ), "subtraction" );

    static_assert( crimild::isEqual( crimild::ColorRGBA { 50, 100, 150, 200 }, ( u * s ) ), "multiplication" );
    static_assert( crimild::isEqual( crimild::ColorRGBA { 50, 100, 150, 200 }, ( s * u ) ), "multiplication" );
    static_assert( crimild::isEqual( crimild::ColorRGBA { 300, 800, 1500, 2400 }, ( u * v ) ), "multiplication" );
    static_assert( crimild::isEqual( crimild::ColorRGBA { 2, 4, 6, 8 }, ( u / s ) ), "division" );

    static_assert( crimild::isNaN( u ) == false, "isNaN" );
    static_assert( crimild::isNaN( crimild::ColorRGBA { NAN, NAN, NAN, NAN } ) == true, "isNaN" );

    static_assert( 42 == crimild::dot( crimild::ColorRGBA { 2, 3, 5, 6 }, crimild::ColorRGBA { 8, 10, -2, 1 } ), "dot" );

    static_assert( crimild::isEqual( crimild::ColorRGBA { 1, 3, 2, 0 }, crimild::min( crimild::ColorRGBA { 2, 3, 4, 0 }, crimild::ColorRGBA { 1, 5, 2, 1 } ) ), "min" );
    static_assert( 2 == crimild::min( crimild::ColorRGBA { 2, 3, 4, 3 } ), "min" );
    static_assert( 0 == crimild::minDimension( crimild::ColorRGBA { 2, 3, 4, 3 } ), "minDimension" );

    EXPECT_TRUE( true );
}
