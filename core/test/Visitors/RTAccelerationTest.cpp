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
#include "Crimild_Mathematics.hpp"
#include "Primitives/BoxPrimitive.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "Visitors/BinTreeScene.hpp"

#include "gtest/gtest.h"

/**
 * 1. Generate balanced scene tree with only geometries (skip CSG)
 * 2. Generate linear scene
 *
 *
 * TLAccelNode (geometries + simple primitives)
 * BLAccelNode (triangles)
 */

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
            }()
        );
        geometry->setLocal( translation( 1, 2, 3 ) );
        return geometry;
    }();

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 2, optimized.nodes.size() );
    ASSERT_EQ( ( Point3f { 1, 2, 3 } ), origin( optimized.nodes[ 0 ].world ) );
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 0 ].type );

    ASSERT_EQ( ( Point3f { -1, -2, -3 } ), origin( inverse( optimized.nodes[ 1 ].world ) ) );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 1 ].type );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].materialIndex );

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

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 2, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 0 ].type );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 1 ].type );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].materialIndex );

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

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 2, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 0 ].type );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_BOX, optimized.nodes[ 1 ].type );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].materialIndex );

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

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 2, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 0 ].type );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_CYLINDER, optimized.nodes[ 1 ].type );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].materialIndex );

    ASSERT_EQ( 1, optimized.materials.size() );
}

TEST( RTAcceleration, it_optimizes_scene_with_triangles )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive(
            [] {
                auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
                primitive->setVertexData(
                    {
                        crimild::alloc< VertexBuffer >(
                            VertexP3N3TC2::getLayout(),
                            Array< VertexP3N3TC2 > {
                                {
                                    .position = Vector3 { 1, 2, 3 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 0, 0 },
                                },
                                {
                                    .position = Vector3 { 4, 5, 6 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 0, 1 },
                                },
                                {
                                    .position = Vector3 { 7, 8, 9 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 1, 1 },
                                },
                            }
                        ),
                    }
                );
                primitive->setIndices( crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, Array< UInt32 > { 0, 1, 2 } ) );
                return primitive;
            }()
        );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 2, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 0 ].type );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_TRIANGLES, optimized.nodes[ 1 ].type );
    ASSERT_EQ( 0, optimized.nodes[ 1 ].materialIndex );

    ASSERT_EQ( 1, optimized.materials.size() );
}

TEST( RTAcceleration, it_ignores_unsupported_geometry )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::LINES ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    RTAcceleration accelerate;
    scene->perform( accelerate );
    auto optimized = accelerate.getResult();

    ASSERT_EQ( 0, optimized.nodes.size() );
    ASSERT_EQ( 0, optimized.materials.size() );
}

TEST( RTAcceleration, it_ignores_geometries_without_primitives )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        return geometry;
    }();

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 0, optimized.nodes.size() );
    ASSERT_EQ( 0, optimized.materials.size() );
}

TEST( RTAcceleration, it_ignores_geometries_without_materials )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::CYLINDER ) );
        return geometry;
    }();

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 0, optimized.nodes.size() );
    ASSERT_EQ( 0, optimized.materials.size() );
}

