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

#include "Mathematics/Sphere.hpp"

#include "Mathematics/Sphere_normal.hpp"
#include "Mathematics/Transformation_operators.hpp"
#include "Mathematics/Transformation_rotation.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "Mathematics/isEqual.hpp"

#include "gtest/gtest.h"
#include <sstream>

using namespace crimild;

TEST( Sphere, construction )
{
    constexpr auto S = Sphere {
        Point3::Constants::ZERO,
        Real( 1 ),
    };

    static_assert( center( S ) == Point3::Constants::ZERO );
    static_assert( radius( S ) == Real( 1 ) );

    EXPECT_TRUE( true );
}

TEST( Sphere, normal )
{
    constexpr auto S = Sphere {};

    static_assert( normal( S, Point3 { 1, 0, 0 } ) == Normal3 { 1, 0, 0 } );
    static_assert( normal( S, Point3 { 0, 1, 0 } ) == Normal3 { 0, 1, 0 } );
    static_assert( normal( S, Point3 { 0, 0, 1 } ) == Normal3 { 0, 0, 1 } );
    static_assert( normal( S, Point3 { numbers::SQRT_3_DIV_3, numbers::SQRT_3_DIV_3, numbers::SQRT_3_DIV_3 } ) == Normal3 { numbers::SQRT_3_DIV_3, numbers::SQRT_3_DIV_3, numbers::SQRT_3_DIV_3 } );
    static_assert( length( normal( S, Point3 { numbers::SQRT_3_DIV_3, numbers::SQRT_3_DIV_3, numbers::SQRT_3_DIV_3 } ) ) == Real( 1 ) );

    EXPECT_TRUE(
        isEqual(
            Normal3 { 0, 0.97014, -0.24254 },
            normal(
                S,
                scale( 1, 0.5, 1 ) * rotationZ( numbers::PI / 5 ),
                Point3 { 0, numbers::SQRT_2_DIV_2, -numbers::SQRT_2_DIV_2 }
            )
        )
    );

    EXPECT_TRUE( true );
}

TEST( Sphere, negate_normal )
{
    constexpr auto S = Sphere {};

    EXPECT_EQ( ( Normal3 { -1, 0, 0 } ), normal( S, scale( -1 ), Point3 { 1, 0, 0 } ) );
}
