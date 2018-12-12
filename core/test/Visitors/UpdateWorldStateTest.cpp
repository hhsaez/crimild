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

#include "Visitors/UpdateWorldState.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Geometry.hpp"

#include "Utils/MockVisitor.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( UpdateWorldStateTest, singleNode )
{
	auto node = crimild::alloc< Node >();

	EXPECT_TRUE( node->getLocal().isIdentity() );
	EXPECT_TRUE( node->getWorld().isIdentity() );

	node->local().setTranslate( 0, 0, -5 );

	EXPECT_FALSE( node->getLocal().isIdentity() );
	EXPECT_EQ( Vector3f( 0, 0, -5 ), node->getLocal().getTranslate() );
	EXPECT_TRUE( node->getWorld().isIdentity() );

	node->perform( UpdateWorldState() );

	EXPECT_FALSE( node->getLocal().isIdentity() );
	EXPECT_EQ( Vector3f( 0, 0, -5 ), node->getLocal().getTranslate() );
	EXPECT_FALSE( node->getWorld().isIdentity() );
	EXPECT_EQ( Vector3f( 0, 0, -5 ), node->getWorld().getTranslate() );
}

TEST( UpdateWorldStateTest, hierarchy )
{
	
	auto group1 = crimild::alloc< Group >();
	auto group2 = crimild::alloc< Group >();
	auto geometry1 = crimild::alloc< Geometry >();
	auto geometry2 = crimild::alloc< Geometry >();
	auto geometry3 = crimild::alloc< Geometry >();

	group1->attachNode( group2.get() );
	group1->attachNode( geometry1.get() );

	group2->attachNode( geometry2.get() );
	group2->attachNode( geometry3.get() );

	EXPECT_TRUE( geometry3->getLocal().isIdentity() );
	EXPECT_TRUE( geometry3->getWorld().isIdentity() );

	group1->local().setTranslate( 0, 0, -5 );
	group1->perform( UpdateWorldState() );

	EXPECT_FALSE( group1->getLocal().isIdentity() );
	EXPECT_FALSE( group1->getWorld().isIdentity() );
	EXPECT_EQ( Vector3f( 0, 0, -5 ), group1->getWorld().getTranslate() );

	EXPECT_TRUE( group2->getLocal().isIdentity() );
	EXPECT_FALSE( group2->getWorld().isIdentity() );
	EXPECT_EQ( Vector3f( 0, 0, -5 ), group2->getWorld().getTranslate() );

	EXPECT_TRUE( geometry1->getLocal().isIdentity() );
	EXPECT_FALSE( geometry1->getWorld().isIdentity() );
	EXPECT_EQ( Vector3f( 0, 0, -5 ), geometry1->getWorld().getTranslate() );

	EXPECT_TRUE( geometry2->getLocal().isIdentity() );
	EXPECT_FALSE( geometry2->getWorld().isIdentity() );
	EXPECT_EQ( Vector3f( 0, 0, -5 ), geometry2->getWorld().getTranslate() );

	EXPECT_TRUE( geometry3->getLocal().isIdentity() );
	EXPECT_FALSE( geometry3->getWorld().isIdentity() );
	EXPECT_EQ( Vector3f( 0, 0, -5 ), geometry3->getWorld().getTranslate() );

}

TEST( UpateWorldStateTest, scale )
{
	auto n0 = crimild::alloc< Group >();
	auto n1 = crimild::alloc< Group >();

	n0->attachNode( n1 );
	n0->local().setScale( 0.5f );

	EXPECT_FALSE( n0->getLocal().isIdentity() );
	EXPECT_TRUE( n1->getLocal().isIdentity() );

	n0->perform( UpdateWorldState() );

	EXPECT_EQ( 0.5f, n0->getWorld().getScale() );
	EXPECT_EQ( 0.5f, n1->getWorld().getScale() );
}