TEST( RTAcceleration, it_discards_empty_groups )
{
    auto scene = [] {
        auto group = crimild::alloc< Group >();
        group->setLocal( translation( 1, 2, 3 ) );
        return group;
    }();

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 0, optimized.nodes.size() );
    ASSERT_EQ( 0, optimized.materials.size() );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_csg_union )
{
    auto scene = [] {
        auto csg = crimild::alloc< CSGNode >( CSGNode::Operator::UNION );
        csg->setLeft(
            [] {
                auto geometry = crimild::alloc< Geometry >();
                geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
                geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
                return geometry;
            }()
        );
        csg->setLocal( translation( 1, 2, 3 ) );
        return csg;
    }();

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 3, optimized.nodes.size() );
    ASSERT_EQ( RTAcceleratedNode::Type::CSG_UNION, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].secondChildIndex );
    ASSERT_EQ( ( Point3f { 1, 2, 3 } ), origin( optimized.nodes[ 0 ].world ) );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 1 ].type );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 2 ].type );

    ASSERT_EQ( 1, optimized.materials.size() );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_csg_intersection )
{
    auto scene = [] {
        auto csg = crimild::alloc< CSGNode >( CSGNode::Operator::INTERSECTION );
        csg->setLeft(
            [] {
                auto geometry = crimild::alloc< Geometry >();
                geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
                geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
                return geometry;
            }()
        );
        csg->setLocal( translation( 1, 2, 3 ) );
        return csg;
    }();

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 3, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::CSG_INTERSECTION, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].secondChildIndex );
    ASSERT_EQ( ( Point3f { 1, 2, 3 } ), origin( optimized.nodes[ 0 ].world ) );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 1 ].type );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 2 ].type );

    ASSERT_EQ( 1, optimized.materials.size() );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_csg_difference )
{
    auto scene = [] {
        auto csg = crimild::alloc< CSGNode >( CSGNode::Operator::DIFFERENCE );
        csg->setLeft(
            [] {
                auto geometry = crimild::alloc< Geometry >();
                geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
                geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
                return geometry;
            }()
        );
        csg->setLocal( translation( 1, 2, 3 ) );
        return csg;
    }();

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 3, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::CSG_DIFFERENCE, optimized.nodes[ 0 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].secondChildIndex );
    ASSERT_EQ( ( Point3f { 1, 2, 3 } ), origin( optimized.nodes[ 0 ].world ) );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 1 ].type );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 2 ].type );

    ASSERT_EQ( 1, optimized.materials.size() );
}

TEST( RTAcceleration, it_optimizes_scene_with_a_child )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    };

    auto scene = [ & ] {
        auto group = crimild::alloc< Group >();
        group->attachNode( geometry() );
        return group;
    }();

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 2, optimized.nodes.size() );
    ASSERT_EQ( 1, optimized.materials.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 0 ].type );

    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 1 ].type );
    ASSERT_EQ( -1, optimized.nodes[ 0 ].primitiveIndex );
    ASSERT_EQ( 0, optimized.nodes[ 0 ].materialIndex );
}

TEST( RTAcceleration, it_optimizes_scene_with_two_children )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    };

    auto scene = [ & ] {
        auto group = crimild::alloc< Group >();
        group->attachNode( geometry() );
        group->attachNode( geometry() );
        return group;
    }();

    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 5, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( 3, optimized.nodes[ 0 ].secondChildIndex );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 1 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 2 ].type );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 3 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 4 ].type );
}

TEST( RTAcceleration, it_optimizes_scene_with_three_children )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    };

    /**
     * Original scene:
     *       A
     *    /  |  \
     *    B  C  D
     */
    auto scene = [ & ] {
        auto group = crimild::alloc< Group >();
        group->attachNode( geometry() );
        group->attachNode( geometry() );
        group->attachNode( geometry() );
        return group;
    }();

    /**
     * Optimized scene:
     *      A
     *    /   \
     *   X     D
     * /   \
     * B    C
     */
    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 8, optimized.nodes.size() );

    // A
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( 6, optimized.nodes[ 0 ].secondChildIndex );

    // X
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 1 ].type );
    ASSERT_EQ( 4, optimized.nodes[ 1 ].secondChildIndex );

    // B
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 2 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 3 ].type );

    // C
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 4 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 5 ].type );

    // D
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 6 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 7 ].type );
}

TEST( RTAcceleration, it_optimizes_scene_with_basic_hierarchy )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    };

    /**
     * Original scene:
     *      A
     *    /   \
     *    B   C
     *        |
     *        D
     */
    auto scene = [ & ] {
        auto group = crimild::alloc< Group >();
        group->attachNode( geometry() );
        group->attachNode(
            [ & ] {
                auto group = crimild::alloc< Group >();
                group->attachNode( geometry() );
                return group;
            }()
        );
        return group;
    }();

    /**
     * Optimized scene:
     *      A
     *    /   \
     *    B   D
     */
    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 5, optimized.nodes.size() );

    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( 3, optimized.nodes[ 0 ].secondChildIndex );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 1 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 2 ].type );

    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 3 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 4 ].type );
}

