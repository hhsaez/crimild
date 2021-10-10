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

TEST( intersect, ray_parallel_to_plane )
{
    constexpr auto P = Plane3 {};
    constexpr auto R = Ray3 { { 0, 10, 0 }, { 0, 0, 1 } };

    Real t;
    EXPECT_FALSE( intersect( R, P, t ) );
}

TEST( intersect, plane_and_coplanar_ray )
{
    constexpr auto P = Plane3 {};
    constexpr auto R = Ray3 { { 0, 0, 0 }, { 0, 0, 1 } };

    Real t;
    EXPECT_FALSE( intersect( R, P, t ) );
}

TEST( intersect, ray_intersecting_plane_from_above )
{
    constexpr auto P = Plane3 {};
    constexpr auto R = Ray3 { { 0, 1, 0 }, { 0, -1, 0 } };

    Real t;
    EXPECT_TRUE( intersect( R, P, t ) );
    EXPECT_EQ( 1, t );
}

TEST( intersect, ray_intersecting_plane_from_below )
{
    constexpr auto P = Plane3 {};
    constexpr auto R = Ray3 { { 0, -1, 0 }, { 0, 1, 0 } };

    Real t;
    EXPECT_TRUE( intersect( R, P, t ) );
    EXPECT_EQ( 1, t );
}

TEST( intersect, ray_and_box_right_face )
{
    const auto B = Box {};
    const auto R = Ray3 { { 5.0, 0.5, 0.0 }, { -1, 0, 0 } };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, B, t0, t1 ) );
    EXPECT_EQ( 4, t0 );
    EXPECT_EQ( 6, t1 );
}

TEST( intersect, ray_and_box_left_face )
{
    const auto B = Box {};

    const auto R = Ray3 { { -5.0, 0.5, 0.0 }, { 1, 0, 0 } };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, B, t0, t1 ) );
    EXPECT_EQ( 4, t0 );
    EXPECT_EQ( 6, t1 );
}

TEST( intersect, ray_and_box_top_face )
{
    const auto B = Box {};

    const auto R = Ray3 { { 0.5, 5.0, 0.0 }, { 0, -1, 0 } };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, B, t0, t1 ) );
    EXPECT_EQ( 4, t0 );
    EXPECT_EQ( 6, t1 );
}

TEST( intersect, ray_and_box_bottom_face )
{
    const auto B = Box {};

    const auto R = Ray3 { { 0.5, -5.0, 0.0 }, { 0, 1, 0 } };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, B, t0, t1 ) );
    EXPECT_EQ( 4, t0 );
    EXPECT_EQ( 6, t1 );
}

TEST( intersect, ray_and_box_front_face )
{
    const auto B = Box {};

    const auto R = Ray3 { { 0.5, 0, 5 }, { 0, 0, -1 } };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, B, t0, t1 ) );
    EXPECT_EQ( 4, t0 );
    EXPECT_EQ( 6, t1 );
}

TEST( intersect, ray_and_box_back_face )
{
    const auto B = Box {};

    const auto R = Ray3 { { 0.5, 0, -5 }, { 0, 0, 1 } };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, B, t0, t1 ) );
    EXPECT_EQ( 4, t0 );
    EXPECT_EQ( 6, t1 );
}

TEST( intersect, ray_and_box_from_inside )
{
    const auto B = Box {};

    const auto R = Ray3 { { 0, 0.5, 0 }, { 0, 0, 1 } };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, B, t0, t1 ) );
    EXPECT_TRUE( isZero( t0 ) );
    EXPECT_EQ( 1, t1 );
}

TEST( intersect, ray_and_custom_box )
{
    const auto B = Box { { 1, 0, 0 }, { 2, 2, 2 } };

    const auto R = Ray3 { { 2.5, 1.5, 5 }, { 0, 0, -1 } };

    Real t0, t1;
    EXPECT_TRUE( intersect( R, B, t0, t1 ) );
    EXPECT_EQ( 3, t0 );
    EXPECT_EQ( 7, t1 );
}

