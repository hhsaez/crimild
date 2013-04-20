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

#include "Utils/MockVisitor.hpp"

#include "gtest/gtest.h"

using namespace Crimild;

TEST( NodeVisitorTest, traversal )
{
	//		node0
	//		/   \
	//	node1	node2
	//			/	\
	//		node3	node4

	GroupNodePtr node0( new GroupNode( "node0" ) );
	GroupNodePtr node2( new GroupNode( "node2" ) );
	NodePtr node1( new Node( "node1" ) );
	NodePtr node3( new Node( "node3" ) );
	NodePtr node4( new Node( "node4" ) );

	node0->attachNode( node1 );
	node0->attachNode( node2 );
	node2->attachNode( node3 );
	node2->attachNode( node4 );

	MockVisitor visitor;

	EXPECT_CALL( visitor, reset() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( visitor, visitNode( ::testing::_ ) )
		.Times( ::testing::Exactly( 3 ) );
	EXPECT_CALL( visitor, visitGroupNode( ::testing::_ ) )
		.Times( ::testing::Exactly( 2 ) )
		.WillRepeatedly( ::testing::Invoke( &visitor, &MockVisitor::NodeVisitor_visitGroupNode ) );
	node0->perform( visitor );

	EXPECT_CALL( visitor, reset() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( visitor, visitNode( ::testing::_ ) )
		.Times( ::testing::Exactly( 2 ) );
	EXPECT_CALL( visitor, visitGroupNode( ::testing::_ ) )
		.Times( ::testing::Exactly( 1 ) )
		.WillRepeatedly( ::testing::Invoke( &visitor, &MockVisitor::NodeVisitor_visitGroupNode ) );
	node2->perform( visitor );
}

class PrependParentNameVisitor : public NodeVisitor {
public:
	virtual void visitNode( Node *node ) override
	{
		if ( node->hasParent() ) {
			node->setName( node->getParent()->getName() + "_" + node->getName() );
		}
	}

	virtual void visitGroupNode( GroupNode *group ) override
	{
		visitNode( group );
		NodeVisitor::visitGroupNode( group );
	}
};

TEST( NodeVisitorTest, prependParentName )
{
	//		node0
	//		/   \
	//	node1	node2
	//			/	\
	//		node3	node4

	GroupNodePtr node0( new GroupNode( "node0" ) );
	GroupNodePtr node2( new GroupNode( "node2" ) );
	NodePtr node1( new Node( "node1" ) );
	NodePtr node3( new Node( "node3" ) );
	NodePtr node4( new Node( "node4" ) );

	node0->attachNode( node1 );
	node0->attachNode( node2 );
	node2->attachNode( node3 );
	node2->attachNode( node4 );

	node0->perform( PrependParentNameVisitor() );

	EXPECT_EQ( node0->getName(), "node0" );
	EXPECT_EQ( node1->getName(), "node0_node1" );
	EXPECT_EQ( node2->getName(), "node0_node2" );
	EXPECT_EQ( node3->getName(), "node0_node2_node3" );
	EXPECT_EQ( node4->getName(), "node0_node2_node4" );
}