TEST( RTAcceleration, it_handles_a_parent_with_many_children )
{
    auto geometry = []( std::string name ) {
        auto geometry = crimild::alloc< Geometry >( name );
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    };

    /**
     * Original scene:
     *       A
     *   ____|____
     *  /| | | | |\
     * B C D E F G H
     */
    auto scene = crimild::alloc< Group >( "A" );
    scene->attachNode( geometry( "B" ) );
    scene->attachNode( geometry( "C" ) );
    scene->attachNode( geometry( "D" ) );
    scene->attachNode( geometry( "E" ) );
    scene->attachNode( geometry( "F" ) );
    scene->attachNode( geometry( "G" ) );
    scene->attachNode( geometry( "H" ) );
    scene->attachNode( geometry( "I" ) );

    /**
     * Optimized scene:
     *        A
     *    z       w
     *  y   X   v   u
     * B C D E F G H I
     */
    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 23, optimized.nodes.size() );

    // A
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( 12, optimized.nodes[ 0 ].secondChildIndex ); // w

    // z
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 1 ].type );
    ASSERT_EQ( 7, optimized.nodes[ 1 ].secondChildIndex ); // X

    // y
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 2 ].type );
    ASSERT_EQ( 5, optimized.nodes[ 2 ].secondChildIndex ); // C

    // B
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 3 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 4 ].type );

    // C
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 5 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 6 ].type );

    // X
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 7 ].type );
    ASSERT_EQ( 10, optimized.nodes[ 7 ].secondChildIndex ); // E

    // D
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 8 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 9 ].type );

    // E
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 10 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 11 ].type );

    // w
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 12 ].type );
    ASSERT_EQ( 18, optimized.nodes[ 12 ].secondChildIndex ); // u

    // v
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 13 ].type );
    ASSERT_EQ( 16, optimized.nodes[ 13 ].secondChildIndex ); // G

    // F
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 14 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 15 ].type );

    // G
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 16 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 17 ].type );

    // u
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 18 ].type );
    ASSERT_EQ( 21, optimized.nodes[ 18 ].secondChildIndex ); // I

    // H
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 19 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 20 ].type );

    // I
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 21 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 22 ].type );
}

TEST( RTAcceleration, it_optimizes_scene_with_mixed_hierarchy )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    };

    /**
     * Original scene:
     *       A
     *    /  |  \
     *   B   C   D
     *      / \
     *     E   F
     *   / | \
     *  G  H  I
     */
    auto scene = [ & ] {
        auto group = crimild::alloc< Group >();
        group->attachNode( geometry() );
        group->attachNode(
            [ & ] {
                auto group = crimild::alloc< Group >();
                group->attachNode(
                    [ & ] {
                        auto group = crimild::alloc< Group >();
                        group->attachNode( geometry() );
                        group->attachNode( geometry() );
                        group->attachNode( geometry() );
                        return group;
                    }()
                );
                group->attachNode( geometry() );
                return group;
            }()
        );
        group->attachNode( geometry() );
        return group;
    }();

    /**
     * Original scene:
     *        A
     *      /   \
     *     X     D
     *   /   \
     *  B     C
     *      /   \
     *     E     F
     *   /  \
     *  Y    I
     * / \
     * G  H
     */
    auto optimized = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    ASSERT_EQ( 17, optimized.nodes.size() );

    // A
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 0 ].type );
    ASSERT_EQ( 15, optimized.nodes[ 0 ].secondChildIndex ); // D

    // X
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 1 ].type );
    ASSERT_EQ( 4, optimized.nodes[ 1 ].secondChildIndex ); // C

    // B
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 2 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 3 ].type );

    // C
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 4 ].type );
    ASSERT_EQ( 13, optimized.nodes[ 4 ].secondChildIndex ); // F

    // E
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 5 ].type );
    ASSERT_EQ( 11, optimized.nodes[ 5 ].secondChildIndex ); // I

    // Y
    ASSERT_EQ( RTAcceleratedNode::Type::GROUP, optimized.nodes[ 6 ].type );
    ASSERT_EQ( 9, optimized.nodes[ 6 ].secondChildIndex ); // H

    // G
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 7 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 8 ].type );

    // H
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 9 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 10 ].type );

    // I
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 11 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 12 ].type );

    // F
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 13 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 14 ].type );

    // D
    ASSERT_EQ( RTAcceleratedNode::Type::GEOMETRY, optimized.nodes[ 15 ].type );
    ASSERT_EQ( RTAcceleratedNode::Type::PRIMITIVE_SPHERE, optimized.nodes[ 16 ].type );
}

