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

#ifndef CRIMILD_MATHEMATICS_NUMERIC_
#define CRIMILD_MATHEMATICS_NUMERIC_

#include <cstdio>
#include <cstdlib>
#include <cmath>

namespace Crimild {

	template< typename PRECISION >
	class Numeric {
	public:
		static const PRECISION ZERO_TOLERANCE;
		static const PRECISION PI;
		static const PRECISION HALF_PI;
		static const PRECISION TWO_PI;
		static const PRECISION SQRT_TWO_DIV_TWO;

		static const PRECISION DEG_TO_RAD;
		static const PRECISION RAD_TO_DEG;

		static bool isZero( PRECISION number )
		{
			return ( std::fabs( number ) <= ZERO_TOLERANCE );
		}

		static bool equals( PRECISION x, PRECISION y )
		{
			return ( std::fabs( x - y ) <= ZERO_TOLERANCE );
		}

		static bool notEqual( PRECISION x, PRECISION y )
		{
			return !equal( x, y );
		}

		static bool less( PRECISION x, PRECISION y )
		{
			return x < y;
		}

		static bool lEqual( PRECISION x, PRECISION y )
		{
			return x <= y;
		}

		static bool greater( PRECISION x, PRECISION y )
		{
			return x > y;
		}

		static bool gEqual( PRECISION x, PRECISION y )
		{
			return x >= y;
		}

		static PRECISION max( PRECISION x, PRECISION y )
		{
			return ( x > y ? x : y );
		}

		static PRECISION min( PRECISION x, PRECISION y )
		{
			return ( x < y ? x : y );
		}

		static PRECISION squared( PRECISION x )
		{
			return ( x * x );
		}

		static PRECISION noiseFunc( PRECISION reference )
		{
			return sqrt( -2 * log( ( PRECISION ) std::rand() / RAND_MAX ) ) * sin( 2 * PI * ( PRECISION ) std::rand() / RAND_MAX ) * reference;
		}

		static PRECISION clamp( PRECISION input, PRECISION min, PRECISION max )
		{
			PRECISION result = input;
			if ( result > max ) {
				result = max;
			}
			if ( result < min ) {
				result = min;
			}
			return result;
		}

		static int sign( PRECISION input )
		{
			return input < 0 ? -1 : 1;
		}

	};

	template< typename T > const T Numeric< T >::ZERO_TOLERANCE = static_cast< T >( 1e-06 );
	template< typename T > const T Numeric< T >::PI = static_cast< T >( 3.1415926535897932384626433832795 );
	template< typename T > const T Numeric< T >::HALF_PI = static_cast< T >( 3.1415926535897932384626433832795 / 2.0 );
	template< typename T > const T Numeric< T >::TWO_PI = static_cast< T >( 3.1415926535897932384626433832795 * 2 );
	template< typename T > const T Numeric< T >::SQRT_TWO_DIV_TWO = static_cast< T >( 0.5 * std::sqrt( 2 ) );
	template< typename T > const T Numeric< T >::DEG_TO_RAD = static_cast< T >( 3.1415926535897932384626433832795 / 180.0 );
	template< typename T > const T Numeric< T >::RAD_TO_DEG = static_cast< T >( 180.0 / 3.1415926535897932384626433832795 );

	typedef Numeric< int > Numerici;
	typedef Numeric< float > Numericf;
	typedef Numeric< double > Numericd;

}

#endif

