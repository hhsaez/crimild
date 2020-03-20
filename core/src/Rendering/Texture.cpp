/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#include "Texture.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

#include "Foundation/Log.hpp"

CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::Texture )

using namespace crimild;

SharedPointer< Texture > Texture::ONE = crimild::alloc< Texture >(
	crimild::alloc< Image >( 1, 1, 4, containers::ByteArray { 0xFF, 0xFF, 0xFF, 0xFF }, Image::PixelFormat::RGBA )
);

SharedPointer< Texture > Texture::ZERO = crimild::alloc< Texture >(
	crimild::alloc< Image >( 1, 1, 4, containers::ByteArray { 0x00, 0x00, 0x00, 0x00 }, Image::PixelFormat::RGBA )
);

SharedPointer< Texture > Texture::CUBE_ONE = [] {
    return crimild::alloc< Texture >(
        containers::Array< SharedPointer< Image >> {
            crimild::alloc< Image >( 1, 1, 4, containers::ByteArray { 0xFF, 0xFF, 0xFF, 0xFF }, Image::PixelFormat::RGBA ),
            crimild::alloc< Image >( 1, 1, 4, containers::ByteArray { 0xFF, 0xFF, 0xFF, 0xFF }, Image::PixelFormat::RGBA ),
            crimild::alloc< Image >( 1, 1, 4, containers::ByteArray { 0xFF, 0xFF, 0xFF, 0xFF }, Image::PixelFormat::RGBA ),
            crimild::alloc< Image >( 1, 1, 4, containers::ByteArray { 0xFF, 0xFF, 0xFF, 0xFF }, Image::PixelFormat::RGBA ),
            crimild::alloc< Image >( 1, 1, 4, containers::ByteArray { 0xFF, 0xFF, 0xFF, 0xFF }, Image::PixelFormat::RGBA ),
            crimild::alloc< Image >( 1, 1, 4, containers::ByteArray { 0xFF, 0xFF, 0xFF, 0xFF }, Image::PixelFormat::RGBA )
        }
    );
}();

SharedPointer< Texture > createCheckerBoardTexture( crimild::Int32 size )
{
    crimild::Byte colors[] = {
        0x00, 0x00, 0x00, 0xFF, // black
        0xFF, 0xFF, 0xFF, 0xFF, // white
    };

    crimild::Int32 width = size;
    crimild::Int32 height = size;
    crimild::Int32 bpp = 4;

    auto data = containers::ByteArray( width * height * bpp );
    for ( int y = 0; y < width; y++ ) {
        for ( int x = 0; x < width; x++ ) {
            auto colorIdx = ( y % 2 + x % 2 ) % 2;
            for ( int i = 0; i < bpp; i++ ) {
                data[ y * width * bpp + x * bpp + i ] = colors[ colorIdx * bpp + i ];
            }
        }
    }

    auto image = crimild::alloc< Image >( width, width, bpp, data, Image::PixelFormat::RGBA );
    auto texture = crimild::alloc< Texture >( image );
    texture->setMagFilter( Texture::Filter::NEAREST );
    texture->setMinFilter( Texture::Filter::NEAREST );
    return texture;
}

SharedPointer< Texture > Texture::CHECKERBOARD = createCheckerBoardTexture( 2 );
SharedPointer< Texture > Texture::CHECKERBOARD_4 = createCheckerBoardTexture( 4 );
SharedPointer< Texture > Texture::CHECKERBOARD_8 = createCheckerBoardTexture( 8 );
SharedPointer< Texture > Texture::CHECKERBOARD_16 = createCheckerBoardTexture( 16 );
SharedPointer< Texture > Texture::CHECKERBOARD_32 = createCheckerBoardTexture( 32 );
SharedPointer< Texture > Texture::CHECKERBOARD_64 = createCheckerBoardTexture( 64 );
SharedPointer< Texture > Texture::CHECKERBOARD_128 = createCheckerBoardTexture( 128 );
SharedPointer< Texture > Texture::CHECKERBOARD_256 = createCheckerBoardTexture( 256 );
SharedPointer< Texture > Texture::CHECKERBOARD_512 = createCheckerBoardTexture( 512 );

SharedPointer< Texture > Texture::INVALID = crimild::alloc< Texture >(
    crimild::alloc< Image >( 1, 1, 4, containers::ByteArray { 0xFF, 0x00, 0xFF, 0xFF }, Image::PixelFormat::RGBA )
);

Texture::Texture( std::string name )
    : NamedObject( name )
{

}

Texture::Texture( SharedPointer< Image > const &image, std::string name )
	: NamedObject( name ),
	  _images( { image } )
{

}

Texture::Texture( Texture::ImageArray const &images )
	: NamedObject( "CubeMap" ),
	  _target( Texture::Target::CUBE_MAP ),
	  _images( images )
{

}

Texture::~Texture( void )
{
    unload();
}

void Texture::encode( coding::Encoder &encoder )
{
	Codable::encode( encoder );

	encoder.encode( "target", _target );
	encoder.encode( "images", _images );
}

void Texture::decode( coding::Decoder &decoder )
{
	Codable::decode( decoder );

    _images.clear();

	decoder.decode( "target", _target );
	decoder.decode( "images", _images );

    SharedPointer< Image > image;
    decoder.decode( "image", image );
    if ( image != nullptr ) {
        _images.add( image );
    }
}

