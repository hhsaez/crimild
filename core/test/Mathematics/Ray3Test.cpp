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

#include "Mathematics/Ray3.hpp"

#include "gtest/gtest.h"
#include <sstream>

TEST( Ray3, construction )
{
    constexpr auto r = crimild::Ray3 {
        crimild::Point3 { 10, 20, 30 },
        crimild::Vector3 { 0, 0, -1 },
    };

    constexpr auto o = crimild::Point3 { 10, 20, 30 };
    constexpr auto d = crimild::Vector3 { 0, 0, -1 };

    EXPECT_EQ( o, r.getOrigin() );
    EXPECT_EQ( d, r.getDirection() );
}

TEST( Ray3, apply )
{
    constexpr auto R = crimild::Ray3 {
        crimild::Point3 { 10, 20, 30 },
        crimild::Vector3 { 0, 0, -1 },
    };

    constexpr auto P = crimild::Point3 { 10, 20, 20 };

    EXPECT_EQ( P, R( 10 ) );
}

TEST( Ray3, ostream )
{
    constexpr auto R = crimild::Ray3 {
        crimild::Point3 { 10, 20, 30 },
        crimild::Vector3 { 0, 0, -1 },
    };

    std::stringstream ss;
    ss << R;

    EXPECT_EQ( ss.str(), "[(10.000000, 20.000000, 30.000000), (0.000000, 0.000000, -1.000000)]" );
}

TEST( Ray3, constexpr )
{
    constexpr auto R = crimild::Ray3 {
        crimild::Point3 { 10, 20, 30 },
        crimild::Vector3 { 0, 0, -1 },
    };

    constexpr auto P = crimild::Point3 { 10, 20, 20 };

    static_assert( P == R( 10 ), "apply" );

    EXPECT_TRUE( true );
}