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

#include "SceneGraph/Group.hpp"
#include "Exceptions/HasParentException.hpp"
#include "Foundation/Stream.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( GroupNodeTest, construction)
{
	auto group = crimild::alloc< Group >( "a group" );

	EXPECT_EQ( group->getName(), "a group" );
	EXPECT_FALSE( group->hasNodes() );
}

TEST( GroupNodeTest, destruction )
{
	auto child1 = crimild::alloc< Group >( "child 1" );
	auto child2 = crimild::alloc< Group >( "child 2" );

	{
		auto parent = crimild::alloc< Group >( "parent" );

		parent->attachNode( child1 );
		EXPECT_TRUE( child1->hasParent() );
		EXPECT_EQ( child1->getParent(), crimild::get_ptr( parent ) );

		parent->attachNode( child2 );
		EXPECT_TRUE( child2->hasParent() );
		EXPECT_EQ( child2->getParent(), crimild::get_ptr( parent ) );
	}

	EXPECT_FALSE( child1->hasParent() );
	EXPECT_EQ( child1->getParent(), nullptr );

	EXPECT_FALSE( child2->hasParent() );
	EXPECT_EQ( child2->getParent(), nullptr );
}

TEST( GroupNodeTest, attachNode )
{
	auto parent = crimild::alloc< Group >( "parent" );

	EXPECT_FALSE( parent->hasNodes() );

	auto child = crimild::alloc< Group >( "child" );
	EXPECT_FALSE( child->hasParent() );
	EXPECT_EQ( child->getParent(), nullptr );

	parent->attachNode( child );

	EXPECT_TRUE( child->hasParent() );
	ASSERT_EQ( child->getParent(), crimild::get_ptr( parent ) );
	EXPECT_TRUE( parent->hasNodes() );

	bool found = false;
	int count = 0;
	parent->forEachNode( [&count, child, &found]( Node *node ) {
		++count;
		if ( crimild::get_ptr( child ) == node ) {
			found = true;
		}
	});

	EXPECT_EQ( count, 1 );
	EXPECT_TRUE( found );
}

TEST( GroupNodeTest, attachMultipleNodes )
{
	auto parent = crimild::alloc< Group >( "parent" );
	
	auto child1 = crimild::alloc< Group >( "child1" );
	auto child2 = crimild::alloc< Group >( "child2" );
	auto child3 = crimild::alloc< Group >( "child3" );

	parent->attachNode( child1 );
	parent->attachNode( child2 );
	parent->attachNode( child3 );

	EXPECT_TRUE( parent->hasNodes() );

	int count = 0;
	parent->forEachNode( [&count]( Node *node ) {
		++count;
	});
	EXPECT_EQ( count, 3 );
}

TEST( GroupNodeTest, reattachNodeToSameParent )
{
	auto parent = crimild::alloc< Group >( "parent" );
	auto child = crimild::alloc< Group >( "child" );

	parent->attachNode( child );

	EXPECT_NO_THROW( parent->attachNode( child ) );

	bool found = false;
	int count = 0;
	parent->forEachNode( [&count, child, &found]( Node *node ) {
		++count;
		if ( crimild::get_ptr( child ) == node ) {
			found = true;
		}
	});

	EXPECT_EQ( count, 1 );
	EXPECT_TRUE( found );
}

TEST( GroupNodeTest, reattachNodeToDifferentParent )
{
	auto parent1 = crimild::alloc< Group >( "parent1" );
	auto parent2 = crimild::alloc< Group >( "parent2" );
	auto child = crimild::alloc< Group >( "child" );

	parent1->attachNode( child );

	ASSERT_THROW( parent2->attachNode( child ), HasParentException );
}

TEST( GroupNodeTest, detachNode )
{
	auto parent = crimild::alloc< Group >( "parent" );
	auto child = crimild::alloc< Group >( "child" );

	parent->attachNode( child );
	parent->detachNode( child );

	EXPECT_FALSE( parent->hasNodes() );
	EXPECT_FALSE( child->hasParent() );
	EXPECT_EQ( child->getParent(), nullptr );
}

TEST( GroupNodeTest, detachMultipleNodes )
{
	auto parent = crimild::alloc< Group >( "parent" );
	
	auto child1 = crimild::alloc< Group >( "child1" );
	auto child2 = crimild::alloc< Group >( "child2" );
	auto child3 = crimild::alloc< Group >( "child3" );

	parent->attachNode( child1 );
	parent->attachNode( child2 );
	parent->attachNode( child3 );

	parent->detachNode( child1 );
	parent->detachNode( child2 );

	EXPECT_TRUE( parent->hasNodes() );

	int count = 0;
	parent->forEachNode( [&count, child3]( Node *node ) {
		EXPECT_EQ( node->getName(), child3->getName() );
		++count;
	});
	EXPECT_EQ( count, 1 );
}

