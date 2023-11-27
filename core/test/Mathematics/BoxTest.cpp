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

#include "Mathematics/Box.hpp"

#include "Mathematics/Box_normal.hpp"
#include "Mathematics/Point_equality.hpp"

#include "gtest/gtest.h"
#include <sstream>

using namespace crimild;

TEST( Box, construction )
{
    constexpr auto B = Box {};

    EXPECT_EQ( ( Point3 { 0, 0, 0 } ), center( B ) );
    EXPECT_EQ( ( Vector3 { 1, 1, 1 } ), size( B ) );
}

TEST( Box, normal )
{
    constexpr auto B = Box {};

    static_assert( normal( B, Point3 { 1, 0.5, -0.8 } ) == Normal3 { 1, 0, 0 } );
    static_assert( normal( B, Point3 { -1, -0.2, 0.9 } ) == Normal3 { -1, 0, 0 } );
    static_assert( normal( B, Point3 { -0.4, 1, -0.1 } ) == Normal3 { 0, 1, 0 } );
    static_assert( normal( B, Point3 { 0.3, -1, -0.7 } ) == Normal3 { 0, -1, 0 } );
    static_assert( normal( B, Point3 { -0.6, 0.3, 1 } ) == Normal3 { 0, 0, 1 } );
    static_assert( normal( B, Point3 { 0.4, 0.4, -1 } ) == Normal3 { 0, 0, -1 } );
    static_assert( normal( B, Point3 { 1, 1, 1 } ) == Normal3 { 1, 0, 0 } );
    static_assert( normal( B, Point3 { -1, -1, -1 } ) == Normal3 { -1, 0, 0 } );

    EXPECT_TRUE( true );
}
