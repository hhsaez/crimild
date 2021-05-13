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

#include "Mathematics/Vector2.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( Vector2, construction )
{
    constexpr auto u = Vector2 { 10, 20 };

    EXPECT_EQ( 10, u.x() );
    EXPECT_EQ( 20, u.y() );
}

TEST( Vector2, index )
{
    constexpr auto u = Vector2f { 10, 20 };

    EXPECT_EQ( 10, u[ 0 ] );
    EXPECT_EQ( 20, u[ 1 ] );
}

TEST( Vector2, equality )
{
    constexpr auto u = Vector2 { 10, 20 };
    constexpr auto v = Vector2 { 30, 40 };
    constexpr auto w = Vector2 { 10, 20 };

    EXPECT_EQ( u, w );
    EXPECT_NE( u, v );
    EXPECT_NE( v, w );
}

TEST( Vector2, addition )
{
    constexpr auto u = Vector2 { 10, 20 };
    constexpr auto v = Vector2 { 30, 40 };
    constexpr auto res = Vector2 { 40, 60 };

    EXPECT_EQ( res, u + v );
}

TEST( Vector2, additionWithDifferentTypes )
{
    constexpr auto u = Vector2f { 10, 20 };
    constexpr auto v = Vector2i { 30, 40 };
    constexpr auto w = u + v;

    EXPECT_EQ( 40, w.x() );
    EXPECT_EQ( 60, w.y() );
}

TEST( Vector2, subtraction )
{
    constexpr auto u = Vector2 { 10, 20 };
    constexpr auto v = Vector2 { 30, 40 };
    constexpr auto res = Vector2 { -20, -20 };

    EXPECT_EQ( res, u - v );
}

TEST( Vector2, multiplication )
{
    constexpr auto u = Vector2 { 10, 20 };
    constexpr auto s = Real( 5 );
    constexpr auto res = Vector2 { 50, 100 };

    EXPECT_EQ( res, u * s );
    EXPECT_EQ( res, s * u );
}

TEST( Vector2, vectorMultiplication )
{
    constexpr auto u = Vector2 { 10, 20 };
    constexpr auto v = Vector2 { 2, 3 };
    constexpr auto res = Vector2 { 20, 60 };

    EXPECT_EQ( res, u * v );
}

TEST( Vector2, division )
{
    constexpr auto u = Vector2 { 10, 20 };
    constexpr auto s = Real( 2 );
    constexpr auto res = Vector2 { 5, 10 };

    EXPECT_EQ( res, u / s );
}

TEST( Vector2, negation )
{
    constexpr auto u = Vector2 { 10, 20 };
    constexpr auto res = Vector2 { -10, -20 };

    EXPECT_EQ( res, -u );
}

TEST( Vector2, isNaN )
{
    constexpr auto u = Vector2 { 10, 20 };
    constexpr auto v = Vector2 { NAN, NAN };

    EXPECT_TRUE( isNaN( v ) );
    EXPECT_FALSE( isNaN( u ) );
}

TEST( Vector2, abs )
{
    constexpr auto u = Vector2 { -10, -20 };
    constexpr auto res = Vector2 { 10, 20 };

    EXPECT_EQ( res, abs( u ) );
}

TEST( Vector2, dot )
{
    constexpr auto u = Vector2 { 2, 3 };
    constexpr auto v = Vector2 { 8, 10 };

    EXPECT_EQ( 46, dot( u, v ) );
}

TEST( Vector2, absDot )
{
    constexpr auto u = Vector2 { 2, 3 };
    constexpr auto v = Vector2 { -8, -10 };

    EXPECT_EQ( 46, absDot( u, v ) );
}

TEST( Vector2, constexpr )
{
    constexpr auto u = Vector2 { 10, 20 };
    constexpr auto v = Vector2 { 30, 40 };
    constexpr auto w = Vector2 { 10, 20 };
    constexpr auto s = Real( 5 );

    static_assert( u == w, "equality" );
    static_assert( u != v, "inequality" );

    static_assert( 10 == u[ 0 ], "index" );
    static_assert( 20 == u[ 1 ], "index" );

    static_assert( Vector2 { 40, 60 } == ( u + v ), "addition" );
    static_assert( Vector2 { -20, -20 } == ( u - v ), "subtraction" );

    static_assert( Vector2 { 50, 100 } == ( u * s ), "multiplication" );
    static_assert( Vector2 { 50, 100 } == ( s * u ), "multiplication" );
    static_assert( Vector2 { 300, 800 } == ( u * v ), "multiplication" );
    static_assert( Vector2 { 2, 4 } == ( u / s ), "division" );

    static_assert( Vector2 { -10, -20 } == -u, "negation" );

    static_assert( isNaN( u ) == false, "isNaN" );
    static_assert( isNaN( Vector2 { NAN, NAN } ) == true, "isNaN" );

    static_assert( Vector2 { 10, 20 } == abs( Vector2 { -10, -20 } ), "abs" );

    static_assert( 46 == dot( Vector2 { 2, 3 }, Vector2 { 8, 10 } ), "dot" );
    static_assert( 46 == absDot( Vector2 { 2, 3 }, Vector2 { -8, -10 } ), "absDot" );

    EXPECT_TRUE( true );
}
