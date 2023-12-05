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

#include "Boundings/AABBBoundingVolume.hpp"

#include "Mathematics/Transformation_translation.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( AABBBoundingVolume, construction )
{
    auto bb = crimild::alloc< AABBBoundingVolume >();

    EXPECT_EQ( ( Point3f { 0, 0, 0 } ), bb->getCenter() );
    EXPECT_TRUE( isEqual( numbers::SQRT_3, bb->getRadius() ) );
    EXPECT_EQ( ( Point3f { -1, -1, -1 } ), bb->getMin() );
    EXPECT_EQ( ( Point3f { 1, 1, 1 } ), bb->getMax() );
}

TEST( AABBBoundingVolume, compute_from_simple_volume )
{
    auto bb = crimild::alloc< AABBBoundingVolume >();
    auto other = crimild::alloc< AABBBoundingVolume >();

    bb->computeFrom( crimild::get_ptr( other ) );

    EXPECT_EQ( ( Point3f { 0, 0, 0 } ), bb->getCenter() );
    EXPECT_TRUE( isEqual( numbers::SQRT_3, bb->getRadius() ) );
    EXPECT_EQ( ( Point3f { -1, -1, -1 } ), bb->getMin() );
    EXPECT_EQ( ( Point3f { 1, 1, 1 } ), bb->getMax() );
}

TEST( AABBBoundingVolume, compute_from_transformed_volume )
{
    auto bb = crimild::alloc< AABBBoundingVolume >();
    auto other = crimild::alloc< AABBBoundingVolume >();

    bb->computeFrom( crimild::get_ptr( other ), translation( -1, 2, 3 ) );

    EXPECT_EQ( ( Point3f { -1, 2, 3 } ), bb->getCenter() );
    EXPECT_TRUE( isEqual( numbers::SQRT_3, bb->getRadius() ) );
    EXPECT_EQ( ( Point3f { -1 - 1, 2 - 1, 3 - 1 } ), bb->getMin() );
    EXPECT_EQ( ( Point3f { -1 + 1, 2 + 1, 3 + 1 } ), bb->getMax() );
}

TEST( AABBBoundingVolume, ray_intersection_hit )
{
    auto bb = crimild::alloc< AABBBoundingVolume >();

    const auto R = Ray3 { { 0, 0, 5 }, { 0, 0, -1 } };

    EXPECT_TRUE( bb->testIntersection( R ) );
}

TEST( AABBBoundingVolume, ray_intersection_hit_sphere )
{
    auto bb = crimild::alloc< AABBBoundingVolume >();

    const auto R = Ray3 { { numbers::SQRT_3 - numbers::EPSILON, 0, 5 }, { 0, 0, -1 } };

    EXPECT_FALSE( bb->testIntersection( R ) );
}

TEST( AABBBoundingVolume, no_ray_intersection )
{
    auto bb = crimild::alloc< AABBBoundingVolume >();

    const auto R = Ray3 { { numbers::SQRT_3 + numbers::EPSILON, 0, 5 }, { 0, 0, -1 } };

    EXPECT_FALSE( bb->testIntersection( R ) );
}
