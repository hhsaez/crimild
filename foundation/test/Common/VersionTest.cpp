/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Common/Version.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( VersionTest, defaultConstruction )
{
    auto v = Version();

    EXPECT_GE( v.getMajor(), 5 );
    EXPECT_GE( v.getMinor(), 0 );
    EXPECT_GE( v.getPatch(), 0 );
}

TEST( VersionTest, construction )
{
    auto v = Version( 1, 2, 3 );

    EXPECT_EQ( 1, v.getMajor() );
    EXPECT_EQ( 2, v.getMinor() );
    EXPECT_EQ( 3, v.getPatch() );
}

TEST( VersionTest, copy )
{
    auto v1 = Version( 1, 2, 3 );
    auto v2 = v1;

    EXPECT_EQ( v1.getMajor(), v2.getMajor() );
    EXPECT_EQ( v1.getMinor(), v2.getMinor() );
    EXPECT_EQ( v1.getPatch(), v2.getPatch() );
}

TEST( VersionTest, toInt )
{
    auto v1 = Version( 1, 2, 3 );

    auto i = v1.toInt();

    EXPECT_EQ( 1002003, i );
}

TEST( VersionTest, getDescription )
{
    auto v = Version( 1, 2, 3 );

    EXPECT_EQ( "v1.2.3", v.getDescription() );
}

TEST( VersionTest, fromString )
{
    Version v;
    v.fromString( "v1.2.3" );

    EXPECT_EQ( 1, v.getMajor() );
    EXPECT_EQ( 2, v.getMinor() );
    EXPECT_EQ( 3, v.getPatch() );
}

TEST( VersionTest, compareGE )
{
    auto v1 = Version( 1, 2, 3 );
    auto v2 = Version( 1, 2, 4 );
    auto v3 = v2;
    auto v4 = Version( 4, 5, 6 );

    EXPECT_TRUE( v1 < v2 );
    EXPECT_TRUE( v2 == v3 );
    EXPECT_TRUE( v4 > v1 );
}
