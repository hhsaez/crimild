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

#include "Point2.hpp"

#include "ceil.hpp"
#include "distance.hpp"
#include "easing.hpp"
#include "floor.hpp"
#include "io.hpp"
#include "isEqual.hpp"
#include "isNaN.hpp"
#include "max.hpp"
#include "min.hpp"
#include "permutation.hpp"
#include "swizzle.hpp"

#include "gtest/gtest.h"
#include <sstream>

using namespace crimild;

TEST( Point2, construction )
{
    constexpr auto u = crimild::Point2 { 10, 20 };

    EXPECT_EQ( 10, u.x );
    EXPECT_EQ( 20, u.y );
}

TEST( Point2, index )
{
    constexpr auto u = crimild::Point2 { 10, 20 };

    EXPECT_EQ( 10, u[ 0 ] );
    EXPECT_EQ( 20, u[ 1 ] );

    auto v = Point2 { 10, 20 };
    v[ 0 ] = 20;
    v[ 1 ] = 21;
    EXPECT_TRUE( isEqual( v, Point2 { 20, 21 } ) );
}

TEST( Point2, equality )
{
    constexpr auto u = crimild::Point2 { 10, 20 };
    constexpr auto v = crimild::Point2 { 30, 40 };
    constexpr auto w = crimild::Point2 { 10, 20 };

    static_assert( u != v );
    static_assert( u == w );

    EXPECT_TRUE( crimild::isEqual( u, w ) );
    EXPECT_FALSE( crimild::isEqual( u, v ) );
    EXPECT_FALSE( crimild::isEqual( v, w ) );
}

// TEST( Point2, vectorAddition )
// {
//     constexpr auto p = crimild::Point2 { 10, 20 };
//     constexpr auto v = crimild::Vector3 { 30, 40 };
//     constexpr auto res = crimild::Point2 { 40, 60 };

//     static_assert( crimild::isEqual( res, p + v ) );

//     EXPECT_TRUE( true );
// }

// TEST( Point2, subtraction )
// {
//     constexpr auto u = crimild::Point2 { 10, 20 };
//     constexpr auto v = crimild::Point2 { 30, 40 };
//     constexpr auto res = crimild::Vector3 { -20, -20 };

//     static_assert( crimild::isEqual( res, u - v ) );

//     EXPECT_TRUE( true );
// }

// TEST( Point2, vectorSubtraction )
// {
//     constexpr auto u = crimild::Point2 { 10, 20 };
//     constexpr auto v = crimild::Vector3 { 30, 40 };
//     constexpr auto res = crimild::Point2 { -20, -20 };

//     static_assert( crimild::isEqual( res, u - v ) );

//     EXPECT_TRUE( true );
// }

TEST( Point2, isNaN )
{
    constexpr auto u = crimild::Point2 { 10, 20 };
    constexpr auto v = crimild::Point2 { NAN, NAN };
    constexpr auto w = crimild::Point2 { NAN, 0 };

    EXPECT_TRUE( crimild::isNaN( v ) );
    EXPECT_FALSE( crimild::isNaN( u ) );
    EXPECT_TRUE( crimild::isNaN( w ) );
}

TEST( Point2, abs )
{
    constexpr auto u = crimild::Point2 { -10, -20 };
    constexpr auto res = crimild::Point2 { 10, 20 };

    EXPECT_TRUE( crimild::isEqual( res, crimild::abs( u ) ) );
}

// TEST( Point2, distance )
// {
//     constexpr auto u = crimild::Point2 { 2, 3 };
//     constexpr auto v = crimild::Point2 { 5, 6 };

//     EXPECT_EQ( crimild::Real( 27 ), crimild::distanceSquared( u, v ) );
//     EXPECT_TRUE( crimild::isEqual( crimild::Real( 5.1961524227 ), crimild::distance( u, v ) ) );
// }

TEST( Point2, lerp )
{
    constexpr auto u = crimild::Point2 { 2, 3 };
    constexpr auto v = crimild::Point2 { 5, 6 };
    constexpr auto res = crimild::Point2 { 3.5, 4.5 };

    EXPECT_TRUE( crimild::isEqual( res, crimild::lerp( u, v, 0.5 ) ) );
}

TEST( Point2, min )
{
    constexpr auto u = crimild::Point2 { 2, 3 };
    constexpr auto v = crimild::Point2 { 1, 5 };
    constexpr auto m = crimild::Point2 { 1, 3 };

    EXPECT_EQ( 2, crimild::min( u ) );
    EXPECT_TRUE( crimild::isEqual( m, crimild::min( u, v ) ) );

    {
        const auto A = crimild::Point2 { 1, -1 };
        const auto B = crimild::min( A, crimild::Point2::Constants::POSITIVE_INFINITY );
        EXPECT_EQ( A, B );
    }
}

TEST( Point2, max )
{
    constexpr auto u = crimild::Point2 { 2, 3 };
    constexpr auto v = crimild::Point2 { 1, 5 };
    constexpr auto m = crimild::Point2 { 2, 5 };

    EXPECT_EQ( 3, crimild::max( u ) );
    EXPECT_TRUE( crimild::isEqual( m, crimild::max( u, v ) ) );

    {
        const auto A = crimild::Point2 { 1, -1 };
        const auto B = crimild::max( A, crimild::Point2::Constants::NEGATIVE_INFINITY );
        EXPECT_EQ( A, B );
    }
}

TEST( Point2, minDimension )
{
    constexpr auto u = crimild::Point2 { 2, 3 };

    EXPECT_EQ( 0, crimild::minDimension( u ) );
}

TEST( Point2, maxDimension )
{
    constexpr auto u = crimild::Point2 { 2, 3 };

    EXPECT_EQ( 1, crimild::maxDimension( u ) );
}

TEST( Point2, floor )
{
    constexpr auto u = crimild::Point2 { 2.5, 3.8 };
    constexpr auto v = crimild::Point2 { 2, 3 };

    EXPECT_TRUE( crimild::isEqual( v, crimild::floor( u ) ) );
}

TEST( Point2, ceil )
{
    constexpr auto u = crimild::Point2 { 2.5, 3.8 };
    constexpr auto v = crimild::Point2 { 3, 4 };

    EXPECT_TRUE( crimild::isEqual( v, crimild::ceil( u ) ) );
}

TEST( Point2, permute )
{
    constexpr auto u = crimild::Point2 { 2, 3 };
    constexpr auto v = crimild::Point2 { 3, 2 };

    EXPECT_TRUE( crimild::isEqual( v, crimild::permute( u, 1, 0 ) ) );
}

TEST( Point2, ostream )
{
    {
        constexpr auto u = crimild::Point2 { 2, 3 };

        std::stringstream ss;
        ss << u;

        EXPECT_EQ( ss.str(), "(2.000000, 3.000000)" );
    }

    {
        constexpr auto u = crimild::Point2i { 2, 3 };

        std::stringstream ss;
        ss << u;

        EXPECT_EQ( ss.str(), "(2, 3)" );
    }
}
