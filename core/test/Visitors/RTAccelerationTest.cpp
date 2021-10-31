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

#include "Visitors/RTAcceleration.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Normal3Ops.hpp"
#include "Mathematics/Ray_apply.hpp"
#include "Mathematics/Sphere_normal.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_inverse.hpp"
#include "Mathematics/Transformation_scale.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/Vector3_isZero.hpp"
#include "Mathematics/Vector_equality.hpp"
#include "Mathematics/easing.hpp"
#include "Mathematics/intersect.hpp"
#include "Mathematics/io.hpp"
#include "Mathematics/min.hpp"
#include "Mathematics/reflect.hpp"
#include "Mathematics/refract.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( RTAcceleration, it_optimizes_scene_with_one_geometry )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >(
            [] {
                auto material = crimild::alloc< materials::PrincipledBSDF >();
                material->setAlbedo( ColorRGB { 1, 0, 0 } );
                return material;
            }() );
        geometry->setLocal( translation( 1, 2, 3 ) );
        return geometry;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( ( Vector4 { -1, -2, -3, 1 } ), optimized.nodes[ 0 ].invWorld.c3 );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( 0, optimized.nodes[ 0 ].index );
    ASSERT_EQ( 1, optimized.materials.size() );
    ASSERT_EQ( ( ColorRGB { 1, 0, 0 } ), optimized.materials[ 0 ].albedo );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_sphere )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( 0, optimized.nodes[ 0 ].index );
    ASSERT_EQ( 1, optimized.materials.size() );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_box )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::BOX ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_BOX, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( 0, optimized.nodes[ 0 ].index );
    ASSERT_EQ( 1, optimized.materials.size() );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_cylinder )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::CYLINDER ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_CYLINDER, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( 0, optimized.nodes[ 0 ].index );
    ASSERT_EQ( 1, optimized.materials.size() );
}

TEST( RTAcceleration, it_ignores_unsupported_geometry )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::TRIANGLES ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::INVALID, optimized.nodes[ 0 ].type );
}

TEST( RTAcceleration, it_ignores_geometries_without_primitives )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        return geometry;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::INVALID, optimized.nodes[ 0 ].type );
}

TEST( RTAcceleration, it_ignores_geometries_without_materials )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::CYLINDER ) );
        return geometry;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::INVALID, optimized.nodes[ 0 ].type );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_group )
{
    auto scene = [] {
        auto group = crimild::alloc< Group >();
        group->setLocal( translation( 1, 2, 3 ) );
        return group;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( 0, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].index );
    ASSERT_EQ( ( Vector4 { -1, -2, -3, 1 } ), optimized.nodes[ 0 ].invWorld.c3 );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_csg_union )
{
    auto scene = [] {
        auto csg = crimild::alloc< CSGNode >( CSGNode::Operator::UNION );
        csg->setLocal( translation( 1, 2, 3 ) );
        return csg;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::CSG_UNION, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].index );
    ASSERT_EQ( ( Vector4 { -1, -2, -3, 1 } ), optimized.nodes[ 0 ].invWorld.c3 );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_csg_intersection )
{
    auto scene = [] {
        auto csg = crimild::alloc< CSGNode >( CSGNode::Operator::INTERSECTION );
        csg->setLocal( translation( 1, 2, 3 ) );
        return csg;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::CSG_INTERSECTION, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].index );
    ASSERT_EQ( ( Vector4 { -1, -2, -3, 1 } ), optimized.nodes[ 0 ].invWorld.c3 );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_csg_difference )
{
    auto scene = [] {
        auto csg = crimild::alloc< CSGNode >( CSGNode::Operator::DIFFERENCE );
        csg->setLocal( translation( 1, 2, 3 ) );
        return csg;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 1, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::CSG_DIFFERENCE, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].index );
    ASSERT_EQ( ( Vector4 { -1, -2, -3, 1 } ), optimized.nodes[ 0 ].invWorld.c3 );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_child )
{
    auto scene = [] {
        auto group = crimild::alloc< Group >();
        group->attachNode( crimild::alloc< Group >() );
        return group;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 2, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( 1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( 1, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 1 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 1 ].firstChildIndex );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 1 ].index );
}

TEST( RTAcceleration, it_optimizes_scene_with_two_children )
{
    auto scene = [] {
        auto group = crimild::alloc< Group >();
        group->attachNode( crimild::alloc< Group >() );
        group->attachNode( crimild::alloc< Group >() );
        return group;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 3, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( 1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( 2, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 1 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 1 ].firstChildIndex );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 1 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 2 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 2 ].firstChildIndex );
    ASSERT_EQ( 0, optimized.nodes[ 2 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 2 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 2 ].index );
}

TEST( RTAcceleration, it_optimizes_scene_with_three_children )
{
    auto scene = [] {
        auto group = crimild::alloc< Group >();
        group->attachNode( crimild::alloc< Group >() );
        group->attachNode( crimild::alloc< Group >() );
        group->attachNode( crimild::alloc< Group >() );
        return group;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 4, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( 1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( 3, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 1 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 1 ].firstChildIndex );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 1 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 2 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 2 ].firstChildIndex );
    ASSERT_EQ( 0, optimized.nodes[ 2 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 2 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 2 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 3 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 3 ].firstChildIndex );
    ASSERT_EQ( 0, optimized.nodes[ 3 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 3 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 3 ].index );
}

