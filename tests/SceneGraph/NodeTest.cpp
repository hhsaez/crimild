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

#include "SceneGraph/Node.hpp"
#include "SceneGraph/Group.hpp"

#include "Utils/MockComponent.hpp"

#include "gtest/gtest.h"

using namespace Crimild;

TEST( NodeTest, construction )
{
	NodePtr node( new Node( "a Node" ) );

	EXPECT_EQ( node->getName(), "a Node" );
	EXPECT_FALSE( node->hasParent() );
	EXPECT_EQ( node->getParent(), nullptr );
}

TEST( NodeTest, destruction )
{
	MockComponentPtr cmp1( new MockComponent( "cmp1" ) );
	EXPECT_CALL( *cmp1, onAttach() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *cmp1, onDetach() )
		.Times( ::testing::Exactly( 1 ) );

	MockComponentPtr cmp2( new MockComponent( "cmp2" ) );
	EXPECT_CALL( *cmp2, onAttach() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *cmp2, onDetach() )
		.Times( ::testing::Exactly( 1 ) );

	{
		NodePtr node( new Node() );
		node->attachComponent( cmp1 );
		node->attachComponent( cmp2 );

		EXPECT_EQ( node->getComponentWithName( "cmp1" ), cmp1.get() );
		EXPECT_EQ( node->getComponentWithName( "cmp1" ), cmp1.get() );
	}

	EXPECT_EQ( cmp1->getNode(), nullptr );
	EXPECT_EQ( cmp2->getNode(), nullptr );
}

TEST( NodeTest, setParent )
{
	NodePtr parent( new Node( "the parent" ) );
	NodePtr child( new Node( "the child" ) );

	EXPECT_FALSE( parent->hasParent() );
	EXPECT_EQ( parent->getParent(), nullptr );

	EXPECT_FALSE( child->hasParent() );
	EXPECT_EQ( child->getParent(), nullptr );

	child->setParent( parent.get() );

	EXPECT_TRUE( child->hasParent() );
	ASSERT_NE( child->getParent(), nullptr );
	EXPECT_EQ( child->getParent(), parent.get() );
	EXPECT_EQ( child->getParent()->getName(), parent->getName() );
}

TEST( NodeTest, attachComponent )
{
	NodePtr node( new Node() );

	MockComponentPtr cmp( new MockComponent() );
	EXPECT_CALL( *cmp, onAttach() )
		.Times( ::testing::Exactly( 1 ) );

	node->attachComponent( cmp );

	ASSERT_EQ( cmp->getNode(), node.get() );
	ASSERT_EQ( node->getComponent< MockComponent >(), cmp.get() );
}

TEST( NodeTest, attachComponentTwice )
{
	NodePtr node( new Node() );

	MockComponentPtr cmp( new MockComponent() );
	EXPECT_CALL( *cmp, onAttach() )
		.Times( ::testing::Exactly( 1 ) );

	node->attachComponent( cmp );
	node->attachComponent( cmp );
}

TEST( NodeTest, getComponentWithName )
{
	NodePtr node( new Node() );

	MockComponentPtr cmp( new MockComponent() );
	EXPECT_CALL( *cmp, onAttach() )
		.Times( ::testing::Exactly( 1 ) );

	node->attachComponent( cmp );

	ASSERT_EQ( cmp->getNode(), node.get() );
	ASSERT_EQ( node->getComponentWithName( cmp->getName() ), cmp.get() );
}

TEST( NodeTest, getInvalidComponent )
{
	NodePtr node( new Node() );

	ASSERT_EQ( node->getComponent< MockComponent >(), nullptr );
}

TEST( NodeTest, attachNewComponentWithSameName )
{
	NodePtr node( new Node() );
	
	MockComponentPtr cmp1( new MockComponent() );
	EXPECT_CALL( *cmp1, onAttach() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *cmp1, onDetach() )
		.Times( ::testing::Exactly( 1 ) );

	MockComponentPtr cmp2( new MockComponent() );
	EXPECT_CALL( *cmp2, onAttach() )
		.Times( ::testing::Exactly( 1 ) );

	node->attachComponent( cmp1 );

	ASSERT_EQ( cmp1->getNode(), node.get() );
	ASSERT_EQ( cmp2->getNode(), nullptr );
	ASSERT_EQ( node->getComponent< MockComponent >(), cmp1.get() );

	node->attachComponent( cmp2 );

	ASSERT_EQ( cmp1->getNode(), nullptr );
	ASSERT_EQ( cmp2->getNode(), node.get() );
	ASSERT_EQ( node->getComponent< MockComponent >(), cmp2.get() );
}

