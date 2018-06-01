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
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

#include <cstring>

CRIMILD_REGISTER_STREAM_OBJECT_BUILDER( crimild::Image )

using namespace crimild;

Image::Image( void )
{
	_width = 0;
	_height = 0;
	_bpp = 0;
}

Image::Image( int width, int height, int bpp, const unsigned char *data, PixelFormat format )
{
	setData( width, height, bpp, data, format );
}

Image::~Image( void )
{
	unload();
}

void Image::setData( int width, int height, int bpp, const unsigned char *data, PixelFormat format )
{
	_width = width;
	_height = height;
	_bpp = bpp;
    _pixelFormat = format;

	int size = _width * _height * _bpp;
	if ( size > 0 ) {
        if ( _data.size() != size ) _data.resize( size );

		if ( data ) {
			memcpy( &_data[ 0 ], data, size * sizeof( unsigned char ) );
		}
		else {
			memset( &_data[ 0 ], 0, size * sizeof( unsigned char ) );
		}
	}
    else {
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

bool Image::registerInStream( Stream &s )
{
	if ( !StreamObject::registerInStream( s ) ) {
		return false;
	}

	return true;
}

void Image::save( Stream &s )
{
	StreamObject::save( s );

	s.write( _width );
	s.write( _height );
	s.write( _bpp );
	s.writeRawBytes( &_data[ 0 ], _width * _height * _bpp * sizeof( unsigned char ) );
}

void Image::load( Stream &s )
{
	StreamObject::load( s );

	s.read( _width );
	s.read( _height );
	s.read( _bpp );
	_data.resize( _width * _height * _bpp );
	s.readRawBytes( &_data[ 0 ], _data.size() * sizeof( unsigned char ) );
}

