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

#include "Mathematics/Matrix4.hpp"

#include "Mathematics/Matrix4_constants.hpp"
#include "Mathematics/Matrix4_determinant.hpp"
#include "Mathematics/Matrix4_equality.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Matrix4_transpose.hpp"
#include "Mathematics/Vector4.hpp"
#include "Mathematics/Vector4Ops.hpp"
#include "Mathematics/Vector4_constants.hpp"
#include "Mathematics/Vector_equality.hpp"
#include "Mathematics/io.hpp"
#include "Mathematics/isEqual.hpp"
#include "Mathematics/isZero.hpp"

#include "gtest/gtest.h"
#include <sstream>

using namespace crimild;

TEST( Matrix4, construction )
{
    constexpr auto M = crimild::Matrix4 {
        { 0, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 8, 9, 10, 11 },
        { 12, 13, 14, 15 },
    };

    static_assert( M[ 0 ] == crimild::Vector4 { 0, 1, 2, 3 } );
    static_assert( M[ 1 ] == crimild::Vector4 { 4, 5, 6, 7 } );
    static_assert( M[ 2 ] == crimild::Vector4 { 8, 9, 10, 11 } );
    static_assert( M[ 3 ] == crimild::Vector4 { 12, 13, 14, 15 } );

    EXPECT_TRUE( true );
}

TEST( Matrix4, equality )
{
    constexpr auto A = crimild::Matrix4 {
        { 1, 2, 3, 4 },
        { 5, 6, 7, 8 },
        { 9, 8, 7, 6 },
        { 5, 4, 3, 2 },
    };

    constexpr auto B = crimild::Matrix4 {
        { 1, 2, 3, 4 },
        { 5, 6, 7, 8 },
        { 9, 8, 7, 6 },
        { 5, 4, 3, 2 },
    };

    constexpr auto C = crimild::Matrix4 {
        { 2, 3, 4, 5 },
        { 6, 7, 8, 9 },
        { 9, 8, 7, 6 },
        { 5, 4, 3, 2 },
    };

    static_assert( A == B );
    static_assert( A != C );

    EXPECT_TRUE( true );
}

TEST( Matrix4, IDENTITY )
{
    static_assert(
        crimild::Matrix4::Constants::IDENTITY == crimild::Matrix4 {
            { 1, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 },
        } );

    EXPECT_TRUE( true );
}

TEST( Matrix4, ZERO )
{
    static_assert(
        crimild::Matrix4::Constants::ZERO == crimild::Matrix4 {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        } );

    EXPECT_TRUE( true );
}

TEST( Matrix4, ONE )
{
    static_assert(
        crimild::Matrix4::Constants::ONE == crimild::Matrix4 {
            { 1, 1, 1, 1 },
            { 1, 1, 1, 1 },
            { 1, 1, 1, 1 },
            { 1, 1, 1, 1 },
        } );

    EXPECT_TRUE( true );
}

TEST( Matrix4, transpose )
{
    constexpr auto M = crimild::Matrix4 {
        { 0, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 8, 9, 10, 11 },
        { 12, 13, 14, 15 },
    };

    constexpr auto MT = crimild::Matrix4 {
        { 0, 4, 8, 12 },
        { 1, 5, 9, 13 },
        { 2, 6, 10, 14 },
        { 3, 7, 11, 15 },
    };

    static_assert( crimild::isEqual( MT, crimild::transpose( M ) ) );

    static_assert( crimild::transpose( crimild::Matrix4::Constants::IDENTITY ) == crimild::Matrix4::Constants::IDENTITY );

    EXPECT_TRUE( true );
}

TEST( Matrix4, matrixProduct )
{
    constexpr auto A = crimild::Matrix4 {
        { 1, 5, 9, 5 },
        { 2, 6, 8, 4 },
        { 3, 7, 7, 3 },
        { 4, 8, 6, 2 },
    };

    constexpr auto B = crimild::Matrix4 {
        { -2, 3, 4, 1 },
        { 1, 2, 3, 2 },
        { 2, 1, 6, 7 },
        { 3, -1, 5, 8 },
    };

    constexpr auto C = crimild::Matrix4 {
        { 20, 44, 40, 16 },
        { 22, 54, 58, 26 },
        { 50, 114, 110, 46 },
        { 48, 108, 102, 42 },
    };

    static_assert( A * B == C );

    static_assert( ( A * crimild::Matrix4::Constants::IDENTITY ) == A );

    EXPECT_TRUE( true );
}

