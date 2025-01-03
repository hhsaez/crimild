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

#include "Crimild_Coding.hpp"
#include "Crimild_Mathematics.hpp"

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
    image->setBufferView(
        crimild::alloc< BufferView >(
            BufferView::Target::IMAGE,
            crimild::alloc< Buffer >(
                ByteArray {
                    0xFF,
                    0xFF,
                    0xFF,
                    0xFF,
                }
            )
        )
    );
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
    image->setBufferView(
        crimild::alloc< BufferView >(
            BufferView::Target::IMAGE,
            crimild::alloc< Buffer >(
                ByteArray {
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                }
            )
        )
    );
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
    image->setBufferView(
        crimild::alloc< BufferView >(
            BufferView::Target::IMAGE,
            crimild::alloc< Buffer >(
                ByteArray {
                    0xFF,
                    0x00,
                    0xFF,
                    0xFF,
                }
            )
        )
    );
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

    auto image = crimild::alloc< Image >();
    image->extent = {
        .width = crimild::Real32( width ),
        .height = crimild::Real32( height ),
        .depth = 1,
    };
    image->format = Format::R8G8B8A8_UNORM;
    image->setBufferView(
        crimild::alloc< BufferView >(
            BufferView::Target::IMAGE,
            crimild::alloc< Buffer >(
                [ & ] {
                    auto data = ByteArray( width * height * bpp );
                    for ( int y = 0; y < width; y++ ) {
                        for ( int x = 0; x < width; x++ ) {
                            auto colorIdx = ( y % 2 + x % 2 ) % 2;
                            for ( int i = 0; i < bpp; i++ ) {
                                data[ y * width * bpp + x * bpp + i ] = colors[ colorIdx * bpp + i ];
                            }
                        }
                    }
                    return data;
                }()
            )
        )
    );
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
    // TODO: use float format?
    image->format = Format::R8G8B8A8_UNORM;
    image->setBufferView(
        crimild::alloc< BufferView >(
            BufferView::Target::IMAGE,
            crimild::alloc< Buffer >(
                [ & ] {
                    auto data = ByteArray( size * size * 4 );
                    for ( int i = 0; i < size; ++i ) {
                        for ( int j = 0; j < size; ++j ) {
                            auto idx = ( i * size + j ) * 4;

                            const auto V0 = Vector2f { Real( i ), Real( j ) };
                            constexpr auto V1 = Vector2f { 223.35734, 550.56781 };
                            const auto V2 = V0 + V1;
                            const auto V3 = V2 * V2;
                            const Real xy = V3.x * V3.y;

                            Vector4f color {
                                Numericf::fract( xy * 0.00000012 ),
                                Numericf::fract( xy * 0.00000543 ),
                                Numericf::fract( xy * 0.00000192 ),
                                Numericf::fract( xy * 0.00000423 ),
                            };

                            data[ idx + 0 ] = UInt8( color[ 0 ] * 255 );
                            data[ idx + 1 ] = UInt8( color[ 1 ] * 255 );
                            data[ idx + 2 ] = UInt8( color[ 2 ] * 255 );
                            data[ idx + 3 ] = UInt8( color[ 3 ] * 255 );
                        }
                    }
                    return data;
                }()
            )
        )
    );
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

    encoder.encodeEnum( "format", format );
    encoder.encode( "type", Int32( type ) );
    encoder.encode( "extent.width", extent.width );
    encoder.encode( "extent.height", extent.height );
    encoder.encode( "extent.depth", extent.depth );
    encoder.encodeEnum( "extent.scaling_mode", extent.scalingMode );
    encoder.encode( "bufferView", m_bufferView );
    encoder.encode( "layerCount", m_layerCount );
    encoder.encode( "mipLevels", m_mipLevels );
}

void Image::decode( coding::Decoder &decoder )
{
    Codable::decode( decoder );

    decoder.decodeEnum( "format", format );

    Int32 type;
    decoder.decode( "type", type );
    this->type = Type( type );

    decoder.decode( "extent.width", extent.width );
    decoder.decode( "extent.height", extent.height );
    decoder.decode( "extent.depth", extent.depth );
    decoder.decodeEnum( "extent.scaling_mode", extent.scalingMode );
    decoder.decode( "bufferView", m_bufferView );
    decoder.decode( "layerCount", m_layerCount );
    decoder.decode( "mipLevels", m_mipLevels );
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
