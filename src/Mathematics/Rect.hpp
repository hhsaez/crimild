/**
 * Crimild Engine is an open source scene graph based engine which purpose
 * is to fulfill the high-performance requirements of typical multi-platform
 * two and tridimensional multimedia projects, like games, simulations and
 * virtual reality.
 *
 * Copyright (C) 2006-2013 Hernan Saez - hhsaez@gmail.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef CRIMILD_MATHEMATICS_RECT_
#define CRIMILD_MATHEMATICS_RECT_

namespace Crimild {

	template< typename PRECISION >
	class Rect {
	public:
		Rect( void )
		{
		}

		Rect( PRECISION x, PRECISION y, PRECISION width, PRECISION height )
		{
			_data[ 0 ] = x;
			_data[ 1 ] = y;
			_data[ 2 ] = width;
			_data[ 3 ] = height;
		}

		Rect( const Rect &rect )
		{
			*this = rect;
		}

		~Rect( void )
		{
		}

		Rect &operator=( const Rect &rect )
		{
			memcpy( _data, rect._data, 4 * sizeof( PRECISION ) );
			return *this;
		}

		PRECISION &x( void ) { return _data[ 0 ]; }
		PRECISION &y( void ) { return _data[ 1 ]; }
		PRECISION &width( void ) { return _data[ 2 ]; }
		PRECISION &height( void ) { return _data[ 3 ]; }

		PRECISION getX( void ) const { return _data[ 0 ]; }
		PRECISION getY( void ) const { return _data[ 1 ]; }
		PRECISION getWidth( void ) const { return _data[ 2 ]; }
		PRECISION getHeight( void ) const { return _data[ 3 ]; }

	private:
		PRECISION _data[ 4 ];
	};

	typedef Rect< int > Recti;
	typedef Rect< float > Rectf;

}

#endif

