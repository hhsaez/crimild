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

#include "Rendering/BufferView.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( BufferView, constructionWithDefaults )
{
    auto data = containers::Array< crimild::Real32 > {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };

    auto buffer = crimild::alloc< Buffer2 >( data );

    auto bufferView = crimild::alloc< BufferView >(
        BufferView::Target::VERTEX,
        buffer
    );

    ASSERT_EQ( crimild::get_ptr( buffer ), bufferView->getBuffer() );
    ASSERT_EQ( 0, bufferView->getOffset() );
    ASSERT_EQ( buffer->getSize(), bufferView->getLength() );
    ASSERT_EQ( sizeof( crimild::Byte ), bufferView->getStride() );
    ASSERT_EQ( buffer->getSize(), bufferView->getCount() );
    
    ASSERT_NE( nullptr, bufferView->getData() );
    ASSERT_EQ( buffer->getData(), bufferView->getData() );
}

TEST( BufferView, constructionWithLength )
{
    auto data = containers::Array< crimild::Real32 > {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };

    auto buffer = crimild::alloc< Buffer2 >( data );

    auto bufferView = crimild::alloc< BufferView >(
        BufferView::Target::VERTEX,
        buffer,
        0, // offset
        0, // stride
        12 // length in bytes
    );

    ASSERT_EQ( crimild::get_ptr( buffer ), bufferView->getBuffer() );
    ASSERT_EQ( 0, bufferView->getOffset() );
    ASSERT_EQ( 12, bufferView->getLength() );
    ASSERT_EQ( sizeof( crimild::Byte ), bufferView->getStride() );
    ASSERT_EQ( 12, bufferView->getCount() );
    
    ASSERT_NE( nullptr, bufferView->getData() );
    ASSERT_EQ( buffer->getData(), bufferView->getData() );
}

TEST( BufferView, constructionWithOffset )
{
    auto data = containers::Array< crimild::Real32 > {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };

    auto buffer = crimild::alloc< Buffer2 >( data );

    auto bufferView = crimild::alloc< BufferView >(
        BufferView::Target::VERTEX,
        buffer,
        12
    );

    ASSERT_EQ( crimild::get_ptr( buffer ), bufferView->getBuffer() );
    ASSERT_EQ( 12, bufferView->getOffset() );
    ASSERT_EQ( 24, bufferView->getLength() );
    ASSERT_EQ( sizeof( crimild::Byte ), bufferView->getStride() );
    ASSERT_EQ( 24, bufferView->getCount() );
    
    ASSERT_NE( nullptr, bufferView->getData() );
    ASSERT_EQ( static_cast< crimild::Byte * >( buffer->getData() ) + 12, bufferView->getData() );
}

TEST( BufferView, constructionWithOffsetAndLength )
{
    auto data = containers::Array< crimild::Real32 > {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };

    auto buffer = crimild::alloc< Buffer2 >( data );

    auto bufferView = crimild::alloc< BufferView >(
        BufferView::Target::VERTEX,
        buffer,
        12,
        0,
        12
    );

    ASSERT_EQ( crimild::get_ptr( buffer ), bufferView->getBuffer() );
    ASSERT_EQ( 12, bufferView->getOffset() );
    ASSERT_EQ( 12, bufferView->getLength() );
    ASSERT_EQ( sizeof( crimild::Byte ), bufferView->getStride() );
    ASSERT_EQ( 12, bufferView->getCount() );
    
    ASSERT_NE( nullptr, bufferView->getData() );
    ASSERT_EQ( static_cast< crimild::Byte * >( buffer->getData() ) + 12, bufferView->getData() );
}

TEST( BufferView, constructionWithStride )
{
    auto data = containers::Array< crimild::Real32 > {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };

    auto buffer = crimild::alloc< Buffer2 >( data );

    auto bufferView = crimild::alloc< BufferView >(
        BufferView::Target::VERTEX,
        buffer,
        0,
        sizeof( crimild::Real32 )
    );

    ASSERT_EQ( crimild::get_ptr( buffer ), bufferView->getBuffer() );
    ASSERT_EQ( 0, bufferView->getOffset() );
    ASSERT_EQ( buffer->getSize(), bufferView->getLength() );
    ASSERT_EQ( sizeof( crimild::Real32 ), bufferView->getStride() );
    ASSERT_EQ( buffer->getSize() / sizeof( crimild::Real32 ), bufferView->getCount() );
    
    ASSERT_NE( nullptr, bufferView->getData() );
    ASSERT_EQ( buffer->getData(), bufferView->getData() );
}

TEST( BufferView, withEmptyData )
{
    auto bufferView = crimild::alloc< BufferView >(
        BufferView::Target::VERTEX,
        crimild::alloc< Buffer2 >( containers::Array< crimild::Real32 >( 3 ) ),
        0,
        sizeof( crimild::Real32 )
    );

    ASSERT_EQ( 3 * sizeof( Real32 ), bufferView->getLength() );
    ASSERT_EQ( sizeof( Real32 ), bufferView->getStride() );
    ASSERT_EQ( 3, bufferView->getCount() );
}

