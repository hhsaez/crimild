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

#include "Visitors/BinTreeScene.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Primitives/Primitive.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( BinTreeScene, it_discards_an_empty_group )
{
    auto scene = crimild::alloc< Group >();
    scene->setLocal( translation( 1, 2, 3 ) );
    scene->setName( "a scene" );

    BinTreeScene binTreeScene;
    scene->perform( binTreeScene );

    auto res = binTreeScene.getResult();

    EXPECT_EQ( nullptr, res );
}

TEST( BinTreeScene, it_discards_all_empty_groups )
{
    auto scene = [] {
        auto group = crimild::alloc< Group >();
        group->attachNode(
            [] {
                auto group = crimild::alloc< Group >();
                return group;
            }() );
        return group;
    }();

    BinTreeScene binTreeScene;
    scene->perform( binTreeScene );

    auto res = binTreeScene.getResult();

    EXPECT_EQ( nullptr, res );
}

TEST( BinTreeScene, it_discards_all_groups_with_one_child )
{
    auto scene = [] {
        auto group = crimild::alloc< Group >();
        group->attachNode(
            [] {
                auto group = crimild::alloc< Group >();
                group->attachNode(
                    [] {
                        auto group = crimild::alloc< Group >();
                        group->attachNode(
                            [] {
                                auto group = crimild::alloc< Group >();
                                group->attachNode( crimild::alloc< Geometry >() );
                                return group;
                            }() );
                        return group;
                    }() );
                return group;
            }() );
        return group;
    }();

    BinTreeScene binTreeScene;
    scene->perform( binTreeScene );

    auto res = binTreeScene.getResult();

    EXPECT_NE( nullptr, res );
    EXPECT_EQ( Geometry::__CLASS_NAME, res->getClassName() );
}

TEST( BinTreeScene, it_keeps_only_geometries )
{
    auto scene = [] {
        auto group = crimild::alloc< Group >();
        group->attachNode(
            [] {
                auto group = crimild::alloc< Group >();
                group->attachNode(
                    [] {
                        auto group = crimild::alloc< Group >();
                        group->attachNode(
                            [] {
                                auto group = crimild::alloc< Group >();
                                group->attachNode( crimild::alloc< Geometry >() );
                                return group;
                            }() );
                        return group;
                    }() );
                return group;
            }() );
        group->attachNode( crimild::alloc< Geometry >() );
        return group;
    }();

    BinTreeScene binTreeScene;
    scene->perform( binTreeScene );

    auto res = binTreeScene.getResult();

    EXPECT_NE( nullptr, res );
    EXPECT_EQ( std::string( Group::__CLASS_NAME ), std::string( res->getClassName() ) );
    EXPECT_EQ( 2, cast_ptr< Group >( res )->getNodeCount() );
    EXPECT_EQ( Geometry::__CLASS_NAME, cast_ptr< Group >( res )->getNodeAt( 0 )->getClassName() );
    EXPECT_EQ( Geometry::__CLASS_NAME, cast_ptr< Group >( res )->getNodeAt( 1 )->getClassName() );
}

TEST( BinTreeScene, it_handles_a_single_geometry )
{
    auto scene = [] {
        auto geometry = crimild::alloc< Geometry >();
        geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
        geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
        return geometry;
    }();
    scene->setLocal( translation( 1, 2, 3 ) );
    scene->setName( "a scene" );

    BinTreeScene binTreeScene;
    scene->perform( binTreeScene );

    auto res = binTreeScene.getResult();

    EXPECT_NE( nullptr, res );
    EXPECT_EQ( "a scene", res->getName() );
    EXPECT_NE( nullptr, cast_ptr< Geometry >( res )->anyPrimitive() );
    EXPECT_NE( nullptr, res->getComponent< MaterialComponent >() );
    EXPECT_NE( nullptr, res->getComponent< MaterialComponent >()->first() );
    EXPECT_EQ( ( Point3 { 1, 2, 3 } ), location( res->getLocal() ) );
}

TEST( BinTreeScene, it_handles_a_group_with_a_single_child )
{
    auto scene = crimild::alloc< Group >();
    scene->attachNode( crimild::alloc< Geometry >() );

    BinTreeScene binTreeScene;
    scene->perform( binTreeScene );

    auto res = binTreeScene.getResult();

    EXPECT_NE( nullptr, res );
    EXPECT_EQ( Geometry::__CLASS_NAME, res->getClassName() );
}

TEST( BinTreeScene, it_handles_a_group_with_two_children )
{
    auto scene = crimild::alloc< Group >();
    scene->attachNode( crimild::alloc< Geometry >() );
    scene->attachNode( crimild::alloc< Geometry >() );

    BinTreeScene binTreeScene;
    scene->perform( binTreeScene );

    auto res = binTreeScene.getResult();

    EXPECT_NE( nullptr, res );
    EXPECT_EQ( std::string( Group::__CLASS_NAME ), std::string( res->getClassName() ) );
    EXPECT_EQ( 2, cast_ptr< Group >( res )->getNodeCount() );
}

