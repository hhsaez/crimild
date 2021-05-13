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

#include "Mathematics/Vector4.hpp"

#include "gtest/gtest.h"
#include <sstream>

TEST( Vector4, construction )
{
    constexpr auto u = crimild::Vector4( 10, 20, 30, 40 );

    EXPECT_EQ( 10, u.x() );
    EXPECT_EQ( 20, u.y() );
    EXPECT_EQ( 30, u.z() );
    EXPECT_EQ( 40, u.w() );
}

TEST( Vector4, index )
{
    constexpr auto u = crimild::Vector4( 10, 20, 30, 40 );

    EXPECT_EQ( 10, u[ 0 ] );
    EXPECT_EQ( 20, u[ 1 ] );
    EXPECT_EQ( 30, u[ 2 ] );
    EXPECT_EQ( 40, u[ 3 ] );
}

TEST( Vector4, equality )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto v = crimild::Vector4 { 30, 40, 50, 60 };
    constexpr auto w = crimild::Vector4 { 10, 20, 30, 40 };

    EXPECT_EQ( u, w );
    EXPECT_NE( u, v );
    EXPECT_NE( v, w );
}

TEST( Vector4, addition )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto v = crimild::Vector4 { 30, 40, 50, 60 };
    constexpr auto res = crimild::Vector4 { 40, 60, 80, 100 };

    EXPECT_EQ( res, u + v );
}

TEST( Vector4, subtraction )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto v = crimild::Vector4 { 30, 40, 50, 60 };
    constexpr auto res = crimild::Vector4 { -20, -20, -20, -20 };

    EXPECT_EQ( res, u - v );
}

TEST( Vector4, multiplication )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto s = crimild::Real( 5 );
    constexpr auto res = crimild::Vector4 { 50, 100, 150, 200 };

    EXPECT_EQ( res, u * s );
    EXPECT_EQ( res, s * u );
}

TEST( Vector4, vectorMultiplication )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto v = crimild::Vector4 { 2, 3, 4, 5 };
    constexpr auto res = crimild::Vector4 { 20, 60, 120, 200 };

    EXPECT_EQ( res, u * v );
}

TEST( Vector4, division )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto s = crimild::Real( 2 );
    constexpr auto res = crimild::Vector4 { 5, 10, 15, 20 };

    EXPECT_EQ( res, u / s );
}

TEST( Vector4, negation )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto res = crimild::Vector4 { -10, -20, -30, -40 };

    EXPECT_EQ( res, -u );
}

TEST( Vector4, isNaN )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto v = crimild::Vector4 { NAN, NAN, NAN, NAN };
    constexpr auto w = crimild::Vector4 { NAN, NAN, 0, NAN };

    EXPECT_TRUE( crimild::isNaN( v ) );
    EXPECT_FALSE( crimild::isNaN( u ) );
    EXPECT_TRUE( crimild::isNaN( w ) );
}

TEST( Vector4, abs )
{
    constexpr auto u = crimild::Vector4 { -10, -20, -30, -40 };
    constexpr auto res = crimild::Vector4 { 10, 20, 30, 40 };

    EXPECT_EQ( res, crimild::abs( u ) );
}

TEST( Vector4, dot )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 5, 10 };
    constexpr auto v = crimild::Vector4 { 8, 10, -2, -1 };

    EXPECT_EQ( 26, crimild::dot( u, v ) );
}

TEST( Vector4, absDot )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 5, 10 };
    constexpr auto v = crimild::Vector4 { -8, -10, -2, -1 };

    EXPECT_EQ( 66, crimild::absDot( u, v ) );
}

TEST( Vector4, length )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 4, 5 };

    EXPECT_EQ( crimild::Real( 54 ), crimild::lengthSquared( u ) );
    EXPECT_EQ( crimild::Real( 7.3484692283 ), crimild::length( u ) );
}

TEST( Vector4, normalize )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 4, 5 };
    constexpr auto v = crimild::Vector4( 0.272165527, 0.4082482905, 0.544331054, 0.6804138174 );

    EXPECT_EQ( v, crimild::normalize( u ) );
    EXPECT_TRUE( crimild::isEqual( crimild::Real( 1 ), crimild::lengthSquared( crimild::normalize( u ) ) ) );
    EXPECT_TRUE( crimild::isEqual( crimild::Real( 1 ), crimild::length( crimild::normalize( u ) ) ) );
}

TEST( Vector4, normalizeNaN )
{
    EXPECT_TRUE( crimild::isNaN( crimild::normalize( crimild::Vector4 {} ) ) );
}

TEST( Vector4, min )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 4, 1 };
    constexpr auto v = crimild::Vector4 { 1, 5, 2, 10 };
    constexpr auto m = crimild::Vector4 { 1, 3, 2, 1 };

    EXPECT_EQ( 1, crimild::min( u ) );
    EXPECT_EQ( m, crimild::min( u, v ) );
}

