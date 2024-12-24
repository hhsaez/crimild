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

#include "Rendering/ImageView.hpp"

#include "Rendering/Image.hpp"

#include <Crimild_Coding.hpp>
#include <gtest/gtest.h>

using namespace crimild;

TEST( ImageView, construction )
{
    auto imageView = crimild::alloc< ImageView >();

    EXPECT_EQ( FrameGraphResource::Type::IMAGE_VIEW, imageView->getType() );
}

TEST( ImageView, coding )
{
    auto image = Image::CHECKERBOARD_4;
    auto imageView = crimild::alloc< ImageView >();
    imageView->image = image;
    imageView->format = image->format;
    imageView->type = ImageView::Type( Int32( image->type ) );
    imageView->mipLevels = image->getMipLevels();
    imageView->layerCount = image->getLayerCount();

    coding::MemoryEncoder encoder;
    ASSERT_TRUE( encoder.encode( imageView ) );
    const auto bytes = encoder.getBytes();

    coding::MemoryDecoder decoder;
    ASSERT_TRUE( decoder.fromBytes( bytes ) );
    ASSERT_EQ( 1, decoder.getObjectCount() );
    auto decoded = decoder.getObjectAt< ImageView >( 0 );

    ASSERT_NE( nullptr, decoded );
    ASSERT_EQ( imageView->format, decoded->format );
    ASSERT_EQ( imageView->type, decoded->type );
    ASSERT_NE( nullptr, decoded->image );
    ASSERT_EQ( imageView->mipLevels, decoded->mipLevels );
    ASSERT_EQ( imageView->layerCount, decoded->layerCount );
}
