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

#include "Coding/JSONDecoder.hpp"
#include "Coding/JSONEncoder.hpp"
#include "Mathematics/Transformation_equality.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/io.hpp"
#include "Rendering/Buffer.hpp"
#include "SceneGraph/Group.hpp"

#include <gtest/gtest.h>

using namespace crimild;

TEST( JSONCoding, encode_version )
{
    const auto json = [] {
        coding::JSONEncoder encoder;
        return encoder.getResult();
    }();

    const auto expected = nlohmann::json {
        { "__VERSION__", "CRIMILD v5.0.0" },
    };

    EXPECT_EQ( expected, json );
}

TEST( JSONCoding, decode_version )
{
    const auto json = nlohmann::json {
        { "__VERSION__", "CRIMILD v5.0.0" },
    };

    coding::JSONDecoder decoder;
    ASSERT_TRUE( decoder.fromJSON( json ) );
    const auto version = decoder.getVersion();

    EXPECT_EQ( 5, version.getMajor() );
    EXPECT_EQ( 0, version.getMinor() );
    EXPECT_EQ( 0, version.getPatch() );
}

TEST( JSONCoding, encode_node )
{
    const auto json = [] {
        auto node = crimild::alloc< Node >( "a node" );
        coding::JSONEncoder encoder;
        encoder.encode( node );
        return encoder.getResult();
    }();

    coding::JSONDecoder decoder;
    ASSERT_TRUE( decoder.fromJSON( json ) );

    ASSERT_EQ( 1, decoder.getObjectCount() );

    auto node = decoder.getObjectAt< Node >( 0 );
    ASSERT_EQ( "a node", node->getName() );
}

TEST( JSONCoding, encode_transformed_node )
{
    auto n1 = crimild::alloc< Node >( "Some Node" );
    n1->setLocal( translation( 0.0f, 0.0f, -5.0f ) );
    n1->setWorld( translation( 1.0f, 0.0f, -5.0f ) );
    n1->setWorldIsCurrent( true );

    coding::JSONEncoder encoder;
    encoder.encode( n1 );
    const auto json = encoder.getResult();
    coding::JSONDecoder decoder;
    ASSERT_TRUE( decoder.fromJSON( json ) );

    auto n2 = decoder.getObjectAt< Node >( 0 );
    EXPECT_TRUE( n2 != nullptr );
    EXPECT_EQ( n1->getName(), n2->getName() );
    EXPECT_EQ( n1->getLocal(), n2->getLocal() );
    EXPECT_EQ( n1->getWorld(), n2->getWorld() );
    EXPECT_TRUE( n2->worldIsCurrent() );
}

TEST( JSONCoding, encode_group )
{
    auto group = crimild::alloc< Group >();
    group->setName( "a group" );

    coding::JSONEncoder encoder;
    encoder.encode( group );
    const auto json = encoder.getResult();
    coding::JSONDecoder decoder;
    ASSERT_TRUE( decoder.fromJSON( json ) );

    EXPECT_EQ( 1, decoder.getObjectCount() );

    auto res = decoder.getObjectAt< Group >( 0 );
    EXPECT_EQ( 0, res->getNodeCount() );
    EXPECT_EQ( "a group", res->getName() );
}

TEST( JSONCoding, encodes_a_group_with_one_child_node )
{
    auto group = crimild::alloc< Group >();
    group->setName( "a group" );
    group->attachNode( crimild::alloc< Node >( "a node" ) );

    coding::JSONEncoder encoder;
    encoder.encode( group );
    const auto json = encoder.getResult();
    coding::JSONDecoder decoder;
    ASSERT_TRUE( decoder.fromJSON( json ) );

    EXPECT_EQ( 1, decoder.getObjectCount() );

    auto res = decoder.getObjectAt< Group >( 0 );
    EXPECT_EQ( "a group", res->getName() );
    ASSERT_EQ( 1, res->getNodeCount() );
    EXPECT_EQ( "a node", res->getNodeAt( 0 )->getName() );
}

TEST( JSONCoding, encodes_group_with_child_nodes )
{
    //		node0
    //		/   \
	//	node1	node2
    //			/	\
	//		node3	node4

    coding::JSONEncoder encoder;

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

        encoder.encode( node0 );
    }

    const auto json = encoder.getResult();
    coding::JSONDecoder decoder;
    ASSERT_TRUE( decoder.fromJSON( json ) );

    {
        auto n0 = decoder.getObjectAt< Group >( 0 );
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

TEST( JSONCoding, encodes_a_buffer_of_raw_data )
{
    auto data = Array< crimild::Real32 > {
        -0.5f,
        -0.5f,
        0.0f,
        0.5f,
        -0.5f,
        0.0f,
        0.0f,
        0.5f,
        0.0f,
    };

    auto buffer = crimild::alloc< Buffer >( data );
    coding::JSONEncoder encoder;
    ASSERT_TRUE( encoder.encode( buffer ) );
    const auto json = encoder.getResult();

    coding::JSONDecoder decoder;
    ASSERT_TRUE( decoder.fromJSON( json ) );
    ASSERT_EQ( 1, decoder.getObjectCount() );
    auto decoded = decoder.getObjectAt< Buffer >( 0 );

    EXPECT_EQ( buffer->getSize(), decoded->getSize() );
    EXPECT_EQ( 0, memcmp( buffer->getData(), decoded->getData(), decoded->getSize() ) );
}
