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

#include "Matrix3.hpp"

#include "determinant.hpp"
#include "transpose.hpp"

#include <gtest/gtest.h>
#include <sstream>

TEST( Matrix3, sizeof )
{
    static_assert( sizeof( crimild::Matrix3 {} ) == 9 * sizeof( crimild::real_t ) );

    // Seems redudant to check at run time, right?
    EXPECT_EQ( sizeof( crimild::Matrix3 {} ), 9 * sizeof( crimild::real_t ) );
}

TEST( Matrix3, construction )
{
    constexpr auto M = crimild::Matrix3 {
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9 },
    };

    static_assert( M[ 0 ] == crimild::Vector3 { 1, 2, 3 } );
    static_assert( M[ 1 ] == crimild::Vector3 { 4, 5, 6 } );
    static_assert( M[ 2 ] == crimild::Vector3 { 7, 8, 9 } );

    EXPECT_TRUE( true );
}

TEST( Matrix3, fromValues )
{
    constexpr auto M = crimild::Matrix3 { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    static_assert( M[ 0 ] == crimild::Vector3 { 1, 4, 7 } );
    static_assert( M[ 1 ] == crimild::Vector3 { 2, 5, 8 } );
    static_assert( M[ 2 ] == crimild::Vector3 { 3, 6, 9 } );

    EXPECT_TRUE( true );
}

TEST( Matrix3, IDENTITY )
{
    static_assert(
        crimild::Matrix3::Constants::IDENTITY == crimild::Matrix3 {
            { 1, 0, 0 },
            { 0, 1, 0 },
            { 0, 0, 1 },
        }
    );

    EXPECT_TRUE( true );
}

TEST( Matrix3, ZERO )
{
    static_assert(
        crimild::Matrix3::Constants::ZERO == crimild::Matrix3 {
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
        }
    );

    EXPECT_TRUE( true );
}

TEST( Matrix3, determinant )
{
    constexpr auto M = crimild::Matrix3 {
        { 2, 1, 1 },
        { 1, 3, 2 },
        { 1, 2, 4 },
    };

    static_assert( determinant( M ) == 13 );

    EXPECT_TRUE( true );
}

TEST( Matrix3, transpose )
{
    constexpr auto M = crimild::Matrix3 {
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9 },
    };

    constexpr auto T = crimild::Matrix3 {
        { 1, 4, 7 },
        { 2, 5, 8 },
        { 3, 6, 9 },
    };

    static_assert( transpose( M ) == T );

    EXPECT_TRUE( true );
}