TEST( RTPrimAccel, ignore_empty_primitives )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::TRIANGLES ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    auto optimized = geometry->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    EXPECT_EQ( 0, optimized.nodes.size() );
    EXPECT_EQ( 0, optimized.materials.size() );
    EXPECT_EQ( 0, optimized.primitives.triangles.size() );
    EXPECT_EQ( 0, optimized.primitives.indices.size() );
    EXPECT_EQ( 0, optimized.primitives.indexOffsets.size() );
    EXPECT_EQ( 0, optimized.primitives.primTree.size() );
}

TEST( RTPrimAccel, ignore_non_triangles )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    auto optimized = geometry->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    EXPECT_EQ( 2, optimized.nodes.size() );
    EXPECT_EQ( 1, optimized.materials.size() );
    EXPECT_EQ( 0, optimized.primitives.triangles.size() );
    EXPECT_EQ( 0, optimized.primitives.indices.size() );
    EXPECT_EQ( 0, optimized.primitives.indexOffsets.size() );
    EXPECT_EQ( 0, optimized.primitives.primTree.size() );
}

TEST( RTPrimAccel, ignore_degenerated_triangle )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive(
            [] {
                auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
                primitive->setVertexData(
                    {
                        crimild::alloc< VertexBuffer >(
                            VertexP3N3TC2::getLayout(),
                            Array< VertexP3N3TC2 > {
                                {
                                    .position = Vector3 { 1, 2, 3 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 0, 0 },
                                },
                            }
                        ),
                    }
                );
                primitive->setIndices( crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, Array< UInt32 > { 0 } ) );
                return primitive;
            }()
        );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    auto optimized = geometry->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    EXPECT_EQ( 0, optimized.nodes.size() );
}

TEST( RTPrimAccel, ignore_invalid_vertex_layout )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive(
            [] {
                auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
                primitive->setVertexData(
                    {
                        crimild::alloc< VertexBuffer >(
                            VertexP3::getLayout(),
                            Array< VertexP3 > {
                                {
                                    .position = Vector3 { 1, 2, 3 },
                                },
                                {
                                    .position = Vector3 { 4, 5, 6 },
                                },
                                {
                                    .position = Vector3 { 7, 8, 9 },
                                },
                            }
                        ),
                    }
                );
                primitive->setIndices( crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, Array< UInt32 > { 0, 1, 2 } ) );
                return primitive;
            }()
        );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    auto optimized = geometry->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    EXPECT_EQ( 0, optimized.nodes.size() );
    EXPECT_EQ( 0, optimized.primitives.triangles.size() );
    EXPECT_EQ( 0, optimized.primitives.indices.size() );
    EXPECT_EQ( 0, optimized.primitives.indexOffsets.size() );
    EXPECT_EQ( 0, optimized.primitives.primTree.size() );
}

