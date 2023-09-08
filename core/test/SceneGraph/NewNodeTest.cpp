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

#include "SceneGraph/NodeBase.hpp"

#include "gtest/gtest.h"

TEST( Node, construction )
{
    auto node = crimild::alloc< crimild::scenegraph::NodeBase >( "a Node" );

    EXPECT_EQ( node->getName(), "a Node" );
    EXPECT_FALSE( node->hasParent() );
    EXPECT_EQ( nullptr, node->getParent() );
}

TEST( Node, get_child )
{
    auto root = crimild::alloc< crimild::scenegraph::NodeBase >( "Root" );
    root->attach( crimild::alloc< crimild::scenegraph::NodeBase >( "Child" ) );

    EXPECT_EQ( "Child", root->getChildAt( 0 )->getName() );
}

TEST( Node, get_child_with_node_type )
{
    auto root = crimild::alloc< crimild::scenegraph::NodeBase >( "Root" );
    root->attach( crimild::alloc< crimild::scenegraph::NodeBase >( "Child" ) );

    EXPECT_EQ( "Child", root->getChildAt< crimild::scenegraph::NodeBase >( 0 )->getName() );
}

TEST( Node, get_parent )
{
    auto root = crimild::alloc< crimild::scenegraph::NodeBase >( "Parent" );
    auto child = crimild::alloc< crimild::scenegraph::NodeBase >( "Child" );

    EXPECT_FALSE( child->hasParent() );
    EXPECT_EQ( nullptr, child->getParent() );

    root->attach( child );

    EXPECT_TRUE( child->hasParent() );
    EXPECT_EQ( root, child->getParent() );
}

TEST( Node, invalidate_parent )
{
    auto child = crimild::alloc< crimild::scenegraph::NodeBase >();

    {
        auto parent = crimild::alloc< crimild::scenegraph::NodeBase >();
        parent->attach( child );
        EXPECT_TRUE( child->hasParent() );
    }

    EXPECT_FALSE( child->hasParent() );
}

TEST( Node, invalidate_children )
{
    std::weak_ptr< crimild::scenegraph::NodeBase > child;

    {
        auto parent = crimild::alloc< crimild::scenegraph::NodeBase >();
        parent->attach( crimild::alloc< crimild::scenegraph::NodeBase >() );
        child = parent->getChildAt( 0 );
        EXPECT_FALSE( child.expired() );
    }

    EXPECT_TRUE( child.expired() );
}

// TEST( NodeTest, destruction )
//{
//      auto cmp1 = crimild::alloc< MockComponent >();
//      EXPECT_CALL( *( cmp1 ), onAttach() )
//          .Times( ::testing::Exactly( 1 ) );
//      EXPECT_CALL( *( cmp1 ), onDetach() )
//          .Times( ::testing::Exactly( 1 ) );

//     {
//         auto node = crimild::alloc< Node >();
//         node->attachComponent( cmp1 );

//         EXPECT_EQ( node->getComponent< MockComponent >(), crimild::get_ptr( cmp1 ) );
//     }

//     EXPECT_EQ( cmp1->getNode(), nullptr );
// }

// TEST( NodeTest, setParent )
// {
//     auto parent = crimild::alloc< Node >( "the parent" );
//     auto child = crimild::alloc< Node >( "the child" );

//     EXPECT_FALSE( parent->hasParent() );
//     EXPECT_EQ( parent->getParent(), nullptr );

//     EXPECT_FALSE( child->hasParent() );
//     EXPECT_EQ( child->getParent(), nullptr );

//     child->setParent( crimild::get_ptr( parent ) );

//     EXPECT_TRUE( child->hasParent() );
//     ASSERT_NE( child->getParent(), nullptr );
//     EXPECT_EQ( child->getParent(), crimild::get_ptr( parent ) );
//     EXPECT_EQ( child->getParent()->getName(), parent->getName() );
// }

// TEST( NodeTest, attachComponent )
// {
//     auto node = crimild::alloc< Node >();

