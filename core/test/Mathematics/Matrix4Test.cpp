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

#include "Mathematics/Vector4.hpp"

#include "gtest/gtest.h"
#include <sstream>

TEST( Matrix4, construction )
{
    constexpr auto M = crimild::Matrix4 {
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
    };

    EXPECT_EQ( 1, M[ 0 ][ 0 ] );
    EXPECT_EQ( 0, M[ 0 ][ 1 ] );
    EXPECT_EQ( 0, M[ 0 ][ 2 ] );
    EXPECT_EQ( 0, M[ 0 ][ 3 ] );

    EXPECT_EQ( 0, M[ 1 ][ 0 ] );
    EXPECT_EQ( 1, M[ 1 ][ 1 ] );
    EXPECT_EQ( 0, M[ 1 ][ 2 ] );
    EXPECT_EQ( 0, M[ 1 ][ 3 ] );

    EXPECT_EQ( 0, M[ 2 ][ 0 ] );
    EXPECT_EQ( 0, M[ 2 ][ 1 ] );
    EXPECT_EQ( 1, M[ 2 ][ 2 ] );
    EXPECT_EQ( 0, M[ 2 ][ 3 ] );

    EXPECT_EQ( 0, M[ 3 ][ 0 ] );
    EXPECT_EQ( 0, M[ 3 ][ 1 ] );
    EXPECT_EQ( 0, M[ 3 ][ 2 ] );
    EXPECT_EQ( 1, M[ 3 ][ 3 ] );
}

TEST( Matrix4, IDENTITY )
{
    constexpr auto I = crimild::Matrix4::Constants::IDENTITY;

    static_assert( crimild::Vector4( 1, 0, 0, 0 ) == I[ 0 ], "identity" );
    static_assert( crimild::Vector4( 0, 1, 0, 0 ) == I[ 1 ], "identity" );
    static_assert( crimild::Vector4( 0, 0, 1, 0 ) == I[ 2 ], "identity" );
    static_assert( crimild::Vector4( 0, 0, 0, 1 ) == I[ 3 ], "identity" );

    EXPECT_TRUE( true );
}

TEST( Matrix4, ZERO )
{
    constexpr auto Z = crimild::Matrix4::Constants::ZERO;

    static_assert( crimild::Vector4 { 0, 0, 0, 0 } == Z[ 0 ], "" );
    static_assert( crimild::Vector4 { 0, 0, 0, 0 } == Z[ 1 ], "" );
    static_assert( crimild::Vector4 { 0, 0, 0, 0 } == Z[ 2 ], "" );
    static_assert( crimild::Vector4 { 0, 0, 0, 0 } == Z[ 3 ], "" );

    EXPECT_TRUE( true );
}

TEST( Matrix4, transpose )
{
    // clang-format off
    constexpr auto M = crimild::Matrix4 {
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15,
    };

    constexpr auto MT = crimild::Matrix4 {
        0, 4, 8, 12,
        1, 5, 9, 13,
        2, 6, 10, 14,
        3, 7, 11, 15,
    };
    // clang-format on

    static_assert( MT == crimild::transpose( M ) );

    EXPECT_TRUE( true );
}

TEST( Matrix4, determinant )
{
    // clang-format off
    constexpr auto M = crimild::Matrix4 {
        10, 1, 2, 3,
        4, 5, 6, 7,
        8, 19, 10, 11,
        121, 13, 14, 15,
    };
    // clang-format on

    constexpr auto d = crimild::determinant( M );

    static_assert( d == 5160 );

    EXPECT_TRUE( true );
}

TEST( Matrix4, inverse )
{
    // clang-format off
    constexpr auto M = crimild::Matrix4 {
        10, 1, 2, 3,
        4, 5, 6, 7,
        8, 19, 10, 11,
        121, 13, 14, 15,
    };
    // clang-format on

    constexpr auto MI = crimild::inverse( M );

    static_assert( crimild::Matrix4::Constants::IDENTITY == M * MI );

    EXPECT_TRUE( true );
}
