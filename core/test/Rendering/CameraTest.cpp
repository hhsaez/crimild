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

#include "SceneGraph/Camera.hpp"

#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_lookAt.hpp"
#include "Mathematics/Transformation_operators.hpp"
#include "Mathematics/Transformation_rotation.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/io.hpp"
#include "SceneGraph/Group.hpp"
#include "Visitors/FetchCameras.hpp"
#include "Visitors/SelectNodes.hpp"
#include "Visitors/UpdateWorldState.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( Camera, construction )
{
    auto camera = crimild::alloc< Camera >();
}

TEST( Camera, view_matrix )
{
    auto camera = crimild::alloc< Camera >();

    camera->setLocal(
        lookAt(
            Point3 { 1, 3, 2 },
            Point3 { 4, -2, 8 },
            Vector3 { 1, 1, 0 }
        )
    );
    camera->perform( UpdateWorldState() );

    constexpr auto M = Matrix4 {
        { -0.514496, 0.778924, -0.358569, 0.000000 },
        { 0.514496, 0.614940, 0.597614, -0.000000 },
        { 0.685994, 0.122988, -0.717137, 0.000000 },
        { -2.400980, -2.869720, 0.000000, 1.000000 },
    };

    const auto view = camera->getViewMatrix();

    // always compare with inverse matrix
    EXPECT_TRUE( isEqual( M, view ) );
}

TEST( Camera, get_ray_through_the_center_of_the_canvas )
{
    const auto width = Int( 202 );
    const auto height = Int( 102 );
    const auto fov = numbers::PI_DIV_2;
    const auto aspect = Real( width ) / Real( height );

    auto camera = crimild::alloc< Camera >( fov, aspect, 0.1, 100 );

    auto px = Real( 101 ) / Real( width );
    auto py = Real( 51 ) / Real( height );
    Ray3 ray;

    camera->perform( UpdateWorldState() );

    EXPECT_TRUE( camera->getPickRay( px, py, ray ) );

    EXPECT_EQ( Point3::Constants::ZERO, origin( ray ) );
    EXPECT_EQ( Vector3::Constants::FORWARD, direction( ray ) );
}

TEST( Camera, get_ray_through_the_corner_of_the_canvas )
{
    const auto width = Int( 201 );
    const auto height = Int( 101 );
    const auto fov = numbers::PI_DIV_2;
    const auto aspect = Real( width ) / Real( height );

    auto camera = crimild::alloc< Camera >( fov, aspect, 0.1, 100 );

    Ray3 ray;

    camera->perform( UpdateWorldState() );

    EXPECT_TRUE( camera->getPickRay( 0, 0, ray ) );

    EXPECT_TRUE( isEqual( Point3::Constants::ZERO, origin( ray ) ) );
    EXPECT_TRUE( isEqual( normalize( Vector3 { -0.027282, 0.013709, -1.000000 } ), direction( ray ) ) );
}

TEST( Camera, get_ray_for_translated_camera )
{
    const auto width = Int( 201 );
    const auto height = Int( 101 );
    const auto fov = numbers::PI_DIV_2;
    const auto aspect = Real( width ) / Real( height );

    auto camera = crimild::alloc< Camera >( fov, aspect, 0.1, 100 );

    camera->setLocal( translation( 0, -2, 5 ) );

    auto px = Real( 100 + 0.5 ) / Real( width );
    auto py = Real( 50 + 0.5 ) / Real( height );
    Ray3 ray;

    camera->perform( UpdateWorldState() );

    EXPECT_TRUE( camera->getPickRay( px, py, ray ) );

    EXPECT_EQ( ( Point3 { 0, -2, 5 } ), origin( ray ) );
    EXPECT_EQ( Vector3::Constants::FORWARD, direction( ray ) );
}

TEST( Camera, get_ray_for_rotated_camera )
{
    const auto width = Int( 201 );
    const auto height = Int( 101 );
    const auto fov = numbers::PI_DIV_2;
    const auto aspect = Real( width ) / Real( height );

    auto camera = crimild::alloc< Camera >( fov, aspect, 0.1, 100 );

    camera->setLocal( rotationY( -numbers::PI_DIV_2 ) );

    auto px = Real( 100 + 0.5 ) / Real( width );
    auto py = Real( 50 + 0.5 ) / Real( height );
    Ray3 ray;

    camera->perform( UpdateWorldState() );

    EXPECT_TRUE( camera->getPickRay( px, py, ray ) );

    EXPECT_TRUE( isEqual( Point3::Constants::ZERO, origin( ray ) ) );
    EXPECT_TRUE( isEqual( Vector3::Constants::RIGHT, direction( ray ) ) );
}

TEST( Camera, get_ray_for_transformed_camera )
{
    const auto width = Int( 201 );
    const auto height = Int( 101 );
    const auto fov = numbers::PI_DIV_2;
    const auto aspect = Real( width ) / Real( height );

    auto camera = crimild::alloc< Camera >( fov, aspect, 0.1, 100 );

    camera->setLocal( translation( 2, 1, -1 ) * rotationX( -numbers::PI_DIV_4 ) );

    auto px = Real( 100 + 0.5 ) / Real( width );
    auto py = Real( 50 + 0.5 ) / Real( height );
    Ray3 ray;

    camera->perform( UpdateWorldState() );

    EXPECT_TRUE( camera->getPickRay( px, py, ray ) );

    EXPECT_EQ( ( Point3 { 2, 1, -1 } ), origin( ray ) );
    EXPECT_EQ( ( Vector3 { 0, -numbers::SQRT_2_DIV_2, -numbers::SQRT_2_DIV_2 } ), direction( ray ) );
}

TEST( Camera, get_ray_for_camera_using_lookAt )
{
    const auto width = Int( 200 );
    const auto height = Int( 100 );
    const auto fov = numbers::PI_DIV_2;
    const auto aspect = Real( width ) / Real( height );

    auto camera = crimild::alloc< Camera >( fov, aspect, 0.1, 100 );

    camera->setLocal(
        lookAt(
            Point3 { 0, -2, 5 },
            Point3 { 1, -2, 5 },
            Vector3 { 0, 1, 0 }
        )
    );

    auto px = Real( 100 ) / Real( width );
    auto py = Real( 50 ) / Real( height );

    Ray3 ray;

    camera->perform( UpdateWorldState() );

    EXPECT_TRUE( camera->getPickRay( px, py, ray ) );

    EXPECT_EQ( ( Point3 { 0, -2, 5 } ), origin( ray ) );
    EXPECT_EQ( Vector3::Constants::RIGHT, direction( ray ) );
}

TEST( Camera, fetchCameras )
{
    auto scene = crimild::alloc< Group >();
    auto camera = crimild::alloc< Camera >();
    scene->attachNode( camera );

    FetchCameras fetchCameras;
    scene->perform( fetchCameras );
    int i = 0;
    fetchCameras.forEachCamera( [ &i, camera ]( Camera *c ) {
        EXPECT_EQ( crimild::get_ptr( camera ), c );
        i++;
    } );
    EXPECT_EQ( 1, i );
}

TEST( Camera, coding )
{
    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();

    {
        auto camera = crimild::alloc< Camera >();
        camera->setFocusDistance( 20 );
        camera->setAperture( 2 );

        encoder->encode( camera );
    }

    {
        decoder->fromBytes( encoder->getBytes() );

        auto camera = decoder->getObjectAt< Camera >( 0 );
        EXPECT_TRUE( camera != nullptr );
        EXPECT_EQ( 20, camera->getFocusDistance() );
        EXPECT_EQ( 2, camera->getAperture() );
    }
}