//     auto cmp = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );

//     node->attachComponent( cmp );

//     ASSERT_EQ( cmp->getNode(), node.get() );
//     ASSERT_EQ( node->getComponent< MockComponent >(), crimild::get_ptr( cmp ) );
// }

// TEST( NodeTest, attachComponentTwice )
// {
//     auto node = crimild::alloc< Node >();

//     auto cmp = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );

//     node->attachComponent( cmp );
//     node->attachComponent( cmp );
// }

// TEST( NodeTest, getComponentWithName )
// {
//     auto node = crimild::alloc< Node >();

//     auto cmp = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );

//     node->attachComponent( cmp );

//     ASSERT_EQ( cmp->getNode(), node.get() );
//     ASSERT_EQ( node->getComponentWithName( cmp->getComponentName() ), crimild::get_ptr( cmp ) );
// }

// TEST( NodeTest, getInvalidComponent )
// {
//     auto node = crimild::alloc< Node >();

//     ASSERT_EQ( node->getComponent< MockComponent >(), nullptr );
// }

// TEST( NodeTest, attachNewComponentWithSameName )
// {
//     auto node = crimild::alloc< Node >();

//     auto cmp1 = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp1.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );
//     EXPECT_CALL( *( cmp1.get() ), onDetach() )
//         .Times( ::testing::Exactly( 1 ) );

//     auto cmp2 = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp2.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );

//     node->attachComponent( cmp1 );

//     ASSERT_EQ( cmp1->getNode(), node.get() );
//     ASSERT_EQ( cmp2->getNode(), nullptr );
//     ASSERT_EQ( node->getComponent< MockComponent >(), crimild::get_ptr( cmp1 ) );

//     node->attachComponent( cmp2 );

//     ASSERT_EQ( cmp1->getNode(), nullptr );
//     ASSERT_EQ( cmp2->getNode(), node.get() );
//     ASSERT_EQ( node->getComponent< MockComponent >(), crimild::get_ptr( cmp2 ) );
// }

// TEST( NodeTest, detachComponentWithName )
// {
//     auto node = crimild::alloc< Node >();

//     auto cmp = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );
//     EXPECT_CALL( *( cmp.get() ), onDetach() )
//         .Times( ::testing::Exactly( 1 ) );

//     node->attachComponent( cmp );
//     node->detachComponentWithName( cmp->getComponentName() );

//     ASSERT_EQ( cmp->getNode(), nullptr );
//     ASSERT_EQ( node->getComponent< MockComponent >(), nullptr );
// }

// TEST( NodeTest, detachComponent )
// {
//     auto node = crimild::alloc< Node >();

//     auto cmp = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );
//     EXPECT_CALL( *( cmp.get() ), onDetach() )
//         .Times( ::testing::Exactly( 1 ) );

//     node->attachComponent( cmp );
//     node->detachComponent( cmp );

//     ASSERT_EQ( cmp->getNode(), nullptr );
//     ASSERT_EQ( node->getComponent< MockComponent >(), nullptr );
// }

// TEST( NodeTest, detachInvalidComponent )
// {
//     auto node = crimild::alloc< Node >();

//     auto cmp1 = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp1.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );

//     auto cmp2 = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp2.get() ), onAttach() )
//         .Times( 0 );

//     node->attachComponent( cmp1 );
//     node->detachComponent( cmp2 );

//     ASSERT_EQ( cmp1->getNode(), node.get() );
//     ASSERT_EQ( node->getComponent< MockComponent >(), crimild::get_ptr( cmp1 ) );

//     ASSERT_EQ( cmp2->getNode(), nullptr );
// }

// TEST( NodeTest, invokeOnAttach )
// {
//     auto node = crimild::alloc< Node >();

//     auto cmp = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );

//     node->attachComponent( cmp );
// }

// TEST( NodeTest, invokeOnDetach )
// {
//     auto node = crimild::alloc< Node >();