TEST( RTPrimAccel, one_triangle )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive(
            [] {
                auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
                primitive->setVertexData(
                    {
                        crimild::alloc< VertexBuffer >(
                            VertexP3N3TC2::getLayout(),
                            Array< VertexP3N3TC2 > {
                                {
                                    .position = Vector3 { 1, 2, 3 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 0, 0 },
                                },
                                {
                                    .position = Vector3 { 4, 5, 6 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 0, 1 },
                                },
                                {
                                    .position = Vector3 { 7, 8, 9 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 1, 1 },
                                },
                            }
                        ),
                    }
                );
                primitive->setIndices( crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, Array< UInt32 > { 0, 1, 2 } ) );
                return primitive;
            }()
        );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    auto optimized = geometry->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    EXPECT_EQ( 2, optimized.nodes.size() );
    EXPECT_EQ( 0, optimized.nodes[ 1 ].primitiveIndex );

    EXPECT_EQ( 3, optimized.primitives.triangles.size() );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 1, 2, 3 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 0, 0 },
            }
        ),
        optimized.primitives.triangles[ 0 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 4, 5, 6 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 0, 1 },
            }
        ),
        optimized.primitives.triangles[ 1 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 7, 8, 9 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 1, 1 },
            }
        ),
        optimized.primitives.triangles[ 2 ]
    );

    EXPECT_EQ( 3, optimized.primitives.indices.size() );
    EXPECT_EQ( 0, optimized.primitives.indices[ 0 ] );
    EXPECT_EQ( 1, optimized.primitives.indices[ 1 ] );
    EXPECT_EQ( 2, optimized.primitives.indices[ 2 ] );

    EXPECT_EQ( 1, optimized.primitives.indexOffsets.size() );
    EXPECT_EQ( 0, optimized.primitives.indexOffsets[ 0 ] );

    EXPECT_EQ( 1, optimized.primitives.primTree.size() );
    EXPECT_TRUE( optimized.primitives.primTree[ 0 ].isLeaf() );
    EXPECT_EQ( 0, optimized.primitives.primTree[ 0 ].primitiveIndicesOffset );
    EXPECT_EQ( 1, optimized.primitives.primTree[ 0 ].getPrimCount() );
}

TEST( RTPrimAccel, many_triangles )
{
    auto geometry = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive(
            [] {
                auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
                primitive->setVertexData(
                    {
                        crimild::alloc< VertexBuffer >(
                            VertexP3N3TC2::getLayout(),
                            Array< VertexP3N3TC2 > {
                                {
                                    .position = Vector3 { 1, 2, 3 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 0, 0 },
                                },
                                {
                                    .position = Vector3 { 4, 5, 6 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 0, 1 },
                                },
                                {
                                    .position = Vector3 { 7, 8, 9 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 1, 1 },
                                },
                                {
                                    .position = Vector3 { 10, 11, 12 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 1, 0 },
                                },
                            }
                        ),
                    }
                );
                primitive->setIndices(
                    crimild::alloc< IndexBuffer >(
                        Format::INDEX_32_UINT,
                        Array< UInt32 > {
                            0,
                            1,
                            2,
                            0,
                            2,
                            3,
                        }
                    )
                );
                return primitive;
            }()
        );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    auto optimized = geometry->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    EXPECT_EQ( 2, optimized.nodes.size() );
    EXPECT_EQ( 0, optimized.nodes[ 1 ].primitiveIndex );

    EXPECT_EQ( 4, optimized.primitives.triangles.size() );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 1, 2, 3 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 0, 0 },
            }
        ),
        optimized.primitives.triangles[ 0 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 4, 5, 6 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 0, 1 },
            }
        ),
        optimized.primitives.triangles[ 1 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 7, 8, 9 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 1, 1 },
            }
        ),
        optimized.primitives.triangles[ 2 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 10, 11, 12 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 1, 0 },
            }
        ),
        optimized.primitives.triangles[ 3 ]
    );

    EXPECT_EQ( 6, optimized.primitives.indices.size() );
    EXPECT_EQ( 0, optimized.primitives.indices[ 0 ] );
    EXPECT_EQ( 1, optimized.primitives.indices[ 1 ] );
    EXPECT_EQ( 2, optimized.primitives.indices[ 2 ] );
    EXPECT_EQ( 0, optimized.primitives.indices[ 3 ] );
    EXPECT_EQ( 2, optimized.primitives.indices[ 4 ] );
    EXPECT_EQ( 3, optimized.primitives.indices[ 5 ] );

    EXPECT_EQ( 2, optimized.primitives.indexOffsets.size() );
    EXPECT_EQ( 0, optimized.primitives.indexOffsets[ 0 ] );
    EXPECT_EQ( 3, optimized.primitives.indexOffsets[ 1 ] );

    EXPECT_EQ( 1, optimized.primitives.primTree.size() );
    EXPECT_TRUE( optimized.primitives.primTree[ 0 ].isLeaf() );
    EXPECT_EQ( 0, optimized.primitives.primTree[ 0 ].primitiveIndicesOffset );
    EXPECT_EQ( 2, optimized.primitives.primTree[ 0 ].getPrimCount() );
}

