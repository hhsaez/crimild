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

#include "ImageTGA.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"
#include "Simulation/FileSystem.hpp"
#include "Exceptions/FileNotFoundException.hpp"
#include "Exceptions/InvalidFileFormatException.hpp"

#include <vector>
#include <algorithm>

#define TGA_RGB 2
#define TGA_A 3
#define TGA_RLE 10

using namespace crimild;

ImageTGA::ImageTGA( void )
{
    
}

ImageTGA::ImageTGA( std::string filePath )
	: _filePath( filePath )
{
	load();
}

ImageTGA::~ImageTGA( void )
{

}

void ImageTGA::load( void )
{
    FILE *file = NULL;
    if ( ( file = fopen( _filePath.c_str(), "rb" ) ) == NULL ) {
        throw FileNotFoundException( _filePath );
    }

    TGAHeader header;

    // Read components one-by-one to avoid alignment issues
    fread( &header.idLength, sizeof( crimild::UInt8 ), 1, file );
    fread( &header.colorMapType, sizeof( crimild::UInt8 ), 1, file );
    fread( &header.imageType, sizeof( crimild::UInt8 ), 1, file );
    fread( &header.colorMapStart, sizeof( crimild::UInt16 ), 1, file );
    fread( &header.colorMapLength, sizeof( crimild::UInt16 ), 1, file );
    fread( &header.colorMapBits, sizeof( crimild::UInt8 ), 1, file );
    fread( &header.xStart, sizeof( crimild::UInt16 ), 1, file );
    fread( &header.yStart, sizeof( crimild::UInt16 ), 1, file );
    fread( &header.width, sizeof( crimild::UInt16 ), 1, file );
    fread( &header.height, sizeof( crimild::UInt16 ), 1, file );
    fread( &header.bits, sizeof( crimild::UInt8 ), 1, file );
    fread( &header.descriptor, sizeof( crimild::UInt8 ), 1, file );

    auto width = header.width;
    auto height = header.height;
    auto bits = header.bits;
    int bpp = 0;
    std::vector< crimild::UInt8 > data;

    fseek( file, header.idLength, SEEK_CUR );

    if( header.imageType != TGA_RLE ) {
        if( bits == 24 || bits == 32 ) {
            bpp = bits / 8;
            auto stride = bpp * width;
			data.resize( stride * height );

            for( unsigned int y = 0; y < height; y++ ) {
                auto dataIdx = stride * y;
                crimild::UInt8 *line = &data[ dataIdx ];

                fread( line, stride, 1, file );

                // Swap the B and R values since TGA
                // files are stored as BGR instead of RGB
                for( int i = 0; i < stride; i += bpp ) {
                    int temp = line[ i ];
                    line[ i ] = line[ i + 2 ];
                    line[ i + 2 ] = temp;
                }
            }
        }
        else if ( bits == 8 ) {
        	bpp = 1;
			data.resize( width * height );
        	fread( &data[ 0 ], width * height, 1, file );
        }
        else {
        	// invalid format
            throw InvalidFileFormatException( _filePath );
        }
    }
    else
    {
        crimild::UInt8 rleID = 0;
        int colorsRead = 0;
        bpp = bits / 8;
        auto stride = bpp * width;

		data.resize( stride * height );
		std::vector< crimild::UInt8 > colors( bpp );

        int i = 0;
        while ( i < static_cast< int >( width * height ) ) {
            fread( &rleID, sizeof( crimild::UInt8 ), 1, file );

            if ( rleID < 128 ) {
                rleID++;

                while ( rleID ) {
                    fread( &colors[ 0 ], sizeof( crimild::UInt8 ) * bpp, 1, file );

                    data[ colorsRead + 0 ] = colors[ 2 ];
                    data[ colorsRead + 1 ] = colors[ 1 ];
                    data[ colorsRead + 2 ] = colors[ 0 ];

                    if ( bits == 32 ) {
                        data[ colorsRead + 3 ] = colors[ 3 ];
                    }

                    i++;
                    rleID--;
                    colorsRead += bpp;
                }
            }
            else {
                rleID -= 127;

                fread( &colors[ 0 ], sizeof( crimild::UInt8 ) * bpp, 1, file );

                while ( rleID ) {
                    data[ colorsRead + 0 ] = colors[ 2 ];
                    data[ colorsRead + 1 ] = colors[ 1 ];
                    data[ colorsRead + 2 ] = colors[ 0 ];

                    if ( bits == 32 ) {
                        data[ colorsRead + 3 ] = colors[ 3 ];
                    }

                    i++;
                    rleID--;
                    colorsRead += bpp;
                }
            }
        }
    }

    fclose( file );

    if ( header.yStart > 0 ) {
        // Force image's vertical origin to be bottom left
        auto halfHeight = height / 2;
        auto lineLength = width * bpp;
        for ( crimild::UInt16 y = 0; y < halfHeight; y++ ) {
            for ( crimild::UInt16 x = 0; x < lineLength; x++ ) {
                auto idx0 = y * lineLength + x;
                auto idx1 = ( height - y - 1 ) * lineLength + x;
                std::swap( data[ idx0 ], data[ idx1 ] );
            }
        }
    }

    setData( width, height, bpp, &data[ 0 ], bpp == 3 ? PixelFormat::RGB : PixelFormat::RGBA );
}

void ImageTGA::saveToFile( const std::string &path ) const
{
    TGAHeader header;
    header.idLength = sizeof( header );
    header.colorMapType = 0;
    header.imageType = getBpp() == 1 ? 3 : 2;
    header.colorMapStart = 0;
    header.colorMapLength = 0;
    header.colorMapBits = 0;
    header.xStart = 0;
    header.yStart = 0;
    header.width = getWidth();
    header.height = getHeight();
    header.bits = getBpp() * 8;
    header.descriptor = 0;

    FILE *out = fopen( path.c_str(), "wb" );

    fwrite( &header.idLength, sizeof( crimild::UInt8 ), 1, out );
    fwrite( &header.colorMapType, sizeof( crimild::UInt8 ), 1, out );
    fwrite( &header.imageType, sizeof( crimild::UInt8 ), 1, out );
    fwrite( &header.colorMapStart, sizeof( crimild::UInt16 ), 1, out );
    fwrite( &header.colorMapLength, sizeof( crimild::UInt16 ), 1, out );
    fwrite( &header.colorMapBits, sizeof( crimild::UInt8 ), 1, out );
    fwrite( &header.xStart, sizeof( crimild::UInt16 ), 1, out );
    fwrite( &header.yStart, sizeof( crimild::UInt16 ), 1, out );
    fwrite( &header.width, sizeof( crimild::UInt16 ), 1, out );
    fwrite( &header.height, sizeof( crimild::UInt16 ), 1, out );
    fwrite( &header.bits, sizeof( crimild::UInt8 ), 1, out );
    fwrite( &header.descriptor, sizeof( crimild::UInt8 ), 1, out );

    fwrite( &getData()[ 0 ], getWidth() * getHeight() * getBpp(), sizeof( crimild::UInt8 ), out );

    fclose( out );
}

void ImageTGA::encode( coding::Encoder &encoder )
{
    Image::encode( encoder );
}

void ImageTGA::decode( coding::Decoder &decoder )
{
    Image::decode( decoder );

    decoder.decode( "imageFileName", _filePath );
    if ( _filePath.length() > 0 ) {
        _filePath = FileSystem::getInstance().pathForResource( _filePath );
        load();
    }
}

