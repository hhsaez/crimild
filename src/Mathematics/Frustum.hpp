/**
 * Crimild Engine is an open source scene graph based engine which purpose
 * is to fulfil the high-performance requirements of typical multi-platform
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

#ifndef CRIMILD_MATHEMATICS_FRUSTUM_
#define CRIMILD_MATHEMATICS_FRUSTUM_

#include "Numeric.hpp"

namespace Crimild {

	template< typename PRECISION >
	class Frustum {
	public:
		Frustum( void )
		{
		}

		Frustum( PRECISION rMin, PRECISION rMax, PRECISION uMin, PRECISION uMax, PRECISION dMin, PRECISION dMax )
		{
			_data[ 0 ] = rMin;
			_data[ 1 ] = rMax;
			_data[ 2 ] = uMin;
			_data[ 3 ] = uMax;
			_data[ 4 ] = dMin;
			_data[ 5 ] = dMax;
		}

		Frustum( PRECISION fov, PRECISION aspect, PRECISION near, PRECISION far )
		{
			PRECISION invE = static_cast< PRECISION >( tan( fov * Numeric< PRECISION >::DEG_TO_RAD / 2.0 ) );
			PRECISION temp = near * invE;

			_data[ 0 ] = -temp;
			_data[ 1 ] = +temp;
			_data[ 2 ] = ( -1.0 / aspect ) * temp;
			_data[ 3 ] = ( +1.0 / aspect ) * temp;
			_data[ 4 ] = near;
			_data[ 5 ] = far;
		}

		Frustum( const Frustum &frustum )
		{
			memcpy( _data, frustum._data, 6 * sizeof( PRECISION ) );
		}

		~Frustum( void )
		{
		}

		bool operator==( const Frustum &frustum ) const
		{
			return memcmp( _data, frustum._data, 6 * sizeof( PRECISION ) ) == 0;
		}

		bool operator!=( const Frustum &frustum ) const
		{
			return memcmp( _data, frustum._data, 6 * sizeof( PRECISION ) ) != 0;
		}

		PRECISION getRMin( void ) const { return _data[ 0 ]; }
		PRECISION getRMax( void ) const { return _data[ 1 ]; }
		PRECISION getUMin( void ) const { return _data[ 2 ]; }
		PRECISION getUMax( void ) const { return _data[ 3 ]; }
		PRECISION getDMin( void ) const { return _data[ 4 ]; }
		PRECISION getDMax( void ) const { return _data[ 5 ]; }

	private:
		PRECISION _data[ 6 ];
	};

	typedef Frustum< float > Frustumf;
	typedef Frustum< double > Frustumd;

}

#endif

