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

#include "SceneGraph/Node3D.hpp"

#include "Foundation/Memory.hpp"
#include "Mathematics/Transformation_identity.hpp"

#include "gtest/gtest.h"

TEST( Node3D, get_parent_node_3d )
{
    auto root = crimild::alloc< crimild::scenegraph::NodeBase >( "Parent" );
    auto child = crimild::alloc< crimild::scenegraph::NodeBase >( "Child" );

    EXPECT_FALSE( child->hasParent() );
    EXPECT_EQ( nullptr, child->getParent() );

    root->attach( child );

    EXPECT_TRUE( child->hasParent() );
    EXPECT_EQ( root, child->getParent() );
}

TEST( Node3D, get_parent_node_3d_indirect )
{
    auto root = crimild::alloc< crimild::scenegraph::Node3D >( "Parent" );
    auto middle = crimild::alloc< crimild::scenegraph::NodeBase >( "Some node" );
    auto child = crimild::alloc< crimild::scenegraph::Node3D >( "Child" );

    root->attach( middle );
    middle->attach( child );

    EXPECT_TRUE( child->hasParent() );
    EXPECT_EQ( middle, child->getParent() );
    EXPECT_EQ( root, child->getParentNode3D() );
}

TEST( Node3D, default_transforms )
{
    auto node = crimild::alloc< crimild::scenegraph::Node3D >();

    EXPECT_FALSE( node->localNeedsUpdate() );
    EXPECT_TRUE( isIdentity( node->getLocal() ) );
    EXPECT_FALSE( node->worldNeedsUpdate() );
    EXPECT_TRUE( isIdentity( node->getWorld() ) );
}

TEST( Node3D, transforms_local )
{
    auto node = crimild::alloc< crimild::scenegraph::Node3D >();

    EXPECT_FALSE( node->localNeedsUpdate() );
    node->setLocal( crimild::translation( 1, 0, 0 ) );
    EXPECT_FALSE( node->localNeedsUpdate() );
    EXPECT_EQ( ( crimild::Point3 { 1, 0, 0 } ), crimild::location( node->getLocal() ) );

    EXPECT_TRUE( node->worldNeedsUpdate() );
    EXPECT_EQ( ( crimild::Point3 { 1, 0, 0 } ), crimild::location( node->getWorld() ) );
    EXPECT_FALSE( node->worldNeedsUpdate() );
    EXPECT_FALSE( node->localNeedsUpdate() );
}

TEST( Node3D, transforms_world )
{
    auto node = crimild::alloc< crimild::scenegraph::Node3D >();

    EXPECT_FALSE( node->worldNeedsUpdate() );
    node->setWorld( crimild::translation( 1, 0, 0 ) );
    EXPECT_FALSE( node->worldNeedsUpdate() );
    EXPECT_EQ( ( crimild::Point3 { 1, 0, 0 } ), crimild::location( node->getWorld() ) );

    EXPECT_TRUE( node->localNeedsUpdate() );
    EXPECT_EQ( ( crimild::Point3 { 1, 0, 0 } ), crimild::location( node->getLocal() ) );
    EXPECT_FALSE( node->localNeedsUpdate() );
    EXPECT_FALSE( node->worldNeedsUpdate() );
}

TEST( Node3D, transforms_local_const )
{
    const auto node = [] {
        auto ret = crimild::alloc< crimild::scenegraph::Node3D >();
        ret->setLocal( crimild::translation( 1, 0, 0 ) );
        return ret;
    }();

    EXPECT_TRUE( node->worldNeedsUpdate() );
    EXPECT_EQ( ( crimild::Point3 { 1, 0, 0 } ), crimild::location( node->getWorld() ) );
    EXPECT_FALSE( node->worldNeedsUpdate() );
}

TEST( Node3D, transform_child_nodes )
{
    // Parent update local
    // Parent updates world

    // Indirect parent updates local
    // Indirect parent updates world

    // Child updates local
    // Child updates world

    // Sibiling updates local
    // Sibiling updates world

    // Indirect child updates local
    // Indirect child updates world

    auto root = crimild::alloc< crimild::scenegraph::Node3D >( "Root" );
    root->attach( crimild::alloc< crimild::scenegraph::Node3D >( "Child" ) );

    root->setLocal( crimild::translation( 1, 0, 0 ) );

    root->getChildAt< crimild::scenegraph::Node3D >( 0 )->setLocal( crimild::translation( 0, 1, 0 ) );

    EXPECT_EQ(
        ( crimild::Point3 { 1, 1, 0 } ),
        crimild::location( root->getChildAt< crimild::scenegraph::Node3D >( 0 )->getWorld() )
    );
}

TEST( Node3D, transform_child_nodes_indirect )
{
    auto root = crimild::alloc< crimild::scenegraph::Node3D >( "Root" );
    root->attach( crimild::alloc< crimild::scenegraph::NodeBase >( "Middle node" ) );
    root->getChildAt( 0 )->attach( crimild::alloc< crimild::scenegraph::Node3D >( "Child" ) );

    root->setLocal( crimild::translation( 1, 0, 0 ) );

    EXPECT_EQ(
        ( crimild::Point3 { 1, 0, 0 } ),
        crimild::location( root->getChildAt( 0 )->getChildAt< crimild::scenegraph::Node3D >( 0 )->getWorld() )
    );
}
