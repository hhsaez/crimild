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

#include "Mathematics/Ray_apply.hpp"
#include "Mathematics/io.hpp"

#include "gtest/gtest.h"
#include <sstream>

TEST( Ray3, construction )
{
    constexpr auto r = crimild::Ray3 {
        crimild::Point3f { 10, 20, 30 },
        crimild::Vector3 { 0, 0, -1 },
    };

    constexpr auto o = crimild::Point3f { 10, 20, 30 };
    constexpr auto d = crimild::Vector3 { 0, 0, -1 };

    static_assert( crimild::isEqual( o, crimild::origin( r ) ) );
    static_assert( crimild::isEqual( d, crimild::direction( r ) ) );

    EXPECT_TRUE( true );
}

TEST( Ray3, apply )
{
    constexpr auto R = crimild::Ray3 {
        crimild::Point3f { 2, 3, 4 },
        crimild::Vector3 { 1, 0, 0 },
    };

    static_assert( R( 0 ) == crimild::Point3f { 2, 3, 4 } );
    static_assert( R( 1 ) == crimild::Point3f { 3, 3, 4 } );
    static_assert( R( -1 ) == crimild::Point3f { 1, 3, 4 } );

    EXPECT_TRUE( true );
}

TEST( Ray3, ostream )
{
    constexpr auto R = crimild::Ray3 {
        crimild::Point3f { 10, 20, 30 },
        crimild::Vector3 { 0, 0, -1 },
    };

    std::stringstream ss;
    ss << R;

    EXPECT_EQ( ss.str(), "[(10.000000, 20.000000, 30.000000), (0.000000, 0.000000, -1.000000)]" );
}