TEST( RTPrimAccel, two_independent_geometries )
{
    auto geometry = []() {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive(
            [] {
                auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
                primitive->setVertexData(
                    {
                        crimild::alloc< VertexBuffer >(
                            VertexP3N3TC2::getLayout(),
                            Array< VertexP3N3TC2 > {
                                {
                                    .position = Vector3 { 1, 2, 3 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 0, 0 },
                                },
                                {
                                    .position = Vector3 { 4, 5, 6 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 0, 1 },
                                },
                                {
                                    .position = Vector3 { 7, 8, 9 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 1, 1 },
                                },
                                {
                                    .position = Vector3 { 10, 11, 12 },
                                    .normal = Vector3 { 0, 1, 0 },
                                    .texCoord = Vector2f { 1, 0 },
                                },
                            }
                        ),
                    }
                );
                primitive->setIndices(
                    crimild::alloc< IndexBuffer >(
                        Format::INDEX_32_UINT,
                        Array< UInt32 > {
                            0,
                            1,
                            2,
                            0,
                            2,
                            3,
                        }
                    )
                );
                return primitive;
            }()
        );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    };

    auto group = crimild::alloc< Group >();
    group->attachNode( geometry() );
    group->attachNode( geometry() );

    auto optimized = group->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    EXPECT_EQ( 5, optimized.nodes.size() );
    EXPECT_EQ( 0, optimized.nodes[ 2 ].primitiveIndex );
    EXPECT_EQ( 1, optimized.nodes[ 4 ].primitiveIndex );

    EXPECT_EQ( 8, optimized.primitives.triangles.size() );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 1, 2, 3 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 0, 0 },
            }
        ),
        optimized.primitives.triangles[ 0 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 4, 5, 6 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 0, 1 },
            }
        ),
        optimized.primitives.triangles[ 1 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 7, 8, 9 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 1, 1 },
            }
        ),
        optimized.primitives.triangles[ 2 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 10, 11, 12 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 1, 0 },
            }
        ),
        optimized.primitives.triangles[ 3 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 1, 2, 3 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 0, 0 },
            }
        ),
        optimized.primitives.triangles[ 4 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 4, 5, 6 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 0, 1 },
            }
        ),
        optimized.primitives.triangles[ 5 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 7, 8, 9 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 1, 1 },
            }
        ),
        optimized.primitives.triangles[ 6 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 10, 11, 12 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 1, 0 },
            }
        ),
        optimized.primitives.triangles[ 7 ]
    );

    EXPECT_EQ( 12, optimized.primitives.indices.size() );
    EXPECT_EQ( 0, optimized.primitives.indices[ 0 ] );
    EXPECT_EQ( 1, optimized.primitives.indices[ 1 ] );
    EXPECT_EQ( 2, optimized.primitives.indices[ 2 ] );
    EXPECT_EQ( 0, optimized.primitives.indices[ 3 ] );
    EXPECT_EQ( 2, optimized.primitives.indices[ 4 ] );
    EXPECT_EQ( 3, optimized.primitives.indices[ 5 ] );
    EXPECT_EQ( 4, optimized.primitives.indices[ 6 ] );
    EXPECT_EQ( 5, optimized.primitives.indices[ 7 ] );
    EXPECT_EQ( 6, optimized.primitives.indices[ 8 ] );
    EXPECT_EQ( 4, optimized.primitives.indices[ 9 ] );
    EXPECT_EQ( 6, optimized.primitives.indices[ 10 ] );
    EXPECT_EQ( 7, optimized.primitives.indices[ 11 ] );

    EXPECT_EQ( 4, optimized.primitives.indexOffsets.size() );
    EXPECT_EQ( 0, optimized.primitives.indexOffsets[ 0 ] );
    EXPECT_EQ( 3, optimized.primitives.indexOffsets[ 1 ] );
    EXPECT_EQ( 0, optimized.primitives.indexOffsets[ 2 ] );
    EXPECT_EQ( 3, optimized.primitives.indexOffsets[ 3 ] );

    EXPECT_EQ( 2, optimized.primitives.primTree.size() );
    EXPECT_TRUE( optimized.primitives.primTree[ 0 ].isLeaf() );
    EXPECT_EQ( 0, optimized.primitives.primTree[ 0 ].primitiveIndicesOffset );
    EXPECT_EQ( 2, optimized.primitives.primTree[ 0 ].getPrimCount() );
    EXPECT_TRUE( optimized.primitives.primTree[ 1 ].isLeaf() );
    EXPECT_EQ( 2, optimized.primitives.primTree[ 1 ].primitiveIndicesOffset );
    EXPECT_EQ( 2, optimized.primitives.primTree[ 1 ].getPrimCount() );
}