TEST( GroupNodeTest, detachNodeFromDifferentParent )
{
	auto parent1 = crimild::alloc< Group >( "parent1" );
	auto parent2 = crimild::alloc< Group >( "parent2" );
	auto child = crimild::alloc< Group >( "child" );

	parent1->attachNode( child );
	parent2->detachNode( child );

	EXPECT_TRUE( child->hasParent() );
	ASSERT_EQ( child->getParent(), crimild::get_ptr( parent1 ) );
	EXPECT_TRUE( parent1->hasNodes() );
	EXPECT_FALSE( parent2->hasNodes() );
}

TEST( GroupNodeTest, detachAllNodes )
{
	auto parent = crimild::alloc< Group >( "parent" );
	
	auto child1 = crimild::alloc< Group >( "child1" );
	auto child2 = crimild::alloc< Group >( "child2" );
	auto child3 = crimild::alloc< Group >( "child3" );

	parent->attachNode( child1 );
	parent->attachNode( child2 );
	parent->attachNode( child3 );

	parent->detachAllNodes();

	EXPECT_FALSE( parent->hasNodes() );

	EXPECT_FALSE( child1->hasParent() );
	EXPECT_EQ( child1->getParent(), nullptr );
	EXPECT_FALSE( child2->hasParent() );
	EXPECT_EQ( child2->getParent(), nullptr );
	EXPECT_FALSE( child3->hasParent() );
	EXPECT_EQ( child3->getParent(), nullptr );

	int count = 0;
	parent->forEachNode( [&count]( Node *node ) {
		++count;
	});

	EXPECT_EQ( count, 0 );
}

TEST( GroupNodeTest, buildHierarchy )
{
	//		node0
	//		/   \
	//	node1	node2
	//			/	\
	//		node3	node4

	auto node0 = crimild::alloc< Group >( "node0" );
	auto node1 = crimild::alloc< Group >( "node1" );
	auto node2 = crimild::alloc< Group >( "node2" );
	auto node3 = crimild::alloc< Group >( "node3" );
	auto node4 = crimild::alloc< Group >( "node4" );

	node0->attachNode( node1 );
	node0->attachNode( node2 );
	node2->attachNode( node3 );
	node2->attachNode( node4 );

	EXPECT_TRUE( node0->hasNodes() );
	EXPECT_EQ( node1->getParent(), crimild::get_ptr( node0 ) );
	EXPECT_EQ( node2->getParent(), crimild::get_ptr( node0 ) );

	EXPECT_TRUE( node2->hasNodes() );
	EXPECT_EQ( node3->getParent(), crimild::get_ptr( node2 ) );
	EXPECT_EQ( node4->getParent(), crimild::get_ptr( node2 ) );
}

TEST( GroupNodeTest, streaming )
{
	//		node0
	//		/   \
	//	node1	node2
	//			/	\
	//		node3	node4

	auto node0 = crimild::alloc< Group >( "node0" );
		auto node1 = crimild::alloc< Group >( "node1" );
		node0->attachNode( node1 );
		auto node2 = crimild::alloc< Group >( "node2" );
		node0->attachNode( node2 );
			auto node3 = crimild::alloc< Group >( "node3" );
			node2->attachNode( node3 );
			auto node4 = crimild::alloc< Group >( "node4" );
			node2->attachNode( node4 );

	{
		FileStream os( "group.crimild", FileStream::OpenMode::WRITE );
		os.addObject( node0 );
		os.flush();
	}

	{
		FileStream is( "group.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );

		EXPECT_EQ( 1, is.getObjectCount() );
		auto n0 = is.getObjectAt< Group >( 0 );
		EXPECT_TRUE( n0 != nullptr );
		EXPECT_EQ( "node0", n0->getName() );
		EXPECT_EQ( 2, n0->getNodeCount() );

		auto n1 = n0->getNodeAt< Group >( 0 );
		EXPECT_TRUE( n1 != nullptr );
		EXPECT_EQ( "node1", n1->getName() );
		EXPECT_EQ( 0, n1->getNodeCount() );
		
		auto n2 = n0->getNodeAt< Group >( 1 );
		EXPECT_TRUE( n2 != nullptr );
		EXPECT_EQ( "node2", n2->getName() );
		EXPECT_EQ( 2, n2->getNodeCount() );

		auto n3 = n2->getNodeAt< Group >( 0 );
		EXPECT_TRUE( n3 != nullptr );
		EXPECT_EQ( "node3", n3->getName() );
		EXPECT_EQ( 0, n3->getNodeCount() );
		
		auto n4 = n2->getNodeAt< Group >( 1 );
		EXPECT_TRUE( n4 != nullptr );
		EXPECT_EQ( "node4", n4->getName() );
		EXPECT_EQ( 0, n4->getNodeCount() );
	}

}

