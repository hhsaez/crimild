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

#include "Rendering/VertexBufferObject.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( VertexBufferObject, construction )
{
	VertexPrecision vertices[] = {
		1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, 3, vertices );

	// EXPECT_EQ( VertexFormat::VF_P3, vbo->getVertexFormat() );
	EXPECT_EQ( 3, vbo->getVertexCount() );
	EXPECT_EQ( 3 * vbo->getVertexFormat().getVertexSize(), vbo->getSize() );
	EXPECT_EQ( 0, memcmp( vertices, vbo->getData(), sizeof( VertexPrecision ) * vbo->getSize() ) );
}

TEST( VertexBufferObject, complexConstruction )
{
	VertexPrecision vertices[] = {
		+1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0,
		-1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
		+0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.5f, 0.0f
	};

	auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3_N3_UV2, 3, vertices );

	// EXPECT_EQ( VertexFormat::VF_P3_N3_UV2, vbo->getVertexFormat() );
	EXPECT_EQ( 3, vbo->getVertexCount() );
	EXPECT_EQ( 3 * vbo->getVertexFormat().getVertexSize(), vbo->getSize() );
	EXPECT_EQ( 0, memcmp( vertices, vbo->getData(), sizeof( VertexPrecision ) * vbo->getSize() ) );
}

TEST( VertexBufferObject, positions )
{
	auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, 3 );
	vbo->setPositionAt( 0, Vector3f( -1.0f, -1.0f, 0.0f ) );
	vbo->setPositionAt( 1, Vector3f( 1.0f, -1.0f, 0.0f ) );
	vbo->setPositionAt( 2, Vector3f( 0.0f, 1.0f, 0.0f ) );

	EXPECT_EQ( 3, vbo->getVertexCount() );

	auto data = vbo->getData();
	EXPECT_EQ( -1.0f, data[ 0 ] );
	EXPECT_EQ( -1.0f, data[ 1 ] );
	EXPECT_EQ( 0.0f, data[ 2 ] );
	EXPECT_EQ( 1.0f, data[ 3 ] );
	EXPECT_EQ( -1.0f, data[ 4 ] );
	EXPECT_EQ( 0.0f, data[ 5 ] );
	EXPECT_EQ( 0.0f, data[ 6 ] );
	EXPECT_EQ( 1.0f, data[ 7 ] );
	EXPECT_EQ( 0.0f, data[ 8 ] );

	EXPECT_EQ( Vector3f( -1.0f, -1.0f, 0.0f ), vbo->getPositionAt( 0 ) );
	EXPECT_EQ( Vector3f( 1.0f, -1.0f, 0.0f ), vbo->getPositionAt( 1 ) );
	EXPECT_EQ( Vector3f( 0.0f, 1.0f, 0.0f ), vbo->getPositionAt( 2 ) );
}

TEST( VertexBufferObject, boneData )
{
	auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat( 0, 0, 0, 0, 0, 3, 3 ), 3 );

	vbo->setBoneIdAt( 0, 0, 22 );
	vbo->setBoneWeightAt( 0, 0, 0.3f );
	vbo->setBoneIdAt( 0, 1, 38 );
	vbo->setBoneWeightAt( 0, 1, 0.7f );

	vbo->setBoneIdAt( 1, 0, 68 );
	vbo->setBoneWeightAt( 1, 0, 0.2 );
	vbo->setBoneIdAt( 1, 1, 14 );
	vbo->setBoneWeightAt( 1, 1, 0.7 );
	vbo->setBoneIdAt( 1, 2, 7 );
	vbo->setBoneWeightAt( 1, 2, 0.1 );

	vbo->setBoneIdAt( 2, 0, 11 );
	vbo->setBoneWeightAt( 2, 0, 1.0 );

	float test_data[] = { 22, 38, 0, 0.3, 0.7, 0, 68, 14, 7, 0.2, 0.7, 0.1, 11, 0, 0, 1, 0, 0 };
	for ( int i = 0; i < vbo->getVertexFormat().getVertexSize() * vbo->getVertexCount(); i++ ) {
		EXPECT_EQ( test_data[ i ], vbo->getData()[ i ] );
	}

	EXPECT_EQ( 22, vbo->getBoneIdAt( 0, 0 ) );
	EXPECT_EQ( 0.3f, vbo->getBoneWeightAt( 0, 0 ) );
	EXPECT_EQ( 38, vbo->getBoneIdAt( 0, 1 ) );
	EXPECT_EQ( 0.7f, vbo->getBoneWeightAt( 0, 1 ) );

	EXPECT_EQ( 68, vbo->getBoneIdAt( 1, 0 ) );
	EXPECT_EQ( 0.2f, vbo->getBoneWeightAt( 1, 0 ) );
	EXPECT_EQ( 14, vbo->getBoneIdAt( 1, 1 ) );
	EXPECT_EQ( 0.7f, vbo->getBoneWeightAt( 1, 1 ) );
	EXPECT_EQ( 7, vbo->getBoneIdAt( 1, 2 ) );
	EXPECT_EQ( 0.1f, vbo->getBoneWeightAt( 1, 2 ) );

	EXPECT_EQ( 11, vbo->getBoneIdAt( 2, 0 ) );
	EXPECT_EQ( 1.0f, vbo->getBoneWeightAt( 2, 0 ) );
}

TEST( VertexBufferObject, vboStream )
{
	VertexPrecision vertices[] = {
		+1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0,
		-1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
		+0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.5f, 0.0f
	};

	auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3_N3_UV2, 3, vertices );

	{
		FileStream os( "vbo.crimild", FileStream::OpenMode::WRITE );
		os.addObject( vbo );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "vbo.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		auto vbo1 = is.getObjectAt< VertexBufferObject >( 0 );
		EXPECT_TRUE( vbo1 != nullptr );
		EXPECT_EQ( vbo->getVertexFormat(), vbo1->getVertexFormat() );
		EXPECT_EQ( vbo->getVertexFormat().getVertexSizeInBytes(), vbo1->getVertexFormat().getVertexSizeInBytes() );
		EXPECT_EQ( vbo->getVertexCount(), vbo1->getVertexCount() );
		EXPECT_EQ( 0, memcmp( vbo->getData(), vbo1->getData(), vbo1->getVertexFormat().getVertexSizeInBytes() * vbo1->getVertexCount() ) );
	}
}

