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

#include "SDFGenerator.hpp"

using namespace crimild;
using namespace crimild::tools::fontgen;

class Grid {
public:
	struct Point {
		int dx;
		int dy;

		int distSqr( void ) const { return dx * dx + dy * dy; }
	};

	Grid( unsigned int width, unsigned int height )
		: _width( width ),
		  _height( height ),
		  _data( _width * _height )
	{

	}

	virtual ~Grid( void )
	{

	}

	void put( int x, int y, const Point &point )
	{
		_data[ y * _width + x ] = point;
	}

	Point &get( int x, int y )
	{
		return _data[ y * _width + x ];
	}

	void generateSDF( void )
	{
		// pass 1
		for ( int y = 0; y < _height; y++ ) {
			for ( int x = 0; x < _width; x++ ) {
				Point p = get( x, y );
				compare( p, x, y, -1, 0 );
				compare( p, x, y, 0, -1 );
				compare( p, x, y, -1, -1 );
				compare( p, x, y, 1, -1 );
				put( x, y, p );
			}

			for ( int x = _width - 1; x >= 0; x-- ) {
				Point p = get( x, y );
				compare( p, x, y, 1, 0 );
				put( x, y, p );
			}
		}

		// pass 2
		for ( int y = _height - 1; y >= 0; y-- ) {
			for ( int x = _width - 1; x >= 0; x-- ) {
				Point p = get( x, y );
				compare( p, x, y, 1, 0 );
				compare( p, x, y, 0, 1 );
				compare( p, x, y, -1, 0 );
				compare( p, x, y, 1, 1 );
				put( x, y, p );
			}

			for ( int x = 0; x < _width; x++ ) {
				Point p = get( x, y );
				compare( p, x, y, -1, 0 );
				put( x, y, p );
			}
		}
	}

private:
	void compare( Point &p, int x, int y, int offsetX, int offsetY )
	{
		if ( x < 0 || x + offsetX >= _width ) {
			return;
		}

		if ( y < 0 || y + offsetY >= _height ) {
			return;
		}
		
		Point other = get( x + offsetX, y + offsetY );
		other.dx += offsetX;
		other.dy += offsetY;

		if ( other.distSqr() < p.distSqr() ) {
			p = other;
		}
	}

	unsigned int _width;
	unsigned int _height;
	std::vector< Point > _data;
};

SDFGenerator::SDFGenerator( void )
{

}

SDFGenerator::~SDFGenerator( void )
{

}

void SDFGenerator::execute( std::string source )
{
	ImageTGA input( source );

	std::cout << input.getWidth() << "x" << input.getHeight() << "x" << input.getBpp() << std::endl;

	Grid grid1( input.getWidth(), input.getHeight() );
	Grid grid2( input.getWidth(), input.getHeight() );

	Grid::Point inside;
	inside.dx = 0;
	inside.dy = 0;

	Grid::Point outside;
	outside.dx = 999;
	outside.dy = 999;

	std::vector< unsigned char > sdf( input.getWidth() * input.getHeight() );
	for ( int y = 0; y < input.getHeight(); y++ ) {
		for ( int x = 0; x < input.getWidth(); x++ ) {
			unsigned char color = input.getData()[ y * ( input.getWidth() * input.getBpp() ) + x * input.getBpp() ];
			grid1.put( x, y, color < 128 ? inside : outside );
			grid2.put( x, y, color < 128 ? outside : inside );
		}
	}

	grid1.generateSDF();
	grid2.generateSDF();

	std::vector< unsigned char > data( input.getWidth() * input.getHeight() );
	for ( int y = 0; y < input.getHeight(); y++ ) {
		for ( int x = 0; x < input.getWidth(); x++ ) {
			// Calculate the actual distance from the dx/dy
			int dist1 = sqrt( grid1.get( x, y ).distSqr() );
			int dist2 = sqrt( grid2.get( x, y ).distSqr() );
			int dist = dist1 - dist2;

			// Clamp and scale it, just for display purposes.
			int c = dist * 3 + 128;
			if ( c < 0 ) c = 0;
			if ( c > 255 ) c = 255;

			data[ y * input.getWidth() + x ] = c;
		}
	}

	std::string outputPath = source.substr( 0, source.find_last_of( "." ) ) + "_sdf.tga";
	ImageTGA output;
	output.setData( input.getWidth(), input.getHeight(), 1, &data[ 0 ] );
	output.save( outputPath );

#ifdef __APPLE__
	system( ( std::string( "open \"" ) + source + "\"" ).c_str() );
	system( ( std::string( "open \"" ) + outputPath + "\"" ).c_str() );
#endif
}

