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

#include "Mathematics/Point3.hpp"

#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Point_equality.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector4.hpp"
#include "Mathematics/Vector_equality.hpp"
#include "Mathematics/ceil.hpp"
#include "Mathematics/distance.hpp"
#include "Mathematics/easing.hpp"
#include "Mathematics/floor.hpp"
#include "Mathematics/io.hpp"
#include "Mathematics/isEqual.hpp"
#include "Mathematics/isNaN.hpp"
#include "Mathematics/max.hpp"
#include "Mathematics/min.hpp"
#include "Mathematics/permutation.hpp"
#include "Mathematics/swizzle.hpp"

#include "gtest/gtest.h"
#include <sstream>

using namespace crimild;

TEST( Point3, construction )
{
    constexpr auto u = crimild::Point3 { 10, 20, 30 };

    EXPECT_EQ( 10, u.x );
    EXPECT_EQ( 20, u.y );
    EXPECT_EQ( 30, u.z );
}

TEST( Point3, index )
{
    constexpr auto u = crimild::Point3f { 10, 20, 30 };

    EXPECT_EQ( 10, u[ 0 ] );
    EXPECT_EQ( 20, u[ 1 ] );
    EXPECT_EQ( 30, u[ 2 ] );
}

TEST( Point3, equality )
{
    constexpr auto u = crimild::Point3 { 10, 20, 30 };
    constexpr auto v = crimild::Point3 { 30, 40, 50 };
    constexpr auto w = crimild::Point3 { 10, 20, 30 };

    EXPECT_TRUE( crimild::isEqual( u, w ) );
    EXPECT_FALSE( crimild::isEqual( u, v ) );
    EXPECT_FALSE( crimild::isEqual( v, w ) );
}

TEST( Point3, vectorAddition )
{
    constexpr auto p = crimild::Point3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 30, 40, 50 };
    constexpr auto res = crimild::Point3 { 40, 60, 80 };

    static_assert( crimild::isEqual( res, p + v ) );

    EXPECT_TRUE( true );
}

TEST( Point3, subtraction )
{
    constexpr auto u = crimild::Point3 { 10, 20, 30 };
    constexpr auto v = crimild::Point3 { 30, 40, 50 };
    constexpr auto res = crimild::Vector3 { -20, -20, -20 };

    static_assert( crimild::isEqual( res, u - v ) );

    EXPECT_TRUE( true );
}

TEST( Point3, vectorSubtraction )
{
    constexpr auto u = crimild::Point3 { 10, 20, 30 };
    constexpr auto v = crimild::Vector3 { 30, 40, 50 };
    constexpr auto res = crimild::Point3 { -20, -20, -20 };

    static_assert( crimild::isEqual( res, u - v ) );

    EXPECT_TRUE( true );
}

TEST( Point3, isNaN )
{
    constexpr auto u = crimild::Point3 { 10, 20, 30 };
    constexpr auto v = crimild::Point3 { NAN, NAN, NAN };
    constexpr auto w = crimild::Point3 { NAN, NAN, 0 };

    EXPECT_TRUE( crimild::isNaN( v ) );
    EXPECT_FALSE( crimild::isNaN( u ) );
    EXPECT_TRUE( crimild::isNaN( w ) );
}

TEST( Point3, abs )
{
    constexpr auto u = crimild::Point3 { -10, -20, -30 };
    constexpr auto res = crimild::Point3 { 10, 20, 30 };

    EXPECT_TRUE( crimild::isEqual( res, crimild::abs( u ) ) );
}

TEST( Point3, distance )
{
    constexpr auto u = crimild::Point3 { 2, 3, 4 };
    constexpr auto v = crimild::Point3 { 5, 6, 7 };

    EXPECT_EQ( crimild::Real( 27 ), crimild::distanceSquared( u, v ) );
    EXPECT_TRUE( crimild::isEqual( crimild::Real( 5.1961524227 ), crimild::distance( u, v ) ) );
}

TEST( Point3, lerp )
{
    constexpr auto u = crimild::Point3 { 2, 3, 4 };
    constexpr auto v = crimild::Point3 { 5, 6, 7 };
    constexpr auto res = crimild::Point3 { 3.5, 4.5, 5.5 };

    EXPECT_TRUE( crimild::isEqual( res, crimild::lerp( u, v, 0.5 ) ) );
}

TEST( Point3, min )
{
    constexpr auto u = crimild::Point3 { 2, 3, 4 };
    constexpr auto v = crimild::Point3 { 1, 5, 2 };
    constexpr auto m = crimild::Point3 { 1, 3, 2 };

    EXPECT_EQ( 2, crimild::min( u ) );
    EXPECT_TRUE( crimild::isEqual( m, crimild::min( u, v ) ) );
}

TEST( Point3, max )
{
    constexpr auto u = crimild::Point3 { 2, 3, 4 };
    constexpr auto v = crimild::Point3 { 1, 5, 2 };
    constexpr auto m = crimild::Point3 { 2, 5, 4 };

    EXPECT_EQ( 4, crimild::max( u ) );
    EXPECT_TRUE( crimild::isEqual( m, crimild::max( u, v ) ) );
}

TEST( Point3, minDimension )
{
    constexpr auto u = crimild::Point3 { 2, 3, 4 };

    EXPECT_EQ( 0, crimild::minDimension( u ) );
}

TEST( Point3, maxDimension )
{
    constexpr auto u = crimild::Point3 { 2, 3, 4 };

    EXPECT_EQ( 2, crimild::maxDimension( u ) );
}

TEST( Point3, floor )
{
    constexpr auto u = crimild::Point3 { 2.5, 3.8, 4.9 };
    constexpr auto v = crimild::Point3 { 2, 3, 4 };

    EXPECT_TRUE( crimild::isEqual( v, crimild::floor( u ) ) );
}

TEST( Point3, ceil )
{
    constexpr auto u = crimild::Point3 { 2.5, 3.8, 4.9 };
    constexpr auto v = crimild::Point3 { 3, 4, 5 };

    EXPECT_TRUE( crimild::isEqual( v, crimild::ceil( u ) ) );
}

TEST( Point3, permute )
{
    constexpr auto u = crimild::Point3 { 2, 3, 4 };
    constexpr auto v = crimild::Point3 { 4, 2, 3 };

    EXPECT_TRUE( crimild::isEqual( v, crimild::permute( u, 2, 0, 1 ) ) );
}

TEST( Point3, homogeneous )
{
    constexpr auto P = crimild::Point3 { 1, 2, 3 };

    constexpr crimild::Vector4 V = crimild::vector4( P, 1 );
    static_assert( crimild::isEqual( V, crimild::Vector4 { 1, 2, 3, 1 } ) );

    constexpr auto Q = crimild::point3( crimild::xyz( V ) );
    static_assert( crimild::isEqual( P, Q ) );

    EXPECT_TRUE( true );
}

TEST( Point3, ostream )
{
    {
        constexpr auto u = crimild::Point3 { 2, 3, 4 };

        std::stringstream ss;
        ss << u;

        EXPECT_EQ( ss.str(), "(2.000000, 3.000000, 4.000000)" );
    }

    {
        constexpr auto u = crimild::Point3i { 2, 3, 4 };

        std::stringstream ss;
        ss << u;

        EXPECT_EQ( ss.str(), "(2, 3, 4)" );
    }
}
