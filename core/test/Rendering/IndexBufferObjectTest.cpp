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

#include "Rendering/IndexBufferObject.hpp"
#include "Streaming/FileStream.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Coding/MemoryDecoder.hpp"
 
#include "gtest/gtest.h"

using namespace crimild;

TEST( IndexBufferObjectTest, construction )
{
	IndexPrecision indices[] = { 0, 1, 2 };

	auto ibo = crimild::alloc< IndexBufferObject >( 3, indices );

	EXPECT_EQ( 3, ibo->getIndexCount() );
	EXPECT_EQ( 0, memcmp( indices, ibo->getData(), sizeof( IndexPrecision ) * ibo->getSize() ) );
}

TEST( IndexBufferObjectTest, setIndexAt )
{
	auto ibo = crimild::alloc< IndexBufferObject >( 6 );
	ibo->setIndexAt( 0, 0 );
	ibo->setIndexAt( 1, 1 );
	ibo->setIndexAt( 2, 2 );
	ibo->setIndexAt( 3, 0 );
	ibo->setIndexAt( 4, 2 );
	ibo->setIndexAt( 5, 3 );

	EXPECT_EQ( 0, ibo->getIndexAt( 0 ) );
	EXPECT_EQ( 1, ibo->getIndexAt( 1 ) );
	EXPECT_EQ( 2, ibo->getIndexAt( 2 ) );
	EXPECT_EQ( 0, ibo->getIndexAt( 3 ) );
	EXPECT_EQ( 2, ibo->getIndexAt( 4 ) );
	EXPECT_EQ( 3, ibo->getIndexAt( 5 ) );
}

TEST( IndexBufferObjectTest, testGenerateIncrementalIndices )
{
	auto ibo = crimild::alloc< IndexBufferObject >( 6 );
	ibo->generateIncrementalIndices();

	EXPECT_EQ( 0, ibo->getIndexAt( 0 ) );
	EXPECT_EQ( 1, ibo->getIndexAt( 1 ) );
	EXPECT_EQ( 2, ibo->getIndexAt( 2 ) );
	EXPECT_EQ( 3, ibo->getIndexAt( 3 ) );
	EXPECT_EQ( 4, ibo->getIndexAt( 4 ) );
	EXPECT_EQ( 5, ibo->getIndexAt( 5 ) );
}

TEST( IndexBufferObjectTest, coding )
{
    auto ibo = crimild::alloc< IndexBufferObject >( 6 );
    ibo->generateIncrementalIndices();
    
    auto encoder = crimild::alloc< coding::MemoryEncoder >();
    encoder->encode( ibo );
    auto bytes = encoder->getBytes();
    auto decoder = crimild::alloc< coding::MemoryDecoder >();
    decoder->fromBytes( bytes );
    
    auto ibo1 = decoder->getObjectAt< IndexBufferObject >( 0 );
    EXPECT_TRUE( ibo1 != nullptr );
    EXPECT_EQ( ibo->getIndexCount(), ibo1->getIndexCount() );
    EXPECT_EQ( 0, memcmp( ibo->getData(), ibo1->getData(), sizeof( IndexPrecision ) * ibo1->getIndexCount() ) );
}

TEST( IndexBufferObjectTest, iboStream )
{
	auto ibo = crimild::alloc< IndexBufferObject >( 6 );
	ibo->generateIncrementalIndices();

	{
		FileStream os( "ibo.crimild", FileStream::OpenMode::WRITE );
		os.addObject( ibo );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "ibo.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		auto ibo1 = is.getObjectAt< IndexBufferObject >( 0 );
		EXPECT_TRUE( ibo1 != nullptr );
		EXPECT_EQ( ibo->getIndexCount(), ibo1->getIndexCount() );
		EXPECT_EQ( 0, memcmp( ibo->getData(), ibo1->getData(), sizeof( IndexPrecision ) * ibo1->getIndexCount() ) );
	}
}

