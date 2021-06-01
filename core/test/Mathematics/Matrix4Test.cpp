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
#include "Mathematics/Vector_equality.hpp"
#include "Mathematics/isEqual.hpp"

#include "gtest/gtest.h"
#include <sstream>

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

    static_assert( crimild::isEqual( MT, crimild::transpose( M ) ) );

    EXPECT_TRUE( true );
}

TEST( Matrix4, matrixProduct )
{
    constexpr auto a = crimild::Real( 1 );
    constexpr auto b = crimild::Real( 2 );
    constexpr auto c = crimild::Real( 3 );
    constexpr auto d = crimild::Real( 4 );
    constexpr auto e = crimild::Real( 5 );
    constexpr auto f = crimild::Real( 6 );
    constexpr auto g = crimild::Real( 7 );
    constexpr auto h = crimild::Real( 8 );
    constexpr auto i = crimild::Real( 9 );
    constexpr auto j = crimild::Real( 10 );
    constexpr auto k = crimild::Real( 11 );
    constexpr auto l = crimild::Real( 12 );
    constexpr auto m = crimild::Real( 13 );
    constexpr auto n = crimild::Real( 14 );
    constexpr auto o = crimild::Real( 15 );
    constexpr auto p = crimild::Real( 16 );

    constexpr auto A = crimild::Matrix4 {
        { a, b, c, d },
        { e, f, g, h },
        { i, j, k, l },
        { m, n, o, p },
    };

    constexpr auto B = crimild::Matrix4 {
        { a, b, c, d },
        { e, f, g, h },
        { i, j, k, l },
        { m, n, o, p },
    };

    static_assert(
        ( A * B ) == crimild::Matrix4 {
            {
                a * a + e * b + i * c + m * d,
                b * a + f * b + j * c + n * d,
                c * a + g * b + k * c + o * d,
                d * a + h * b + l * c + p * d,
            },
            {
                a * e + e * f + i * g + m * h,
                b * e + f * f + j * g + n * h,
                c * e + g * f + k * g + o * h,
                d * e + h * f + l * g + p * h,
            },
            {
                a * i + e * j + i * k + m * l,
                b * i + f * j + j * k + n * l,
                c * i + g * j + k * k + o * l,
                d * i + h * j + l * k + p * l,
            },
            {
                a * m + e * n + i * o + m * p,
                b * m + f * n + j * o + n * p,
                c * m + g * n + k * o + o * p,
                d * m + h * n + l * o + p * p,
            },
        } );

    static_assert( ( A * crimild::Matrix4::Constants::IDENTITY ) == A );

    EXPECT_TRUE( true );
}

TEST( Matrix4, vectorProduct )
{
    constexpr auto A = crimild::Matrix4 {
        { 0, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 8, 9, 10, 11 },
        { 12, 13, 14, 15 },
    };

    constexpr auto V = crimild::Vector4 {
        0,
        1,
        2,
        3,
    };

    static_assert(
        ( A * V ) == crimild::Vector4 {
            0 * 0 + 4 * 1 + 8 * 2 + 12 * 3,
            1 * 0 + 5 * 1 + 9 * 2 + 13 * 3,
            2 * 0 + 6 * 1 + 10 * 2 + 14 * 3,
            3 * 0 + 7 * 1 + 11 * 2 + 15 * 3,
        } );

    static_assert( ( crimild::Matrix4::Constants::IDENTITY * V ) == V );

    static_assert( ( A * crimild::Vector4::Constants::ZERO ) == crimild::Vector4::Constants::ZERO );

    static_assert(
        ( A * crimild::Vector4::Constants::ONE ) == crimild::Vector4 {
            0 * 1 + 4 * 1 + 8 * 1 + 12 * 1,
            1 * 1 + 5 * 1 + 9 * 1 + 13 * 1,
            2 * 1 + 6 * 1 + 10 * 1 + 14 * 1,
            3 * 1 + 7 * 1 + 11 * 1 + 15 * 1,
        } );

    EXPECT_TRUE( true );
}

TEST( Matrix4, determinant )
{
    constexpr auto M = crimild::Matrix4 {
        { 10, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 8, 19, 10, 11 },
        { 121, 13, 14, 15 },
    };

    constexpr auto d = crimild::determinant( M );

    static_assert( d == 5160 );

    EXPECT_TRUE( true );
}

TEST( Matrix4, inverse )
{
    constexpr auto M = crimild::Matrix4 {
        { 10, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 8, 19, 10, 11 },
        { 121, 13, 14, 15 },
    };

    constexpr auto MI = crimild::inverse( M );

    static_assert( crimild::Matrix4::Constants::IDENTITY == M * MI );

    EXPECT_TRUE( true );
}
