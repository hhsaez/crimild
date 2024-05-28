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

#include "Vector4.hpp"

#include "Vector3.hpp"
#include "Vector4.hpp"
#include "abs.hpp"
#include "dot.hpp"
#include "io.hpp"
#include "isEqual.hpp"
#include "isNaN.hpp"
#include "length.hpp"
#include "max.hpp"
#include "min.hpp"
#include "normalize.hpp"
#include "permutation.hpp"
#include "swizzle.hpp"

#include "gtest/gtest.h"
#include <sstream>

using namespace crimild;

TEST( Vector4, construction )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };

    EXPECT_EQ( 10, u.x );
    EXPECT_EQ( 20, u.y );
    EXPECT_EQ( 30, u.z );
    EXPECT_EQ( 40, u.w );
}

TEST( Vector4, fromPoint3 )
{
    constexpr auto p3 = Point3i( 1, 2, 3 );
    constexpr auto v4 = Vector4i( p3 );
    static_assert( v4 == Vector4i( 1, 2, 3, 1 ) );
    EXPECT_TRUE( true );
}

TEST( Vector4, fromVector3 )
{
    constexpr auto v3 = Vector3i( 1, 2, 3 );
    constexpr auto v4 = Vector4i( v3 );
    static_assert( v4 == Vector4i( 1, 2, 3, 0 ) );
    EXPECT_TRUE( true );
}

TEST( Vector4, fromNormal3 )
{
    constexpr auto n3 = Vector3i( 1, 2, 3 );
    constexpr auto v4 = Vector4i( n3 );
    static_assert( v4 == Vector4i( 1, 2, 3, 0 ) );
    EXPECT_TRUE( true );
}

TEST( Vector4, index )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };

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

    static_assert( crimild::isEqual( u, w ), "equality" );
    static_assert( !crimild::isEqual( u, v ), "inequality" );

    EXPECT_TRUE( true );
}

TEST( Vector4, addition )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto v = crimild::Vector4 { 30, 40, 50, 60 };
    constexpr auto res = crimild::Vector4 { 40, 60, 80, 100 };

    static_assert( crimild::isEqual( res, u + v ) );

    EXPECT_TRUE( true );
}

TEST( Vector4, subtraction )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto v = crimild::Vector4 { 30, 40, 50, 60 };
    constexpr auto res = crimild::Vector4 { -20, -20, -20, -20 };

    static_assert( crimild::isEqual( res, u - v ) );

    EXPECT_TRUE( true );
}

TEST( Vector4, multiplication )
{
    constexpr auto u = crimild::Vector4f { 10, 20, 30, 40 };
    constexpr auto s = real_t( 5 );
    constexpr auto res = crimild::Vector4f { 50, 100, 150, 200 };

    static_assert( crimild::isEqual( res, u * s ) );
    static_assert( crimild::isEqual( res, s * u ) );

    EXPECT_TRUE( true );
}

TEST( Vector4, vectorMultiplication )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto v = crimild::Vector4 { 2, 3, 4, 5 };
    constexpr auto res = crimild::Vector4 { 20, 60, 120, 200 };

    static_assert( crimild::isEqual( res, u * v ) );

    EXPECT_TRUE( true );
}

TEST( Vector4, division )
{
    constexpr auto u = crimild::Vector4f { 10, 20, 30, 40 };
    constexpr auto s = real_t( 2 );
    constexpr auto res = crimild::Vector4f { 5, 10, 15, 20 };

    static_assert( crimild::isEqual( res, u / s ) );

    EXPECT_TRUE( true );
}

TEST( Vector4, negation )
{
    constexpr auto u = crimild::Vector4 { 10, 20, 30, 40 };
    constexpr auto res = crimild::Vector4 { -10, -20, -30, -40 };

    static_assert( crimild::isEqual( res, -u ) );

    EXPECT_TRUE( true );
}

TEST( Vector4, isNaN )
{
    constexpr auto u = crimild::Vector4f { 10, 20, 30, 40 };
    constexpr auto v = crimild::Vector4f { NAN, NAN, NAN, NAN };
    constexpr auto w = crimild::Vector4f { NAN, NAN, 0, NAN };

    EXPECT_TRUE( crimild::isNaN( v ) );
    EXPECT_FALSE( crimild::isNaN( u ) );
    EXPECT_TRUE( crimild::isNaN( w ) );
}

TEST( Vector4, abs )
{
    constexpr auto u = crimild::Vector4f { -10, -20, -30, -40 };
    constexpr auto res = crimild::Vector4f { 10, 20, 30, 40 };

    static_assert( crimild::isEqual( res, crimild::abs( u ) ) );

    EXPECT_TRUE( true );
}

TEST( Vector4, dot )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 5, 10 };
    constexpr auto v = crimild::Vector4 { 8, 10, -2, -1 };

    EXPECT_EQ( 26, crimild::dot( u, v ) );
}

TEST( Vector4, length )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 4, 5 };

    EXPECT_EQ( real_t( 54 ), crimild::length2( u ) );
    EXPECT_EQ( real_t( 7.3484692283 ), crimild::length( u ) );
}

TEST( Vector4, normalize )
{
    constexpr auto u = crimild::Vector4f { 2, 3, 4, 5 };
    constexpr auto v = crimild::Vector4f { 0.272165527, 0.4082482905, 0.544331054, 0.6804138174 };

    static_assert( crimild::isEqual( v, crimild::normalize( u ) ) );

    EXPECT_TRUE( crimild::isEqual( real_t( 1 ), crimild::length2( crimild::normalize( u ) ) ) );
    EXPECT_TRUE( crimild::isEqual( real_t( 1 ), crimild::length( crimild::normalize( u ) ) ) );
}

TEST( Vector4, normalizeNaN )
{
    EXPECT_TRUE( crimild::isNaN( crimild::normalize( crimild::Vector4f {} ) ) );
}

TEST( Vector4, min )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 4, 1 };
    constexpr auto v = crimild::Vector4 { 1, 5, 2, 10 };
    constexpr auto m = crimild::Vector4 { 1, 3, 2, 1 };

    EXPECT_EQ( 1, crimild::min( u ) );
    static_assert( crimild::isEqual( m, crimild::min( u, v ) ) );

    EXPECT_TRUE( true );
}

TEST( Vector4, max )
{
    constexpr auto u = crimild::Vector4 { 2, 3, 4, 1 };
    constexpr auto v = crimild::Vector4 { 1, 5, 2, 10 };
    constexpr auto m = crimild::Vector4 { 2, 5, 4, 10 };

    EXPECT_EQ( 4, crimild::max( u ) );
    static_assert( crimild::isEqual( m, crimild::max( u, v ) ) );

    EXPECT_TRUE( true );
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
        constexpr auto u = crimild::Vector4f { 2, 3, 4, 1 };

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
    static_assert( crimild::isEqual( crimild::Vector4 { 6, 4, 1, 5 }, crimild::permute( crimild::Vector4f { 4, 5, 6, 1 }, 2, 0, 3, 1 ) ) );
    EXPECT_TRUE( true );
}

TEST( Vector4, swizzle )
{
    constexpr auto V = crimild::Vector4 { 1, 2, 3, 4 };

    static_assert( crimild::isEqual( crimild::Vector3( V ), crimild::Vector3 { 1, 2, 3 } ) );
}
