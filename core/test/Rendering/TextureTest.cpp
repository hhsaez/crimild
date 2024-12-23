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

#include "Rendering/Texture.hpp"

#include "Rendering/Image.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Sampler.hpp"

#include <Crimild_Coding.hpp>
#include <gtest/gtest.h>

using namespace crimild;

TEST( Texture, construction )
{
    auto texture = crimild::alloc< Texture >();
    texture->imageView = [ & ] {
        auto imageView = crimild::alloc< ImageView >();
        imageView->image = Image::CHECKERBOARD_16;
        return imageView;
    }();
    texture->sampler = [ & ] {
        auto sampler = crimild::alloc< Sampler >();
        sampler->setMinFilter( Sampler::Filter::NEAREST );
        sampler->setMagFilter( Sampler::Filter::NEAREST );
        return sampler;
    }();

    EXPECT_EQ( FrameGraphResource::Type::TEXTURE, texture->getType() );

    EXPECT_NE( nullptr, texture->imageView );
    EXPECT_NE( nullptr, texture->sampler );
}

TEST( Texture, coding )
{
    auto texture = crimild::alloc< Texture >();
    texture->setName( "color" );
    texture->imageView = [ & ] {
        auto imageView = crimild::alloc< ImageView >();
        imageView->image = Image::CHECKERBOARD_16;
        return imageView;
    }();
    texture->sampler = [ & ] {
        auto sampler = crimild::alloc< Sampler >();
        sampler->setMinFilter( Sampler::Filter::NEAREST );
        sampler->setMagFilter( Sampler::Filter::NEAREST );
        return sampler;
    }();

    coding::MemoryEncoder encoder;
    ASSERT_TRUE( encoder.encode( texture ) );
    const auto bytes = encoder.getBytes();

    coding::MemoryDecoder decoder;
    ASSERT_TRUE( decoder.fromBytes( bytes ) );
    ASSERT_EQ( 1, decoder.getObjectCount() );
    auto decoded = decoder.getObjectAt< Texture >( 0 );

    ASSERT_NE( nullptr, decoded );
    EXPECT_EQ( "color", decoded->getName() );
    EXPECT_NE( nullptr, decoded->imageView );
    EXPECT_NE( nullptr, decoded->imageView->image );
    EXPECT_NE( nullptr, decoded->sampler );
}