//     auto cmp = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );
//     EXPECT_CALL( *( cmp.get() ), onDetach() )
//         .Times( ::testing::Exactly( 1 ) );

//     node->attachComponent( cmp );
//     node->detachComponent( cmp );
// }

// TEST( NodeTest, detachAllComponents )
// {
//     auto node = crimild::alloc< Node >();

//     auto cmp1 = crimild::alloc< MockComponent >();
//     EXPECT_CALL( *( cmp1.get() ), onAttach() )
//         .Times( ::testing::Exactly( 1 ) );
//     EXPECT_CALL( *( cmp1.get() ), onDetach() )
//         .Times( ::testing::Exactly( 1 ) );

//     node->attachComponent( cmp1 );
//     EXPECT_EQ( node->getComponent< MockComponent >(), crimild::get_ptr( cmp1 ) );
//     EXPECT_EQ( cmp1->getNode(), node.get() );

//     node->detachAllComponents();

//     EXPECT_EQ( node->getComponent< MockComponent >(), nullptr );
//     EXPECT_EQ( cmp1->getNode(), nullptr );
// }

// TEST( NodeTest, getRootParent )
// {
//     auto g1 = crimild::alloc< Group >();
//     auto g2 = crimild::alloc< Group >();
//     auto g3 = crimild::alloc< Group >();

//     g1->attachNode( g2 );
//     g2->attachNode( g3 );

//     EXPECT_EQ( crimild::get_ptr( g1 ), g3->getRootParent() );
// }

// TEST( NodeTest, coding )
// {
//     auto n1 = crimild::alloc< Node >( "Some node" );
//     n1->setWorldIsCurrent( true );

//     auto encoder = crimild::alloc< coding::MemoryEncoder >();
//     encoder->encode( n1 );
//     auto bytes = encoder->getBytes();
//     auto decoder = crimild::alloc< coding::MemoryDecoder >();
//     decoder->fromBytes( bytes );

//     auto n2 = decoder->getObjectAt< Node >( 0 );
//     EXPECT_TRUE( n2 != nullptr );
//     EXPECT_EQ( n1->getName(), n2->getName() );
//     EXPECT_EQ( n1->worldIsCurrent(), n2->worldIsCurrent() );
// }

// TEST( NodeTest, codingTransformation )
// {
//     auto n1 = crimild::alloc< Node >( "Some Node" );
//     n1->setLocal( translation( 0.0f, 0.0f, -5.0f ) );
//     n1->setWorld( translation( 1.0f, 0.0f, -5.0f ) );

//     auto encoder = crimild::alloc< coding::MemoryEncoder >();
//     encoder->encode( n1 );
//     auto bytes = encoder->getBytes();
//     auto decoder = crimild::alloc< coding::MemoryDecoder >();
//     decoder->fromBytes( bytes );

//     auto n2 = decoder->getObjectAt< Node >( 0 );
//     EXPECT_TRUE( n2 != nullptr );
//     EXPECT_EQ( n1->getName(), n2->getName() );
//     EXPECT_EQ( n1->getLocal(), n2->getLocal() );
//     EXPECT_EQ( n1->getWorld(), n2->getWorld() );
// }

// TEST( NodeTest, codingComponents )
// {
//     auto n1 = crimild::alloc< Node >( "Some Node" );
//     n1->attachComponent< RotationComponent >( Vector3f { 1.0, 2.0, 3.0 }, 3.14159f );

//     auto encoder = crimild::alloc< coding::MemoryEncoder >();
//     encoder->encode( n1 );
//     auto bytes = encoder->getBytes();
//     auto decoder = crimild::alloc< coding::MemoryDecoder >();
//     decoder->fromBytes( bytes );

//     auto n2 = decoder->getObjectAt< Node >( 0 );
//     EXPECT_TRUE( n2 != nullptr );
//     auto cmp = n2->getComponent< RotationComponent >();
//     EXPECT_TRUE( cmp != nullptr );
// }