TEST( BinTreeScene, it_handles_a_group_with_three_children )
{
    auto scene = crimild::alloc< Group >();
    scene->attachNode( crimild::alloc< Geometry >() );
    scene->attachNode( crimild::alloc< Geometry >() );
    scene->attachNode( crimild::alloc< Geometry >() );

    BinTreeScene binTreeScene;
    scene->perform( binTreeScene );

    auto res = binTreeScene.getResult();

    EXPECT_NE( nullptr, res );
    EXPECT_EQ( 2, cast_ptr< Group >( res )->getNodeCount() );
    EXPECT_EQ( 2, cast_ptr< Group >( cast_ptr< Group >( res )->getNodeAt( 0 ) )->getNodeCount() );
}

TEST( BinTreeScene, it_handles_a_csg_node_with_one_child )
{
    auto scene = [] {
        auto csg = crimild::alloc< CSGNode >( CSGNode::Operator::UNION );
        csg->setLeft(
            [] {
                auto geometry = crimild::alloc< Geometry >();
                geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
                geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
                return geometry;
            }() );
        return csg;
    }();

    BinTreeScene binTreeScene;
    scene->perform( binTreeScene );

    auto res = binTreeScene.getResult();

    EXPECT_NE( nullptr, res );
    EXPECT_NE( nullptr, cast_ptr< CSGNode >( res )->getLeft() );
}

TEST( BinTreeScene, it_handles_a_csg_node_with_two_children )
{
    auto scene = [] {
        auto csg = crimild::alloc< CSGNode >( CSGNode::Operator::UNION );
        csg->setLeft(
            [] {
                auto geometry = crimild::alloc< Geometry >();
                geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
                geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
                return geometry;
            }() );
        csg->setRight(
            [] {
                auto geometry = crimild::alloc< Geometry >();
                geometry->attachPrimitive( crimild::alloc< Primitive >( Primitive::Type::SPHERE ) );
                geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::PrincipledBSDF >() );
                return geometry;
            }() );
        return csg;
    }();

    BinTreeScene binTreeScene;
    scene->perform( binTreeScene );

    auto res = binTreeScene.getResult();

    EXPECT_NE( nullptr, res );
    EXPECT_NE( nullptr, cast_ptr< CSGNode >( res )->getLeft() );
    EXPECT_NE( nullptr, cast_ptr< CSGNode >( res )->getRight() );
}

TEST( BinTreeScene, it_handles_a_parent_with_many_children )
{
    /**
     * Original scene:
     *       A 
     *   ____|____ 
     *  /| | | | |\
     * B C D E F G H
     */
    auto scene = crimild::alloc< Group >( "A" );
    scene->attachNode( crimild::alloc< Geometry >( "B" ) );
    scene->attachNode( crimild::alloc< Geometry >( "C" ) );
    scene->attachNode( crimild::alloc< Geometry >( "D" ) );
    scene->attachNode( crimild::alloc< Geometry >( "E" ) );
    scene->attachNode( crimild::alloc< Geometry >( "F" ) );
    scene->attachNode( crimild::alloc< Geometry >( "G" ) );
    scene->attachNode( crimild::alloc< Geometry >( "H" ) );
    scene->attachNode( crimild::alloc< Geometry >( "I" ) );

    /**
     * Optimized scene:
     *        A
     *    Z       W 
     *  Y   X   V   U 
     * B C D E F G H I
     */
    auto res = scene->perform< BinTreeScene >( BinTreeScene::SplitStrategy::NONE );

    class AssignNames : public NodeVisitor {
    public:
        using Result = void;

    public:
        void visitNode( Node *node ) noexcept override
        {
            assignName( node );
        }

        void visitGroup( Group *group ) noexcept override
        {
            assignName( group );
            NodeVisitor::visitGroup( group );
        }

    private:
        void assignName( Node *node ) noexcept
        {
            if ( node->getName() == "" ) {
                std::string name;
                name += m_name--;
                node->setName( name );
            }
        }

    private:
        char m_name = 'Z';
    };

    class DescribeScene : public NodeVisitor {
    public:
        using Result = std::string;

    public:
        std::string getResult( void ) const noexcept { return m_result; }

        void visitNode( Node *node ) noexcept override
        {
            m_result += node->getName();
        }

        void visitGroup( Group *group ) noexcept override
        {
            m_result += group->getName();
            NodeVisitor::visitGroup( group );
        }

    private:
        std::string m_result;
    };

    res->perform( AssignNames() );
    EXPECT_EQ( "AZYBCXDEWVFGUHI", res->perform< DescribeScene >() );
}