TEST( Vector4, max )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 4, 1 };
    constexpr auto v = crimild::Vector4 { 1, 5, 2, 10 };
    constexpr auto m = crimild::Vector4 { 2, 5, 4, 10 };

    EXPECT_EQ( 4, crimild::max( u ) );
    EXPECT_EQ( m, crimild::max( u, v ) );
}

TEST( Vector4, minDimension )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 4, 1 };

    EXPECT_EQ( 3, crimild::minDimension( u ) );
}

TEST( Vector4, maxDimension )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 4, 1 };

    EXPECT_EQ( 2, crimild::maxDimension( u ) );
}

TEST( Vector4, ostream )
{
    {
        constexpr auto u = crimild::Vector4 { 2, 3, 4, 1 };

        std::stringstream ss;
        ss << u;

        EXPECT_EQ( ss.str(), "(2.000000, 3.000000, 4.000000, 1.000000)" );
    }

    {
        constexpr auto u = crimild::Vector4i { 2, 3, 4, 1 };

        std::stringstream ss;
        ss << u;

        EXPECT_EQ( ss.str(), "(2, 3, 4, 1)" );
    }
}

TEST( Vector4, permute )
{
    static_assert( crimild::Vector4( 6, 4, 1, 5 ) == crimild::permute( crimild::Vector4( 4, 5, 6, 1 ), 2, 0, 3, 1 ) );
    EXPECT_TRUE( true );
}

TEST( Vector4, constexpr )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto v = crimild::Vector4 { 30, 40, 50, 60 };
    constexpr auto w = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto s = crimild::Real( 5 );

    static_assert( u == w, "equality" );
    static_assert( u != v, "inequality" );

    static_assert( 10 == u[ 0 ], "index" );
    static_assert( 20 == u[ 1 ], "index" );
    static_assert( 30 == u[ 2 ], "index" );

    static_assert( crimild::Vector4( 40, 60, 80, 100 ) == ( u + v ), "addition" );
    static_assert( crimild::Vector4 { -20, -20, -20, -20 } == ( u - v ), "subtraction" );

    static_assert( crimild::Vector4 { 50, 100, 150, 200 } == ( u * s ), "multiplication" );
    static_assert( crimild::Vector4 { 50, 100, 150, 200 } == ( s * u ), "multiplication" );
    static_assert( crimild::Vector4 { 300, 800, 1500, 2400 } == ( u * v ), "multiplication" );
    static_assert( crimild::Vector4 { 2, 4, 6, 8 } == ( u / s ), "division" );

    static_assert( crimild::Vector4 { -10, -20, -30, -40 } == -u, "negation" );

    static_assert( crimild::isNaN( u ) == false, "isNaN" );
    static_assert( crimild::isNaN( crimild::Vector4( NAN, NAN, NAN, NAN ) ) == true, "isNaN" );

    static_assert( crimild::Vector4 { 10, 20, 30, 40 } == crimild::abs( crimild::Vector4 { -10, -20, -30, -40 } ), "abs" );

    static_assert( 42 == crimild::dot( crimild::Vector4 { 2, 3, 5, 6 }, crimild::Vector4 { 8, 10, -2, 1 } ), "dot" );
    static_assert( 62 == crimild::absDot( crimild::Vector4 { 2, 3, 5, 6 }, crimild::Vector4 { -8, -10, -2, -1 } ), "absDot" );

    static_assert( crimild::Real( 54 ) == crimild::lengthSquared( crimild::Vector4 { 2, 3, 4, 5 } ), "lengthSquared" );
    static_assert( crimild::Real( 7.3484692283 ) == crimild::length( crimild::Vector4 { 2, 3, 4, 5 } ), "length" );

    static_assert( crimild::Vector4 { 0.272165527, 0.4082482905, 0.544331054, 0.6804138174 } == crimild::normalize( crimild::Vector4 { 2, 3, 4, 5 } ), "normalize" );
    static_assert( crimild::isEqual( crimild::Real( 1 ), crimild::length( crimild::normalize( crimild::Vector4 { 2, 3, 4, 5 } ) ) ), "normalize" );

    static_assert( crimild::Vector4 { 1, 3, 2, 0 } == crimild::min( crimild::Vector4 { 2, 3, 4, 0 }, crimild::Vector4 { 1, 5, 2, 1 } ), "min" );
    static_assert( 2 == crimild::min( crimild::Vector4 { 2, 3, 4, 3 } ), "min" );
    static_assert( 0 == crimild::minDimension( crimild::Vector4 { 2, 3, 4, 3 } ), "minDimension" );

    static_assert( crimild::Vector4 { 2, 5, 4, 10 } == crimild::max( crimild::Vector4 { 2, 3, 4, 0 }, crimild::Vector4 { 1, 5, 2, 10 } ), "max" );
    static_assert( 4 == crimild::max( crimild::Vector4 { 2, 3, 4, 3 } ), "max" );
    static_assert( 2 == crimild::maxDimension( crimild::Vector4 { 2, 3, 4, 2 } ), "maxDimension" );

    EXPECT_TRUE( true );
}