TEST( RTPrimAccel, two_geometries_with_shared_primitives )
{
    auto primitive = [] {
        auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
        primitive->setVertexData(
            {
                crimild::alloc< VertexBuffer >(
                    VertexP3N3TC2::getLayout(),
                    Array< VertexP3N3TC2 > {
                        {
                            .position = Vector3 { 1, 2, 3 },
                            .normal = Vector3 { 0, 1, 0 },
                            .texCoord = Vector2f { 0, 0 },
                        },
                        {
                            .position = Vector3 { 4, 5, 6 },
                            .normal = Vector3 { 0, 1, 0 },
                            .texCoord = Vector2f { 0, 1 },
                        },
                        {
                            .position = Vector3 { 7, 8, 9 },
                            .normal = Vector3 { 0, 1, 0 },
                            .texCoord = Vector2f { 1, 1 },
                        },
                        {
                            .position = Vector3 { 10, 11, 12 },
                            .normal = Vector3 { 0, 1, 0 },
                            .texCoord = Vector2f { 1, 0 },
                        },
                    }
                ),
            }
        );
        primitive->setIndices(
            crimild::alloc< IndexBuffer >(
                Format::INDEX_32_UINT,
                Array< UInt32 > {
                    0,
                    1,
                    2,
                    0,
                    2,
                    3,
                }
            )
        );
        return primitive;
    }();

    auto geometry = [ primitive ]() {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( primitive );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    };

    auto group = crimild::alloc< Group >();
    group->attachNode( geometry() );
    group->attachNode( geometry() );

    auto optimized = group->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >();

    EXPECT_EQ( 5, optimized.nodes.size() );
    EXPECT_EQ( 0, optimized.nodes[ 2 ].primitiveIndex );
    EXPECT_EQ( 0, optimized.nodes[ 4 ].primitiveIndex );

    EXPECT_EQ( 4, optimized.primitives.triangles.size() );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 1, 2, 3 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 0, 0 },
            }
        ),
        optimized.primitives.triangles[ 0 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 4, 5, 6 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 0, 1 },
            }
        ),
        optimized.primitives.triangles[ 1 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 7, 8, 9 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 1, 1 },
            }
        ),
        optimized.primitives.triangles[ 2 ]
    );
    EXPECT_EQ(
        (
            VertexP3N3TC2 {
                .position = Vector3 { 10, 11, 12 },
                .normal = Vector3 { 0, 1, 0 },
                .texCoord = Vector2f { 1, 0 },
            }
        ),
        optimized.primitives.triangles[ 3 ]
    );

    EXPECT_EQ( 6, optimized.primitives.indices.size() );
    EXPECT_EQ( 0, optimized.primitives.indices[ 0 ] );
    EXPECT_EQ( 1, optimized.primitives.indices[ 1 ] );
    EXPECT_EQ( 2, optimized.primitives.indices[ 2 ] );
    EXPECT_EQ( 0, optimized.primitives.indices[ 3 ] );
    EXPECT_EQ( 2, optimized.primitives.indices[ 4 ] );
    EXPECT_EQ( 3, optimized.primitives.indices[ 5 ] );

    EXPECT_EQ( 2, optimized.primitives.indexOffsets.size() );
    EXPECT_EQ( 0, optimized.primitives.indexOffsets[ 0 ] );
    EXPECT_EQ( 3, optimized.primitives.indexOffsets[ 1 ] );

    // The tree has only one node since there is only one primitive
    EXPECT_EQ( 1, optimized.primitives.primTree.size() );
    EXPECT_TRUE( optimized.primitives.primTree[ 0 ].isLeaf() );
    EXPECT_EQ( 0, optimized.primitives.primTree[ 0 ].primitiveIndicesOffset );
    EXPECT_EQ( 2, optimized.primitives.primTree[ 0 ].getPrimCount() );
}

