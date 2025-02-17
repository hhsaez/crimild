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

#include "Cylinder.hpp"

#include "height.hpp"
#include "normal.hpp"
#include "radius.hpp"

#include <gtest/gtest.h>
#include <sstream>

using namespace crimild;

TEST( Cylinder, construction )
{
    constexpr auto C = Cylinder {};

    static_assert( radius( C ) == real_t( 1 ) );
    static_assert( height( C ) == real_t( 1 ) );

    EXPECT_TRUE( true );
}

TEST( Cylinder, normal )
{
    constexpr auto C = Cylinder {};

    EXPECT_EQ( ( Normal3 { 1, 0, 0 } ), normal( C, Point3f { 1, 0, 0 } ) );
    EXPECT_EQ( ( Normal3 { 0, 0, -1 } ), normal( C, Point3f { 0, 5, -1 } ) );
    EXPECT_EQ( ( Normal3 { 0, 0, 1 } ), normal( C, Point3f { 0, -2, 1 } ) );
    EXPECT_EQ( ( Normal3 { -1, 0, 0 } ), normal( C, Point3f { -1, 1, 0 } ) );
}
