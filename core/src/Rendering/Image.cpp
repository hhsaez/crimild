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

#include "Image.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"

#include <cstring>

using namespace crimild;

SharedPointer< Image > Image::ONE = [] {
    auto image = crimild::alloc< Image >();
    image->extent = {
        .width = 1,
        .height = 1,
        .depth = 1,
    };
    image->format = Format::R8G8B8A8_UNORM;
    image->data = {
        0xFF,
        0xFF,
        0xFF,
        0xFF
    };
    return image;
}();

SharedPointer< Image > Image::ZERO = [] {
    auto image = crimild::alloc< Image >();
    image->extent = {
        .width = 1,
        .height = 1,
        .depth = 1,
    };
    image->format = Format::R8G8B8A8_UNORM;
    image->data = {
        0x00,
        0x00,
        0x00,
        0x00,
    };
    return image;
}();

SharedPointer< Image > Image::INVALID = [] {
    auto image = crimild::alloc< Image >();
    image->extent = {
        .width = 1,
        .height = 1,
        .depth = 1,
    };
    image->format = Format::R8G8B8A8_UNORM;
    image->data = {
        0xFF,
        0x00,
        0xFF,
        0xFF
    };
    return image;
}();

SharedPointer< Image > Image::CUBE_ONE = [] {
    //    return crimild::alloc< Image >(
    //        containers::Array< SharedPointer< Image >> {
    //        	ONE,
    //        	ONE,
    //        	ONE,
    //        	ONE,
    //        	ONE,
    //        	ONE,
    //        }
    //    );
    return nullptr;
}();

SharedPointer< Image > createCheckerBoardImage( crimild::Int32 size )
{
    crimild::Byte colors[] = {
        0x00,
        0x00,
        0x00,
        0xFF, // black
        0xFF,
        0xFF,
        0xFF,
        0xFF, // white
    };

    crimild::Int32 width = size;
    crimild::Int32 height = size;
    crimild::Int32 bpp = 4;

    auto data = ByteArray( width * height * bpp );
    for ( int y = 0; y < width; y++ ) {
        for ( int x = 0; x < width; x++ ) {
            auto colorIdx = ( y % 2 + x % 2 ) % 2;
            for ( int i = 0; i < bpp; i++ ) {
                data[ y * width * bpp + x * bpp + i ] = colors[ colorIdx * bpp + i ];
            }
        }
    }

    auto image = crimild::alloc< Image >();
    image->extent = {
        .width = crimild::Real32( width ),
        .height = crimild::Real32( height ),
        .depth = 1,
    };
    image->format = Format::R8G8B8A8_UNORM;
    image->data = data;
    return image;
}

SharedPointer< Image > Image::CHECKERBOARD = createCheckerBoardImage( 2 );
SharedPointer< Image > Image::CHECKERBOARD_4 = createCheckerBoardImage( 4 );
SharedPointer< Image > Image::CHECKERBOARD_8 = createCheckerBoardImage( 8 );
SharedPointer< Image > Image::CHECKERBOARD_16 = createCheckerBoardImage( 16 );
SharedPointer< Image > Image::CHECKERBOARD_32 = createCheckerBoardImage( 32 );
SharedPointer< Image > Image::CHECKERBOARD_64 = createCheckerBoardImage( 64 );
SharedPointer< Image > Image::CHECKERBOARD_128 = createCheckerBoardImage( 128 );
SharedPointer< Image > Image::CHECKERBOARD_256 = createCheckerBoardImage( 256 );
SharedPointer< Image > Image::CHECKERBOARD_512 = createCheckerBoardImage( 512 );

SharedPointer< Image > Image::fromRGBANoise( UInt32 size ) noexcept
{
    auto image = crimild::alloc< Image >();
    image->extent = {
        .width = Real32( size ),
        .height = Real32( size ),
        .depth = 1,
    };
    image->format = Format::R8G8B8A8_UNORM;
    image->data = [ & ] {
        auto data = ByteArray( size * size * 4 );
        for ( int i = 0; i < size; ++i ) {
            for ( int j = 0; j < size; ++j ) {
                auto idx = ( i * size + j ) * 4;

                Vector2f uv( i, j );
                uv += Vector2f( 223.35734, 550.56781 );
                uv.times( uv );
                float xy = uv.x() * uv.y();

                Vector4f color(
                    Numericf::fract( xy * 0.00000012 ),
                    Numericf::fract( xy * 0.00000543 ),
                    Numericf::fract( xy * 0.00000192 ),
                    Numericf::fract( xy * 0.00000423 ) );

                data[ idx + 0 ] = UInt8( color[ 0 ] * 255 );
                data[ idx + 1 ] = UInt8( color[ 1 ] * 255 );
                data[ idx + 2 ] = UInt8( color[ 2 ] * 255 );
                data[ idx + 3 ] = UInt8( color[ 3 ] * 255 );
            }
        }
        return data;
    }();
    return image;
}

Image::Image( void )
{
    _width = 0;
    _height = 0;
    _bpp = 0;
}

Image::Image( int width, int height, int bpp, PixelFormat format, PixelType pixelType )
    : _width( width ),
      _height( height ),
      _bpp( bpp ),
      _pixelFormat( format ),
      _pixelType( pixelType )
{
}

Image::Image( int width, int height, int bpp, const unsigned char *data, PixelFormat format )
{
    setData( width, height, bpp, data, format );
}

Image::Image( int width, int height, int bpp, const ByteArray &data, PixelFormat format )
{
    setData( width, height, bpp, data.size() > 0 ? &data[ 0 ] : nullptr, format );
}

Image::~Image( void )
{
    unload();
}

void Image::setData( int width, int height, int bpp, const unsigned char *data, PixelFormat format, PixelType pixelType )
{
    _width = width;
    _height = height;
    _bpp = bpp;
    _pixelFormat = format;
    _pixelType = pixelType;

    int size = _width * _height * _bpp;
    if ( size > 0 ) {
        if ( _data.size() != size )
            _data.resize( size );

        if ( data ) {
            memcpy( &_data[ 0 ], data, size * sizeof( unsigned char ) );
        } else {
            memset( &_data[ 0 ], 0, size * sizeof( unsigned char ) );
        }
    } else {
        _data.resize( 0 );
    }
}

void Image::load( void )
{
}

void Image::unload( void )
{
    _width = 0;
    _height = 0;
    _bpp = 0;
    _data.resize( 0 );
}

void Image::encode( coding::Encoder &encoder )
{
    Codable::encode( encoder );

    encoder.encode( "width", _width );
    encoder.encode( "height", _height );
    encoder.encode( "bpp", _bpp );
    encoder.encode( "data", _data );
}

void Image::decode( coding::Decoder &decoder )
{
    Codable::decode( decoder );

    decoder.decode( "width", _width );
    decoder.decode( "height", _height );
    decoder.decode( "bpp", _bpp );
    decoder.decode( "data", _data );
}

void Image::setMipLevels( crimild::UInt32 mipLevels ) noexcept
{
    m_mipLevels = mipLevels;
}

crimild::UInt32 Image::getMipLevels( void ) const noexcept
{
    if ( type == Image::Type::IMAGE_2D_CUBEMAP ) {
        // no mipmapping for cubemaps
        return 1;
    }

    if ( m_mipLevels > 0 ) {
        return m_mipLevels;
    }

    // Each mipmap level is half the size of the previous one
    // At the very least, we'll have 1 mip level (the original size)
    return 1 + static_cast< crimild::UInt32 >( Numericf::floor( Numericf::log2( Numericf::max( extent.width, extent.height ) ) ) );
}