TEST( intersect, ray_misses_cube )
{
    const auto B = Box {};
    Real t0, t1;

    {
        const auto R = Ray3 { { -2, 0, 0 }, { 0.2673, 0.5345, 0.8018 } };
        EXPECT_FALSE( intersect( R, B, t0, t1 ) );
    }

    {
        const auto R = Ray3 { { 0, -2, 0 }, { 0.8018, 0.2673, 0.5345 } };
        EXPECT_FALSE( intersect( R, B, t0, t1 ) );
    }

    {
        const auto R = Ray3 { { 0, 0, -2 }, { 0.5345, 0.8018, 0.2673 } };
        EXPECT_FALSE( intersect( R, B, t0, t1 ) );
    }

    {
        const auto R = Ray3 { { 2, 0, 2 }, { 0, 0, -1 } };
        EXPECT_FALSE( intersect( R, B, t0, t1 ) );
        std::cout << t0 << " " << t1 << std::endl;
    }

    {
        const auto R = Ray3 { { 0, 2, 2 }, { 0, -1, 0 } };
        EXPECT_FALSE( intersect( R, B, t0, t1 ) );
        std::cout << t0 << " " << t1 << std::endl;
    }

    {
        const auto R = Ray3 { { 2, 2, 0 }, { -1, 0, 0 } };
        EXPECT_FALSE( intersect( R, B, t0, t1 ) );
        std::cout << t0 << " " << t1 << std::endl;
    }
}

TEST( intersect, ray_misses_cylinder )
{
    auto test = []( const Ray3 &R ) {
        const auto C = Cylinder {};
        Real t0, t1;
        EXPECT_FALSE( intersect( R, C, t0, t1 ) );
    };

    test( Ray3 { Point3 { 1, 0, 0 }, Vector3 { 0, 1, 0 } } );
    test( Ray3 { Point3 { 0, 0, 0 }, Vector3 { 0, 1, 0 } } );
    test( Ray3 { Point3 { 0, 0, -5 }, Vector3 { 1, 1, 1 } } );
}

TEST( intersect, ray_hits_cylinder )
{
    auto test = []( const Ray3 &R, Real x0, Real x1 ) {
        const auto C = Cylinder {};
        Real t0, t1;
        EXPECT_TRUE( intersect( R, C, t0, t1 ) );
        EXPECT_TRUE( isEqual( t0, x0 ) );
        EXPECT_TRUE( isEqual( t1, x1 ) );
    };

    test( Ray3 { Point3 { 1, 0, -5 }, Vector3 { 0, 0, 1 } }, 5, 5 );
    test( Ray3 { Point3 { 0, 0, -5 }, Vector3 { 0, 0, 1 } }, 4, 6 );
    test( Ray3 { Point3 { 0.5, 0, -5 }, normalize( Vector3 { 0.1, 1, 1 } ) }, 6.80798, 7.08872 );
}

TEST( intersect, ray_hits_cylinder_with_different_height )
{
    auto test = []( const Ray3 &R ) {
        const auto C = Cylinder { .height = 2 };
        Real t0, t1;
        EXPECT_TRUE( intersect( R, C, t0, t1 ) );
    };

    test( Ray3 { Point3 { 0, 0.15, 0 }, normalize( Vector3 { 0.1, 1, 0 } ) } );
    test( Ray3 { Point3 { 0, 3, -5 }, normalize( Vector3 { 0, 0, 1 } ) } );
    test( Ray3 { Point3 { 0, 0, -5 }, normalize( Vector3 { 0, 0, 1 } ) } );
    test( Ray3 { Point3 { 0, 2, -5 }, normalize( Vector3 { 0, 0, 1 } ) } );
    test( Ray3 { Point3 { 0, 1, -5 }, normalize( Vector3 { 0, 0, 1 } ) } );
    test( Ray3 { Point3 { 0, 1.5, -2 }, normalize( Vector3 { 0, 0, 1 } ) } );
}
