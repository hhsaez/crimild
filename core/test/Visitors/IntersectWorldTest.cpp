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

#include "Visitors/IntersectWorld.hpp"

#include "Mathematics/Ray3.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "Visitors/UpdateWorldState.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( IntersectWorld, intersect_world_with_a_ray )
{
    auto world = crimild::alloc< Group >();
    world->attachNode(
        [] {
            auto geometry = crimild::alloc< Geometry >();
            return geometry;
        }() );
    world->attachNode(
        [] {
            auto geometry = crimild::alloc< Geometry >();
            geometry->setLocal( scale( 0.5 ) );
            return geometry;
        }() );
    world->perform( UpdateWorldState() );

    const auto R = Ray3 { { 0, 0, -5 }, { 0, 0, 1 } };

    auto results = IntersectWorld::Results {};
    world->perform( IntersectWorld( R, results ) );

    EXPECT_EQ( 4, results.size() );
    EXPECT_EQ( 4, results[ 0 ].t );
    EXPECT_EQ( 4.5, results[ 1 ].t );
    EXPECT_EQ( 5.5, results[ 2 ].t );
    EXPECT_EQ( 6, results[ 3 ].t );
}

TEST( IntersectWorld, precompute_intersection_results )
{
    auto world = crimild::alloc< Group >();
    world->attachNode(
        [] {
            auto geometry = crimild::alloc< Geometry >();
            return geometry;
        }() );
    world->perform( UpdateWorldState() );

    const auto R = Ray3 { { 0, 0, -5 }, { 0, 0, 1 } };

    auto results = IntersectWorld::Results {};
    world->perform( IntersectWorld( R, results ) );

    EXPECT_EQ( 2, results.size() );
    EXPECT_EQ( 4, results[ 0 ].t );
    EXPECT_EQ( world->getNodeAt( 0 ), results[ 0 ].geometry );
    EXPECT_EQ( ( Point3 { 0, 0, -1 } ), results[ 0 ].point );
    EXPECT_EQ( ( Normal3 { 0, 0, -1 } ), results[ 0 ].normal );
}

TEST( IntersectWorld, intersection_occurs_on_the_outside )
{
    auto world = crimild::alloc< Group >();
    world->attachNode(
        [] {
            auto geometry = crimild::alloc< Geometry >();
            return geometry;
        }() );
    world->perform( UpdateWorldState() );

    const auto R = Ray3 { { 0, 0, -5 }, { 0, 0, 1 } };

    auto results = IntersectWorld::Results {};
    world->perform( IntersectWorld( R, results ) );

    EXPECT_EQ( 2, results.size() );
    EXPECT_EQ( 4, results[ 0 ].t );
    EXPECT_EQ( world->getNodeAt( 0 ), results[ 0 ].geometry );
    EXPECT_EQ( ( Point3 { 0, 0, -1 } ), results[ 0 ].point );
    EXPECT_EQ( ( Normal3 { 0, 0, -1 } ), results[ 0 ].normal );
    EXPECT_EQ( true, results[ 0 ].frontFace );
}

TEST( IntersectWorld, intersection_occurs_on_the_inside )
{
    auto world = crimild::alloc< Group >();
    world->attachNode(
        [] {
            auto geometry = crimild::alloc< Geometry >();
            return geometry;
        }() );
    world->perform( UpdateWorldState() );

    const auto R = Ray3 { { 0, 0, 0 }, { 0, 0, 1 } };

    auto results = IntersectWorld::Results {};
    world->perform( IntersectWorld( R, results ) );

    EXPECT_EQ( 1, results.size() );
    EXPECT_EQ( 1, results[ 0 ].t );
    EXPECT_EQ( world->getNodeAt( 0 ), results[ 0 ].geometry );
    EXPECT_EQ( ( Point3 { 0, 0, 1 } ), results[ 0 ].point );
    EXPECT_EQ( ( Normal3 { 0, 0, -1 } ), results[ 0 ].normal );
    EXPECT_EQ( false, results[ 0 ].frontFace );
}

TEST( IntersectWorld, no_intersection )
{
    auto world = crimild::alloc< Group >();
    world->attachNode(
        [] {
            auto geometry = crimild::alloc< Geometry >();
            return geometry;
        }() );
    world->perform( UpdateWorldState() );

    const auto R = Ray3 { { 0, 0, -5 }, { 0, 0, -1 } };

    auto results = IntersectWorld::Results {};
    world->perform( IntersectWorld( R, results ) );

    EXPECT_EQ( 0, results.size() );
}
