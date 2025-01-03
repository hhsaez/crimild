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

#include "Rendering/Image.hpp"

#include <Crimild_Coding.hpp>
#include <gtest/gtest.h>

using namespace crimild;

TEST( Image, construction )
{
    auto image = crimild::alloc< Image >();

    EXPECT_EQ( FrameGraphResource::Type::IMAGE, image->getType() );
}

TEST( Image, coding )
{
    auto image = Image::CHECKERBOARD_4;

    coding::MemoryEncoder encoder;
    ASSERT_TRUE( encoder.encode( image ) );
    const auto bytes = encoder.getBytes();

    coding::MemoryDecoder decoder;
    ASSERT_TRUE( decoder.fromBytes( bytes ) );
    ASSERT_EQ( 1, decoder.getObjectCount() );
    auto decoded = decoder.getObjectAt< Image >( 0 );

    ASSERT_NE( nullptr, decoded );
    ASSERT_EQ( image->format, decoded->format );
    ASSERT_EQ( image->type, decoded->type );
    ASSERT_EQ( image->extent.width, decoded->extent.width );
    ASSERT_EQ( image->extent.height, decoded->extent.height );
    ASSERT_EQ( image->extent.depth, decoded->extent.depth );
    ASSERT_NE( nullptr, decoded->getBufferView() );
    ASSERT_EQ( image->getBufferView()->getLength(), decoded->getBufferView()->getLength() );
    ASSERT_EQ( 0, memcmp( image->getBufferView()->getData(), decoded->getBufferView()->getData(), decoded->getBufferView()->getLength() ) );
    ASSERT_EQ( image->getLayerCount(), decoded->getLayerCount() );
    ASSERT_EQ( image->getMipLevels(), decoded->getMipLevels() );
}
