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

#include "Rendering/IndexBuffer.hpp"
#include "Rendering/Buffer.hpp"
#include "Rendering/BufferView.hpp"
#include "Rendering/BufferAccessor.hpp"
#include "Rendering/Format.hpp"
#include "Rendering/FrameGraph.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( IndexBuffer, constructionUInt32 )
{
    auto indices = crimild::alloc< IndexBuffer >(
        Format::INDEX_32_UINT,
        Array< crimild::UInt32 > {
            0, 1, 2,
            0, 2, 3,
        }
    );

    ASSERT_NE( nullptr, indices->getBufferView() );
	ASSERT_EQ( 6, indices->getIndexCount() );
	ASSERT_EQ( Format::INDEX_32_UINT, indices->getFormat() );
	ASSERT_EQ( 0, indices->getIndex( 0 ) );
	ASSERT_EQ( 1, indices->getIndex( 1 ) );
	ASSERT_EQ( 2, indices->getIndex( 2 ) );
	ASSERT_EQ( 0, indices->getIndex( 3 ) );
	ASSERT_EQ( 2, indices->getIndex( 4 ) );
	ASSERT_EQ( 3, indices->getIndex( 5 ) );
}

TEST( IndexBuffer, constructionUInt16 )
{
    auto indices = crimild::alloc< IndexBuffer >(
        Format::INDEX_16_UINT,
        Array< crimild::UInt16 > {
            0, 1, 2,
            0, 2, 3,
        }
    );

    ASSERT_NE( nullptr, indices->getBufferView() );
	ASSERT_EQ( 6, indices->getIndexCount() );
	ASSERT_EQ( Format::INDEX_16_UINT, indices->getFormat() );
	ASSERT_EQ( 0, indices->getIndex( 0 ) );
	ASSERT_EQ( 1, indices->getIndex( 1 ) );
	ASSERT_EQ( 2, indices->getIndex( 2 ) );
	ASSERT_EQ( 0, indices->getIndex( 3 ) );
	ASSERT_EQ( 2, indices->getIndex( 4 ) );
	ASSERT_EQ( 3, indices->getIndex( 5 ) );
}

TEST( IndexBuffer, setIndex )
{
    auto indices = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, 6 );

    ASSERT_NE( nullptr, indices->getBufferView() );
	ASSERT_EQ( 6, indices->getIndexCount() );
	ASSERT_EQ( Format::INDEX_32_UINT, indices->getFormat() );

    indices->setIndex( 0, 0 );
    indices->setIndex( 1, 1 );
    indices->setIndex( 2, 2 );
    indices->setIndex( 3, 0 );
    indices->setIndex( 4, 2 );
    indices->setIndex( 5, 3 );
    
	ASSERT_EQ( 0, indices->getIndex( 0 ) );
	ASSERT_EQ( 1, indices->getIndex( 1 ) );
	ASSERT_EQ( 2, indices->getIndex( 2 ) );
	ASSERT_EQ( 0, indices->getIndex( 3 ) );
	ASSERT_EQ( 2, indices->getIndex( 4 ) );
	ASSERT_EQ( 3, indices->getIndex( 5 ) );
}

TEST( IndexBuffer, setIndices )
{
    auto indices = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, 6 );

    ASSERT_NE( nullptr, indices->getBufferView() );
	ASSERT_EQ( 6, indices->getIndexCount() );
	ASSERT_EQ( Format::INDEX_32_UINT, indices->getFormat() );

    auto data = Array< crimild::UInt32 > {
        0, 1, 2,
        0, 2, 3,
    };

    indices->setIndices( data );
    
	ASSERT_EQ( 0, indices->getIndex( 0 ) );
	ASSERT_EQ( 1, indices->getIndex( 1 ) );
	ASSERT_EQ( 2, indices->getIndex( 2 ) );
	ASSERT_EQ( 0, indices->getIndex( 3 ) );
	ASSERT_EQ( 2, indices->getIndex( 4 ) );
	ASSERT_EQ( 3, indices->getIndex( 5 ) );
}

TEST( IndexBuffer, eachIndex )
{
    auto indices = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, 6 );

    ASSERT_NE( nullptr, indices->getBufferView() );
	ASSERT_EQ( 6, indices->getIndexCount() );
	ASSERT_EQ( Format::INDEX_32_UINT, indices->getFormat() );

    auto data = Array< crimild::UInt32 > {
        0, 1, 2,
        0, 2, 3,
    };

    indices->setIndices( data );

    auto callCount = 0;
    indices->each(
        [&]( auto val, auto i ) {
            ASSERT_EQ( data[ i ], val );
            callCount++;
        }
    );
    ASSERT_EQ( 6, callCount );
}

TEST( IndexBuffer, autoAddToFrameGraph )
{
	auto graph = crimild::alloc< FrameGraph >();

	ASSERT_FALSE( graph->hasNodes() );

	{
		auto indexBuffer = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, 0 );

		ASSERT_TRUE( graph->contains( indexBuffer ) );
        ASSERT_TRUE( graph->contains( indexBuffer->getBufferView() ) );
        ASSERT_TRUE( graph->contains( indexBuffer->getBufferView()->getBuffer() ) );
		ASSERT_TRUE( graph->hasNodes() );
	}

    ASSERT_FALSE( graph->hasNodes() );
}

