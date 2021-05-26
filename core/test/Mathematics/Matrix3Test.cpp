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

#include "Mathematics/Matrix3.hpp"

#include "Mathematics/Vector3.hpp"

#include "gtest/gtest.h"
#include <sstream>

TEST( Matrix3, construction )
{
    // clang-format off
    constexpr auto M = crimild::Matrix3 {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };
    // clang-format on

    EXPECT_EQ( 1, M[ 0 ] );
    EXPECT_EQ( 0, M[ 1 ] );
    EXPECT_EQ( 0, M[ 2 ] );
    EXPECT_EQ( 0, M[ 3 ] );
    EXPECT_EQ( 1, M[ 4 ] );
    EXPECT_EQ( 0, M[ 5 ] );
    EXPECT_EQ( 0, M[ 6 ] );
    EXPECT_EQ( 0, M[ 7 ] );
    EXPECT_EQ( 1, M[ 8 ] );
}

TEST( Matrix3, IDENTITY )
{
    constexpr auto I = crimild::Matrix3::Constants::IDENTITY;

    EXPECT_EQ( 1, I[ 0 ] );
    EXPECT_EQ( 0, I[ 1 ] );
    EXPECT_EQ( 0, I[ 2 ] );
    EXPECT_EQ( 0, I[ 3 ] );
    EXPECT_EQ( 1, I[ 4 ] );
    EXPECT_EQ( 0, I[ 5 ] );
    EXPECT_EQ( 0, I[ 6 ] );
    EXPECT_EQ( 0, I[ 7 ] );
    EXPECT_EQ( 1, I[ 8 ] );
}

TEST( Matrix3, ZERO )
{
    constexpr auto Z = crimild::Matrix3::Constants::ZERO;

    EXPECT_EQ( 0, Z[ 0 ] );
    EXPECT_EQ( 0, Z[ 1 ] );
    EXPECT_EQ( 0, Z[ 2 ] );
    EXPECT_EQ( 0, Z[ 3 ] );
    EXPECT_EQ( 0, Z[ 4 ] );
    EXPECT_EQ( 0, Z[ 5 ] );
    EXPECT_EQ( 0, Z[ 6 ] );
    EXPECT_EQ( 0, Z[ 7 ] );
    EXPECT_EQ( 0, Z[ 8 ] );
}
