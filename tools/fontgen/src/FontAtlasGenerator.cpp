/**
 * Copyright (c) 2013, Hugo Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "FontAtlasGenerator.hpp"

#include <cmath>

using namespace crimild;
using namespace crimild::tools::fontgen;

#define PADDING 30

FontAtlasGenerator::FontAtlasGenerator( std::string fontFace, unsigned int width, unsigned int height )
	: _width( width ),
	  _height( height ),
	  _buffer( width * height )
{
	if ( init() ) {
		loadFont( fontFace );
	}
}

FontAtlasGenerator::~FontAtlasGenerator( void )
{
	cleanup();
}

bool FontAtlasGenerator::execute( std::string output )
{
	_pixelSize = ( int )( std::min( _width - 2 * PADDING, _height - 7 * PADDING ) / std::ceil( std::sqrt( 256 ) ) );
	_cursorX = PADDING;
	_cursorY = PADDING;

	memset( &_buffer[ 0 ], 0, sizeof( unsigned char ) * _buffer.size() );

	_glyphs.open( output + ".txt" );
	if ( !_glyphs.is_open() ) {
		std::cout << "Cannot open file " << ( output + ".txt" ) << std::endl;
		return false;
	}

	int error = FT_Set_Pixel_Sizes( _face, _pixelSize, 0 );
	if ( error != 0 ) {
		std::cout << "Error setting char size: " << error << std::endl;
		return false;
	}

	// generate the list of characters to be processed
	for ( int i = 0; i < 255; i++ ) {
		pushChar( ( unsigned char ) ( 32 + i ) );
	}

	saveTexture( output + ".tga" );
	saveGlyphs( output + ".txt" );

	return true;
}

bool FontAtlasGenerator::init( void )
{
	int error = FT_Init_FreeType( &_library );
	if ( error ) {
		std::cerr << "Error opening library " << error << std::endl;
		return false;
	}

	return true;
}

bool FontAtlasGenerator::loadFont( std::string fontFace )
{
	int error = FT_New_Face( _library, fontFace.c_str(), 0, &_face );
	if ( error == FT_Err_Unknown_File_Format ) {
		std::cout << "Unknown file format for face " << fontFace << std::endl;
		return false;
	}
	else if ( error ) {
		std::cout << "Error loading face " << fontFace << std::endl;
		return false;
	}

	return true;
}

void FontAtlasGenerator::cleanup( void )
{
	FT_Done_Face( _face );
	FT_Done_FreeType( _library );
}

void FontAtlasGenerator::pushChar( unsigned char c )
{
	int error = FT_Load_Char( _face, c, FT_LOAD_RENDER );
	if ( error ) {
		return;
	}

	FT_Glyph glyph;
	error = FT_Get_Glyph( _face->glyph, &glyph );
	if ( error ) {
		return;
	}

	FT_Glyph_Metrics metrics = _face->glyph->metrics;

	if ( _cursorX + _face->glyph->bitmap.width > ( _width - 2 * PADDING ) ) {
		_cursorX = PADDING;
		_cursorY += _pixelSize + PADDING;
	}

	_glyphs 
		<< ( int ) c 
		<< " " << ( float ) ( metrics.width / 64 ) / ( float ) _pixelSize	
		<< " " << ( float ) ( metrics.height / 64 ) / ( float ) _pixelSize
		<< " " << ( float ) ( metrics.horiBearingX / 64 ) / ( float ) _pixelSize
		<< " " << ( float ) ( metrics.horiBearingY / 64 ) / ( float ) _pixelSize
		<< " " << ( float ) ( metrics.horiAdvance / 64 ) / ( float ) _pixelSize
		<< " " << ( float ) _cursorX / ( float ) _width
		<< " " << ( float ) _cursorY / ( float ) _height
		<< " " << ( float ) _face->glyph->bitmap.width / ( float ) _width
		<< " " << ( float ) _face->glyph->bitmap.rows / ( float ) _height
		<< "\n";

	writeBuffer( _face->glyph, _cursorX, _cursorY );

	_cursorX += _pixelSize;
}

void FontAtlasGenerator::writeBuffer( FT_GlyphSlot &slot, int x, int y )
{
	FT_Int x_max = x + slot->bitmap.width;
	FT_Int y_max = y + slot->bitmap.rows;

	for ( FT_Int i = x, p = 0; i < x_max; i++, p++ ) {
		for ( FT_Int j = y, q = 0; j < y_max; j++, q++ ) {
			if ( i < 0 || j < 0 || i >= _width || j >= _height ) {
				continue;
			}

			int bufferIdx =  ( _height - j ) * _width + i;
			if ( bufferIdx >= _width * _height ) {
				continue;
			}

			int slotIdx = q * slot->bitmap.width + p;
			if ( slotIdx >= slot->bitmap.width * slot->bitmap.rows ) {
				continue;
			}

			_buffer[ bufferIdx ] |= slot->bitmap.buffer[ slotIdx ];
		}
	}
}

void FontAtlasGenerator::saveTexture( std::string fileName )
{
	unsigned char identsize = 0;
    unsigned char colorMapType = 0;
    unsigned char imageType = 2;	// 2 - rgb, 3 - greyscale
    unsigned short colorMapStart = 0;
    unsigned short colorMapLength = 0;
    unsigned char colorMapBits = 0;
    unsigned short xstart = 0;
    unsigned short ystart = 0;
    unsigned short width = _width;
    unsigned short height = _height;
    unsigned char bits = 24;
    unsigned char descriptor = 0;

	FILE *out = fopen( fileName.c_str(), "wb" );

	fwrite( &identsize, sizeof( unsigned char ), 1, out );
	fwrite( &colorMapType, sizeof( unsigned char ), 1, out );
	fwrite( &imageType, sizeof( unsigned char ), 1, out );
	fwrite( &colorMapStart, sizeof( unsigned short ), 1, out );
	fwrite( &colorMapLength, sizeof( unsigned short ), 1, out );
	fwrite( &colorMapBits, sizeof( unsigned char ), 1, out );
	fwrite( &xstart, sizeof( unsigned short ), 1, out );
	fwrite( &ystart, sizeof( unsigned short ), 1, out );
	fwrite( &width, sizeof( unsigned short ), 1, out );
	fwrite( &height, sizeof( unsigned short ), 1, out );
	fwrite( &bits, sizeof( unsigned char ), 1, out );
	fwrite( &descriptor, sizeof( unsigned char ), 1, out );

	for ( int i = 0; i < _width * _height; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			fwrite( &_buffer[ i ], 1, sizeof( unsigned char ), out );
		}
	}

	fclose( out );
}

void FontAtlasGenerator::saveGlyphs( std::string fileName )
{

}

