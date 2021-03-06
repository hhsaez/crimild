/*
 * Copyright (c) 2013-2018, Hernan Saez
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

#include "RenderTarget.hpp"
#include "Texture.hpp"

using namespace crimild;

constexpr const char *RenderTarget::RENDER_TARGET_NAME_COLOR;
constexpr const char *RenderTarget::RENDER_TARGET_NAME_DEPTH;

RenderTarget::RenderTarget( RenderTarget::Type type, crimild::Int8 output, int width, int height )
    : RenderTarget( type, output, width, height, false )
{

}

RenderTarget::RenderTarget( RenderTarget::Type type, crimild::Int8 output, int width, int height, bool floatTextureHint )
{
    _type = type;
    _output = output;
    _width = width;
    _height = height;
    _useFloatTexture = floatTextureHint;

    auto pixelFormat = Image::PixelFormat::RGBA;
    auto pixelType = floatTextureHint ? Image::PixelType::FLOAT : Image::PixelType::UNSIGNED_BYTE;
    auto channels = 4;

    switch ( type ) {
    case RenderTarget::Type::COLOR_RGB:
        pixelFormat = Image::PixelFormat::RGB;
        channels = 3;
        break;

    case RenderTarget::Type::COLOR_RGBA:
        pixelFormat = Image::PixelFormat::RGBA;
        channels = 4;
        break;

    case RenderTarget::Type::DEPTH_16:
        pixelFormat = Image::PixelFormat::DEPTH_16;
        channels = 1;
        break;

    case RenderTarget::Type::DEPTH_24:
        pixelFormat = Image::PixelFormat::DEPTH_32;
        channels = 1;
        break;

    case RenderTarget::Type::DEPTH_32:
        pixelFormat = Image::PixelFormat::DEPTH_32;
        channels = 1;
        break;
    }

    auto image = crimild::alloc< Image >( width, height, channels, pixelFormat, pixelType );
    _texture = crimild::alloc< Texture >( image );
    _texture->setWrapMode( Texture::WrapMode::CLAMP_TO_EDGE );
    _texture->setMagFilter( Texture::Filter::NEAREST );
    _texture->setMinFilter( Texture::Filter::NEAREST );
}

RenderTarget::~RenderTarget( void )
{
    
}