TEST( Matrix4, vectorProduct )
{
    constexpr auto A = crimild::Matrix4 {
        { 1, 2, 8, 0 },
        { 2, 4, 6, 0 },
        { 3, 4, 4, 0 },
        { 4, 2, 1, 1 },
    };

    constexpr auto V = crimild::Vector4 {
        1,
        2,
        3,
        1,
    };

    static_assert( A * V == crimild::Vector4 { 18, 24, 33, 1 } );

    static_assert( ( crimild::Matrix4::Constants::IDENTITY * V ) == V );

    static_assert( ( A * crimild::Vector4::Constants::ZERO ) == crimild::Vector4::Constants::ZERO );

    EXPECT_TRUE( true );
}

TEST( Matrix4, determinant )
{
    constexpr auto A = crimild::Matrix4 {
        { -2, -3, 1, -6 },
        { -8, 1, 2, 7 },
        { 3, 7, -9, 7 },
        { 5, 3, 6, -9 },
    };

    static_assert( crimild::determinant( A ) == -4071 );

    constexpr auto B = crimild::Matrix4 {
        { 6, 5, 4, 9 },
        { 4, 5, -9, 1 },
        { 4, 7, 3, 7 },
        { 4, 6, -7, -6 },
    };

    static_assert( crimild::determinant( B ) == -2120 );

    constexpr auto C = crimild::Matrix4 {
        { -4, 9, 0, 0 },
        { 2, 6, -5, 0 },
        { -2, 2, 1, 0 },
        { -3, 6, -5, 0 },
    };

    static_assert( crimild::isZero( crimild::determinant( C ) ) );

    EXPECT_TRUE( true );
}

TEST( Matrix4, inverse )
{
    constexpr auto A = crimild::Matrix4 {
        { -5, 1, 7, 1 },
        { 2, -5, 7, -3 },
        { 6, 1, -6, 7 },
        { -8, 8, -7, 4 },
    };

    static_assert( crimild::determinant( A ) == 532 );

    constexpr auto B = crimild::Matrix4 {
        { 0.21805, -0.80827, -0.07895, -0.52256 },
        { 0.45113, -1.45677, -0.22368, -0.81391 },
        { 0.24060, -0.44361, -0.05263, -0.30075 },
        { -0.04511, 0.52068, 0.19737, 0.30639 },
    };

    EXPECT_EQ( crimild::Matrix4::Constants::IDENTITY, A * B );
    EXPECT_EQ( crimild::Matrix4::Constants::IDENTITY, B * A );

    EXPECT_EQ( B, crimild::inverse( A ) );

    EXPECT_EQ( crimild::Matrix4::Constants::IDENTITY, A * crimild::inverse( A ) );
    EXPECT_EQ( crimild::Matrix4::Constants::IDENTITY, crimild::inverse( A ) * A );
}

TEST( Matrix4, inverseProduct )
{
    constexpr auto A = crimild::Matrix4 {
        { 3, 3, -4, -6 },
        { -9, -8, 4, 5 },
        { 7, 2, 4, -1 },
        { 3, -9, 1, 1 },
    };

    constexpr auto B = crimild::Matrix4 {
        { 8, 3, 7, 6 },
        { 2, -1, 0, -2 },
        { 2, 7, 5, 0 },
        { 2, 0, 4, 5 },
    };

    constexpr auto C = A * B;

    EXPECT_EQ( A, C * crimild::inverse( B ) );
}

TEST( Matrix4, io )
{
    constexpr auto A = crimild::Matrix4 {
        { 3, 3, -4, -6 },
        { -9, -8, 4, 5 },
        { 7, 2, 4, -1 },
        { 3, -9, 1, 1 },
    };

    std::stringstream ss;
    ss << A;

    EXPECT_EQ( "[(3.000000, 3.000000, -4.000000, -6.000000), (-9.000000, -8.000000, 4.000000, 5.000000), (7.000000, 2.000000, 4.000000, -1.000000), (3.000000, -9.000000, 1.000000, 1.000000)]", ss.str() );
}
