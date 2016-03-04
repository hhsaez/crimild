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

	EXPECT_EQ( VertexFormat::VF_P3, vbo->getVertexFormat() );
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

	EXPECT_EQ( VertexFormat::VF_P3_N3_UV2, vbo->getVertexFormat() );
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

