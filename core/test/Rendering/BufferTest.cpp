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

#include "Rendering/Buffer.hpp"

#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Crimild_Mathematics.hpp"

#include <gtest/gtest.h>

using namespace crimild;

TEST( Buffer, constructionWithValueArray )
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

    ASSERT_EQ( 9 * sizeof( crimild::Real32 ), buffer->getSize() );
    ASSERT_NE( nullptr, buffer->getData() );
}

TEST( Buffer, withEmptyData )
{
    auto buffer = crimild::alloc< Buffer >( Array< crimild::Real32 >( 3 ) );

    ASSERT_EQ( 3 * sizeof( crimild::Real32 ), buffer->getSize() );
}

TEST( Buffer, constructionWithStructArray )
{
    struct Vertex {
        Vector3f position;
    };

    auto data = Array< Vertex > {
        { .position = { -0.5f, -0.5f, 0.0f } },
        { .position = { 0.5f, -0.5f, 0.0f } },
        { .position = { 0.0f, 0.5f, 0.0f } },
    };

    auto buffer = crimild::alloc< Buffer >( data );

    ASSERT_EQ( 9 * sizeof( crimild::Real32 ), buffer->getSize() );
    ASSERT_NE( nullptr, buffer->getData() );

    ASSERT_TRUE( true );
}

TEST( Buffer, constructionWithStruct )
{
    struct Uniform {
        Matrix4f proj;
        Matrix4f view;
        Matrix4f model;
        ColorRGBA color;
        crimild::Real32 metalness;
    };

    auto buffer = crimild::alloc< Buffer >(
        Uniform {
            .color = ColorRGBA { 0.5f, 0.75f, 0.95f, 1.0f },
            .metalness = 0.5f,
        }
    );

    ASSERT_EQ( sizeof( Uniform ), buffer->getSize() );
    ASSERT_NE( nullptr, buffer->getData() );
    ASSERT_EQ( ( ColorRGBA { 0.5f, 0.75f, 0.95f, 1.0f } ), static_cast< Uniform * >( static_cast< void * >( buffer->getData() ) )->color );
    ASSERT_EQ( 0.5f, static_cast< Uniform * >( static_cast< void * >( buffer->getData() ) )->metalness );
}

TEST( Buffer, coding )
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
    coding::MemoryEncoder encoder;
    ASSERT_TRUE( encoder.encode( buffer ) );
    const auto bytes = encoder.getBytes();

    coding::MemoryDecoder decoder;
    ASSERT_TRUE( decoder.fromBytes( bytes ) );
    ASSERT_EQ( 1, decoder.getObjectCount() );
    auto decoded = decoder.getObjectAt< Buffer >( 0 );

    EXPECT_EQ( buffer->getSize(), decoded->getSize() );
    EXPECT_EQ( 0, memcmp( buffer->getData(), decoded->getData(), decoded->getSize() ) );
}
