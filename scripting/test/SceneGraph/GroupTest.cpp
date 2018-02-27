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
#include "Coding/LuaEncoder.hpp"
#include "Coding/LuaDecoder.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( GroupTest, luaCoding )
{
	//		node0
	//		/   \
	//	node1	node2
	//			/	\
	//		node3	node4

    auto encoder = crimild::alloc< coding::LuaEncoder >();

	{
		auto node0 = crimild::alloc< Group >( "node0" );
		auto node1 = crimild::alloc< Group >( "node1" );
		node0->attachNode( node1 );
		auto node2 = crimild::alloc< Group >( "node2" );
		node0->attachNode( node2 );
		auto node3 = crimild::alloc< Group >( "node3" );
		node2->attachNode( node3 );
		auto node4 = crimild::alloc< Group >( "node4" );
		node2->attachNode( node4 );

		encoder->encode( node0 );
	}
		
    auto encoded = "scene = " + encoder->getEncodedString();
    auto decoder = crimild::alloc< coding::LuaDecoder >();
    decoder->parse( encoded );

	{
		auto n0 = decoder->getObjectAt< Group >( 0 );
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

