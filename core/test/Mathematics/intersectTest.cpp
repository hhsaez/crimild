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

#include "Mathematics/intersect.hpp"

#include "Mathematics/Transformation_scale.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( intersect, raySphereTwoRoots )
{
    constexpr auto S = Sphere {
        Point3::Constants::ZERO,
        Real( 1 ),
    };

    constexpr auto R = Ray3 {
        Point3 { 0, 0, -5 },
        Vector3 { 0, 0, 1 },
    };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, S, t0, t1 ) );
    EXPECT_EQ( Real( 4 ), t0 );
    EXPECT_EQ( Real( 6 ), t1 );
}

TEST( intersect, raySphereOneRoot )
{
    constexpr auto S = Sphere {
        Point3::Constants::ZERO,
        Real( 1 ),
    };

    constexpr auto R = Ray3 {
        Point3 { 0, 1, -5 },
        Vector3 { 0, 0, 1 },
    };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, S, t0, t1 ) );
    EXPECT_EQ( Real( 5 ), t0 );
    EXPECT_EQ( Real( 5 ), t1 );
}

TEST( intersect, raySphereNoIntersection )
{
    constexpr auto S = Sphere {
        Point3::Constants::ZERO,
        Real( 1 ),
    };

    constexpr auto R = Ray3 {
        Point3 { 0, 2, -5 },
        Vector3 { 0, 0, 1 },
    };

    Real t0, t1;
    EXPECT_FALSE( intersect( R, S, t0, t1 ) );
}

TEST( intersect, raySphereOriginInside )
{
    constexpr auto S = Sphere {
        Point3::Constants::ZERO,
        Real( 1 ),
    };

    constexpr auto R = Ray3 {
        Point3 { 0, 0, 0 },
        Vector3 { 0, 0, 1 },
    };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, S, t0, t1 ) );
    EXPECT_EQ( Real( -1 ), t0 );
    EXPECT_EQ( Real( 1 ), t1 );
}

TEST( intersect, sphereBehindRay )
{
    constexpr auto S = Sphere {
        Point3::Constants::ZERO,
        Real( 1 ),
    };

    constexpr auto R = Ray3 {
        Point3 { 0, 0, 5 },
        Vector3 { 0, 0, 1 },
    };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, S, t0, t1 ) );
    EXPECT_EQ( Real( -6 ), t0 );
    EXPECT_EQ( Real( -4 ), t1 );
}

TEST( intersect, rayAndTransformedSphere )
{
    constexpr auto S = Sphere {
        Point3::Constants::ZERO,
        Real( 1 ),
    };

    constexpr auto R = Ray3 {
        Point3 { 0, 0, -5 },
        Vector3 { 0, 0, 1 },
    };

    // Represents a world transforms for the sphere (as if it where a node)
    constexpr auto sphereWorld = scale( 2, 2, 2 );

    Real t0, t1;
    EXPECT_TRUE( intersect( R, S, sphereWorld, t0, t1 ) );
    EXPECT_EQ( Real( 3 ), t0 );
    EXPECT_EQ( Real( 7 ), t1 );
}