TEST( RTAcceleration, it_optimizes_scene_with_basic_hierarchy )
{
    auto scene = [] {
        auto group = crimild::alloc< Group >();
        group->attachNode( crimild::alloc< Group >() );
        group->attachNode(
            [] {
                auto group = crimild::alloc< Group >();
                group->attachNode( crimild::alloc< Group >() );
                return group;
            }() );
        return group;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 4, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( 1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( 2, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 1 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 1 ].firstChildIndex );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 1 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 2 ].type );
    ASSERT_EQ( 3, optimized.nodes[ 2 ].firstChildIndex );
    ASSERT_EQ( 1, optimized.nodes[ 2 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 2 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 2 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 3 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 3 ].firstChildIndex );
    ASSERT_EQ( 0, optimized.nodes[ 3 ].childCount );
    ASSERT_EQ( 2, optimized.nodes[ 3 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 3 ].index );
}

TEST( RTAcceleration, it_optimizes_scene_with_mixed_hierarchy )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    };

    auto scene = [ & ] {
        auto group = crimild::alloc< Group >();
        group->attachNode( crimild::alloc< Group >() );
        group->attachNode(
            [ & ] {
                auto group = crimild::alloc< Group >();
                group->attachNode(
                    [ & ] {
                        auto group = crimild::alloc< Group >();
                        group->attachNode( geometry() );
                        group->attachNode( geometry() );
                        return group;
                    }() );
                group->attachNode( geometry() );
                return group;
            }() );
        group->attachNode( geometry() );
        return group;
    }();

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    ASSERT_EQ( 8, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( 1, optimized.nodes[ 0 ].firstChildIndex );
    ASSERT_EQ( 3, optimized.nodes[ 0 ].childCount );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 1 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 1 ].firstChildIndex );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 1 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 2 ].type );
    ASSERT_EQ( 4, optimized.nodes[ 2 ].firstChildIndex );
    ASSERT_EQ( 2, optimized.nodes[ 2 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 2 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 2 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 3 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 3 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 3 ].childCount );
    ASSERT_EQ( 0, optimized.nodes[ 3 ].parentIndex );
    ASSERT_EQ( 3, optimized.nodes[ 3 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 4 ].type );
    ASSERT_EQ( 6, optimized.nodes[ 4 ].firstChildIndex );
    ASSERT_EQ( 2, optimized.nodes[ 4 ].childCount );
    ASSERT_EQ( 2, optimized.nodes[ 4 ].parentIndex );
    ASSERT_EQ( -1, optimized.nodes[ 4 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 5 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 5 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 5 ].childCount );
    ASSERT_EQ( 2, optimized.nodes[ 5 ].parentIndex );
    ASSERT_EQ( 2, optimized.nodes[ 5 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 6 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 6 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 6 ].childCount );
    ASSERT_EQ( 4, optimized.nodes[ 6 ].parentIndex );
    ASSERT_EQ( 0, optimized.nodes[ 6 ].index );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 7 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 7 ].firstChildIndex );
    ASSERT_EQ( -1, optimized.nodes[ 7 ].childCount );
    ASSERT_EQ( 4, optimized.nodes[ 7 ].parentIndex );
    ASSERT_EQ( 1, optimized.nodes[ 7 ].index );
}

TEST( RTAcceleration, it_traverses_optimized_scene )
{
    auto geometry = []( auto name ) {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        geometry->setName( name );
        return geometry;
    };

    auto group = []( auto name, Array< SharedPointer< Node > > const &nodes ) {
        auto group = crimild::alloc< Group >();
        nodes.each(
            [ & ]( auto &node ) {
                group->attachNode( node );
            } );
        group->setName( name );
        return group;
    };

    auto scene = group(
        "A",
        {
            group( "B", {} ),
            group(
                "C",
                {
                    group(
                        "D",
                        {
                            geometry( "E" ),
                            geometry( "F" ),
                        } ),
                    geometry( "G" ),
                } ),
            geometry( "H" ),
        } );

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    std::stringstream ss;
    utils::traverseNonRecursive( optimized, [ & ]( const auto &, Index i ) { ss << i << ", "; return true; } );

    ASSERT_EQ( "0, 1, 2, 4, 6, 7, 5, 3, ", ss.str() );
}

TEST( RTAcceleration, it_traverses_optimized_scene_with_early_termination )
{
    auto geometry = []( auto name ) {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        geometry->setName( name );
        return geometry;
    };

    auto group = []( auto name, Array< SharedPointer< Node > > const &nodes ) {
        auto group = crimild::alloc< Group >();
        nodes.each(
            [ & ]( auto &node ) {
                group->attachNode( node );
            } );
        group->setName( name );
        return group;
    };

    auto scene = group(
        "A",
        {
            group( "B", {} ),
            group(
                "C",
                {
                    group(
                        "D",
                        {
                            geometry( "E" ),
                            geometry( "F" ),
                        } ),
                    geometry( "G" ),
                } ),
            geometry( "H" ),
        } );

    RTAcceleration optimize;
    scene->perform( optimize );
    auto optimized = optimize.getResult();

    std::stringstream ss;
    utils::traverseNonRecursive(
        optimized,
        [ & ]( const auto &, Index i ) {
            ss << i << ", ";
            return i != 2;
        } );

    ASSERT_EQ( "0, 1, 2, 3, ", ss.str() );
}
