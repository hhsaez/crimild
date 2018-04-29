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

#include "Primitives/Primitive.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Coding/MemoryDecoder.hpp"
#include "Streaming/FileStream.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( PrimitiveTest, construction )
{
	auto p1 = crimild::alloc< Primitive >();
	EXPECT_EQ( p1->getType(), Primitive::Type::TRIANGLES );

	auto p2 = crimild::alloc< Primitive >( Primitive::Type::LINES );
	EXPECT_EQ( p2->getType(), Primitive::Type::LINES );
}

TEST( PrimitiveTest, destruction )
{

}

TEST( PrimitiveTest, setVertexBuffer )
{
	auto p = crimild::alloc< Primitive >();

	EXPECT_EQ( p->getVertexBuffer(), nullptr );

	auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, 0, nullptr );
	p->setVertexBuffer( vbo );

    EXPECT_EQ( p->getVertexBuffer(), crimild::get_ptr( vbo ) );
}

TEST( PrimitiveTest, setIndexBuffer )
{
	auto p = crimild::alloc< Primitive >();

	EXPECT_EQ( p->getIndexBuffer(), nullptr );

	auto ibo = crimild::alloc< IndexBufferObject >( 0, nullptr );
	p->setIndexBuffer( ibo );

    EXPECT_EQ( p->getIndexBuffer(), crimild::get_ptr( ibo ) );
}

TEST( PrimitiveTest, coding )
{
	auto primitive = crimild::alloc< Primitive >( Primitive::Type::POINTS );

	auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, 3 );
	vbo->setPositionAt( 0, Vector3f( -1.0f, 0.0f, 0.0f ) );
	vbo->setPositionAt( 1, Vector3f( 1.0f, 0.0f, 0.0f ) );
	vbo->setPositionAt( 1, Vector3f( 0.0f, 1.0f, 0.0f ) );
	primitive->setVertexBuffer( vbo );

	auto ibo = crimild::alloc< IndexBufferObject >( 3 );
	ibo->generateIncrementalIndices();
	primitive->setIndexBuffer( ibo );
	
	auto encoder = crimild::alloc< coding::MemoryEncoder >();
	encoder->encode( primitive );
	auto bytes = encoder->getBytes();
	auto decoder = crimild::alloc< coding::MemoryDecoder >();
	decoder->fromBytes( bytes );
	
	auto p = decoder->getObjectAt< Primitive >( 0 );
	EXPECT_TRUE( p != nullptr );
	EXPECT_EQ( p->getType(), Primitive::Type::POINTS );
	
	EXPECT_TRUE( p->getVertexBuffer() != nullptr );
	EXPECT_EQ( primitive->getVertexBuffer()->getVertexCount(), p->getVertexBuffer()->getVertexCount() );
	
	EXPECT_TRUE( p->getIndexBuffer() != nullptr );
	EXPECT_EQ( primitive->getIndexBuffer()->getIndexCount(), p->getIndexBuffer()->getIndexCount() );
}

TEST( PrimitiveTest, primitiveStream )
{
	auto primitive = crimild::alloc< Primitive >( Primitive::Type::POINTS );

	auto vbo = crimild::alloc< VertexBufferObject >( VertexFormat::VF_P3, 3 );
	vbo->setPositionAt( 0, Vector3f( -1.0f, 0.0f, 0.0f ) );
	vbo->setPositionAt( 1, Vector3f( 1.0f, 0.0f, 0.0f ) );
	vbo->setPositionAt( 1, Vector3f( 0.0f, 1.0f, 0.0f ) );
	primitive->setVertexBuffer( vbo );

	auto ibo = crimild::alloc< IndexBufferObject >( 3 );
	ibo->generateIncrementalIndices();
	primitive->setIndexBuffer( ibo );

	{
		FileStream os( "primitive.crimild", FileStream::OpenMode::WRITE );
		os.addObject( primitive );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "primitive.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		
		auto p = is.getObjectAt< Primitive >( 0 );
		EXPECT_TRUE( p != nullptr );
		EXPECT_EQ( p->getType(), Primitive::Type::POINTS );
		
		EXPECT_TRUE( p->getVertexBuffer() != nullptr );
		EXPECT_EQ( primitive->getVertexBuffer()->getVertexCount(), p->getVertexBuffer()->getVertexCount() );
		
		EXPECT_TRUE( p->getIndexBuffer() != nullptr );
		EXPECT_EQ( primitive->getIndexBuffer()->getIndexCount(), p->getIndexBuffer()->getIndexCount() );
	}
}