TEST( NodeTest, detachComponentWithName )
{
	NodePtr node( new Node() );

	MockComponentPtr cmp( new MockComponent() );
	EXPECT_CALL( *cmp, onAttach() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *cmp, onDetach() )
		.Times( ::testing::Exactly( 1 ) );

	node->attachComponent( cmp );
	node->detachComponentWithName( cmp->getName() );

	ASSERT_EQ( cmp->getNode(), nullptr );
	ASSERT_EQ( node->getComponent< MockComponent >(), nullptr );
}

TEST( NodeTest, detachComponent )
{
	NodePtr node( new Node() );

	MockComponentPtr cmp( new MockComponent() );
	EXPECT_CALL( *cmp, onAttach() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *cmp, onDetach() )
		.Times( ::testing::Exactly( 1 ) );

	node->attachComponent( cmp );
	node->detachComponent( cmp );

	ASSERT_EQ( cmp->getNode(), nullptr );
	ASSERT_EQ( node->getComponent< MockComponent >(), nullptr );
}

TEST( NodeTest, detachInvalidComponent )
{
	NodePtr node( new Node() );

	MockComponentPtr cmp1( new MockComponent() );
	EXPECT_CALL( *cmp1, onAttach() )
		.Times( ::testing::Exactly( 1 ) );

	MockComponentPtr cmp2( new MockComponent() );
	EXPECT_CALL( *cmp2, onAttach() )
		.Times( 0 );

	node->attachComponent( cmp1 );
	node->detachComponent( cmp2 );

	ASSERT_EQ( cmp1->getNode(), node.get() );
	ASSERT_EQ( node->getComponent< MockComponent >(), cmp1.get() );

	ASSERT_EQ( cmp2->getNode(), nullptr );
}

TEST( NodeTest, invokeOnAttach )
{
	NodePtr node( new Node() );

	MockComponentPtr cmp( new MockComponent() );
	EXPECT_CALL( *cmp, onAttach() )
		.Times( ::testing::Exactly( 1 ) );

	node->attachComponent( cmp );
}

TEST( NodeTest, invokeOnDetach )
{
	NodePtr node( new Node() );
	
	MockComponentPtr cmp( new MockComponent() );
	EXPECT_CALL( *cmp, onAttach() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *cmp, onDetach() )
		.Times( ::testing::Exactly( 1 ) );

	node->attachComponent( cmp );
	node->detachComponent( cmp );
}

TEST( NodeTest, detachAllComponents )
{
	NodePtr node( new Node() );

	MockComponentPtr cmp1( new MockComponent( "cmp1" ) );
	EXPECT_CALL( *cmp1, onAttach() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *cmp1, onDetach() )
		.Times( ::testing::Exactly( 1 ) );
	
	MockComponentPtr cmp2( new MockComponent( "cmp2" ) );
	EXPECT_CALL( *cmp2, onAttach() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *cmp2, onDetach() )
		.Times( ::testing::Exactly( 1 ) );

	node->attachComponent( cmp1 );
	EXPECT_EQ( node->getComponentWithName( "cmp1" ), cmp1.get() );
	EXPECT_EQ( cmp1->getNode(), node.get() );

	node->attachComponent( cmp2 );
	EXPECT_EQ( node->getComponentWithName( "cmp1" ), cmp1.get() );
	EXPECT_EQ( cmp1->getNode(), node.get() );

	node->detachAllComponents();

	EXPECT_EQ( node->getComponentWithName( "cmp1" ), nullptr );
	EXPECT_EQ( cmp1->getNode(), nullptr );

	EXPECT_EQ( node->getComponentWithName( "cmp2" ), nullptr );
	EXPECT_EQ( cmp2->getNode(), nullptr );
}

TEST( NodeTest, updateComponents )
{
	NodePtr node( new Node() );

	Time t;
	
	MockComponentPtr cmp1( new MockComponent( "cmp1" ) );
	EXPECT_CALL( *cmp1, update( testing::_ ) )
		.Times( ::testing::Exactly( 3 ) );
	
	MockComponentPtr cmp2( new MockComponent( "cmp2" ) );
	EXPECT_CALL( *cmp2, update( testing::_ ) )
		.Times( ::testing::Exactly( 2 ) );

	node->attachComponent( cmp1 );
	node->attachComponent( cmp2 );

	node->updateComponents( t );
	node->updateComponents( t );

	node->detachComponent( cmp2 );

	node->updateComponents( t );
}

TEST( NodeTest, getRootParent )
{
	GroupPtr g1( new Group() );
	GroupPtr g2( new Group() );
	GroupPtr g3( new Group() );

	g1->attachNode( g2 );
	g2->attachNode( g3 );

	EXPECT_EQ( g1.get(), g3->getRootParent() );
}

