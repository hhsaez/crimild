/**
 * Crimild Engine is an open source scene graph based engine which purpose
 * is to fulfill the high-performance requirements of typical multi-platform
 * two and tridimensional multimedia projects, like games, simulations and
 * virtual reality.
 *
 * Copyright (C) 2006-2013 Hernan Saez - hhsaez@gmail.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <Crimild.hpp>

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
	
	MockComponentPtr cmp1( new MockComponent( "cmp1" ) );
	EXPECT_CALL( *cmp1, update() )
		.Times( ::testing::Exactly( 3 ) );
	
	MockComponentPtr cmp2( new MockComponent( "cmp2" ) );
	EXPECT_CALL( *cmp2, update() )
		.Times( ::testing::Exactly( 2 ) );

	node->attachComponent( cmp1 );
	node->attachComponent( cmp2 );

	node->updateComponents();
	node->updateComponents();

	node->detachComponent( cmp2 );

	node->updateComponents();
}

