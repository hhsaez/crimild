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

#include "SceneGraph/Geometry.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "Components/MaterialComponent.hpp"
#include "Streaming/FileStream.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Coding/MemoryDecoder.hpp"
 
#include "gtest/gtest.h"

using namespace crimild;

TEST( GeometryTest, construction )
{
	auto geometry = crimild::alloc< Geometry >( "a geometry" );

	EXPECT_EQ( geometry->getName(), "a geometry" );

	auto materials = geometry->getComponent< MaterialComponent >();
	ASSERT_NE( nullptr, materials );
	EXPECT_FALSE( materials->hasMaterials() );
}

TEST( GeometryTest, attachPrimitive )
{
	auto geometry = crimild::alloc< Geometry >( "a geometry" );

	auto primitive = crimild::alloc< Primitive >();

	EXPECT_FALSE( geometry->hasPrimitives() );	

	geometry->attachPrimitive( primitive );

	EXPECT_TRUE( geometry->hasPrimitives() );	
	
	bool found = false;
	int count = 0;
	geometry->forEachPrimitive( [&count, primitive, &found]( Primitive *p ) {
		++count;
		if ( p == crimild::get_ptr( primitive ) ) {
			found = true;
		}
	});

	EXPECT_EQ( count, 1 );
	EXPECT_TRUE( found );
}

TEST( GeometryTest, detachPrimitive )
{
	auto geometry = crimild::alloc< Geometry >( "a geometry" );

	auto primitive = crimild::alloc< Primitive >();

	EXPECT_FALSE( geometry->hasPrimitives() );	

	geometry->attachPrimitive( primitive );

	EXPECT_TRUE( geometry->hasPrimitives() );	

	geometry->detachPrimitive( primitive );

	EXPECT_FALSE( geometry->hasPrimitives() );	
	
	int count = 0;
	geometry->forEachPrimitive( [&count]( Primitive *p ) {
		++count;
	});

	EXPECT_EQ( count, 0 );
}

TEST( GeometryTest, detachAllPrimitives )
{
	auto geometry = crimild::alloc< Geometry >( "a geometry" );

	auto primitive1 = crimild::alloc< Primitive >();
	auto primitive2 = crimild::alloc< Primitive >();

	geometry->attachPrimitive( primitive1 );
	geometry->attachPrimitive( primitive2 );

	int count = 0;
	geometry->forEachPrimitive( [&count]( Primitive *p ) {
		++count;
	});
	EXPECT_EQ( count, 2 );

	geometry->detachAllPrimitives();

	count = 0;
	geometry->forEachPrimitive( [&count]( Primitive *p ) {
		++count;
	});
	EXPECT_EQ( count, 0 );
}

//TEST( GeometryTest, geometryStream )
//{
//	{
//		auto geometry = crimild::alloc< Geometry >( "a geometry" );
//		geometry->attachPrimitive( crimild::alloc< QuadPrimitive >( 100.0f, 200.0f, VertexFormat::VF_P3_N3 ) );
//
//		FileStream os( "geometry.crimild", FileStream::OpenMode::WRITE );
//		os.addObject( geometry );
//		EXPECT_TRUE( os.flush() );
//	}
//
//	{
//		FileStream is( "geometry.crimild", FileStream::OpenMode::READ );
//		EXPECT_TRUE( is.load() );
//		EXPECT_EQ( 1, is.getObjectCount() );
//
//		auto g = is.getObjectAt< Geometry >( 0 );
//		EXPECT_NE( nullptr, g );
//		EXPECT_EQ( "a geometry", g->getName() );
//
//		int primitiveCount = 0;
//		g->forEachPrimitive( [&]( Primitive *p ) {
//			++primitiveCount;
//			EXPECT_EQ( 4, p->getVertexBuffer()->getVertexCount() );
//		});
//		EXPECT_EQ( 1, primitiveCount );
//	}
//}

TEST( GeometryTest, coding )
{
	auto geometry = crimild::alloc< Geometry >( "a geometry" );
	geometry->attachPrimitive( crimild::alloc< QuadPrimitive >( 100.0f, 200.0f, VertexFormat::VF_P3_N3 ) );

    auto encoder = crimild::alloc< coding::MemoryEncoder >();
	encoder->encode( geometry );
    auto bytes = encoder->getBytes();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();
	decoder->fromBytes( bytes );

	auto g = decoder->getObjectAt< Geometry >( 0 );
	EXPECT_TRUE( g != nullptr );
	EXPECT_EQ( geometry->getName(), geometry->getName() );

	crimild::Int16 count = 0;
	g->forEachPrimitive( [ &count ]( Primitive *p ) {
		++count;
		EXPECT_TRUE( p->getVertexBuffer() != nullptr );
		EXPECT_EQ( 4, p->getVertexBuffer()->getVertexCount() );

		EXPECT_TRUE( p->getIndexBuffer() != nullptr );
		EXPECT_EQ( 4, p->getIndexBuffer()->getIndexCount() );
	});

	EXPECT_EQ( 1, count );
}