TEST( RTPrimAccel, a_box_primitive )
{
    auto geometry = []() {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< BoxPrimitive >() );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();

    auto group = crimild::alloc< Group >();
    group->attachNode( geometry );

    auto optimized = group->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE )->perform< RTAcceleration >( false );

    EXPECT_EQ( 2, optimized.nodes.size() );
    EXPECT_EQ( 0, optimized.nodes[ 1 ].primitiveIndex );

    EXPECT_EQ( 24, optimized.primitives.triangles.size() );
    EXPECT_EQ(
        0,
        memcmp(
            optimized.primitives.triangles.getData(),
            geometry->anyPrimitive()->getVertexData().first()->getBufferView()->getData(),
            sizeof( VertexP3N3TC2 ) * 24
        )
    );

    EXPECT_EQ( 36, optimized.primitives.indices.size() );
    EXPECT_EQ(
        0,
        memcmp(
            optimized.primitives.indices.getData(),
            geometry->anyPrimitive()->getIndices()->getBufferView()->getData(),
            sizeof( UInt32 ) * 36
        )
    );

    /*
                                                              x
                               _______________________________|_______________________________
                              |                                                               |
                              y                                                               y
               _______________|_______________                                ________________|______________
              |                               |                              |                               |
              z                               z                              z                               z
       _______|_______                ________|_______                 ______|________                _______|_______
      |               |              |                |               |               |              |               |
      x               x              x                x               x               x              x               x
    /   \           /   \          /    \          /    \            /  \            /  \          /   \           /    \
left    bottom  left    bottom  left    top     left    top     bottom  right   bottom  right   top     right   top     right
bottom  back    bottom  front   top     back    top     front   back    bottom  front   bottom  back    top     front   top
back            front           back            front                   back            front           back            front

            */

    EXPECT_EQ( 31, optimized.primitives.primTree.size() );

    EXPECT_FALSE( optimized.primitives.primTree[ 0 ].isLeaf() );
    EXPECT_EQ( 0, optimized.primitives.primTree[ 0 ].getSplitAxis() );
    EXPECT_EQ( 0, optimized.primitives.primTree[ 0 ].getSplitPos() );
    EXPECT_EQ( 16, optimized.primitives.primTree[ 0 ].getAboveChild() );

    EXPECT_FALSE( optimized.primitives.primTree[ 1 ].isLeaf() );
    EXPECT_EQ( 1, optimized.primitives.primTree[ 1 ].getSplitAxis() );
    EXPECT_EQ( 0, optimized.primitives.primTree[ 1 ].getSplitPos() );
    EXPECT_EQ( 9, optimized.primitives.primTree[ 1 ].getAboveChild() );

    EXPECT_FALSE( optimized.primitives.primTree[ 2 ].isLeaf() );
    EXPECT_EQ( 2, optimized.primitives.primTree[ 2 ].getSplitAxis() );
    EXPECT_EQ( 0, optimized.primitives.primTree[ 2 ].getSplitPos() );
    EXPECT_EQ( 6, optimized.primitives.primTree[ 2 ].getAboveChild() );

    EXPECT_FALSE( optimized.primitives.primTree[ 3 ].isLeaf() );
    EXPECT_EQ( 0, optimized.primitives.primTree[ 3 ].getSplitAxis() );
    EXPECT_EQ( -0.5, optimized.primitives.primTree[ 3 ].getSplitPos() );
    EXPECT_EQ( 5, optimized.primitives.primTree[ 3 ].getAboveChild() );

    EXPECT_TRUE( optimized.primitives.primTree[ 4 ].isLeaf() );
    EXPECT_TRUE( optimized.primitives.primTree[ 5 ].isLeaf() );
}
