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

#ifndef CRIMILD_MATHEMATICS_INTERPOLATION_
#define CRIMILD_MATHEMATICS_INTERPOLATION_

#include "Numeric.hpp"
#include "Quaternion.hpp"

namespace Crimild {

	/**
		\brief Implements different interpolation calculations

		This class is used to group different interpolation calculations using
		templates which make it useful for several types of objects.
	 */
	class Interpolation {
	public:
		/**
		 	\brief Implements linear interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
		template< typename T >
		static T linear( T start, T end, double t )
		{
			return t * end + ( 1.0 - t ) * start;
		}

		/**
		 	\brief Implements Penner's quadratic ease-in interpolation

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
		template< typename T >
		static T quadraticEaseIn( T start, T end, double t )
		{
			return linear( start, end, t * t );
		}

		/**
		 	\brief Implements Penner's quadratic ease-in-out interpolation

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
		template< typename T >
		static T quadraticEaseInOut( T start, T end, double t )
		{
			T middle = 0.5 * ( start + end );
			t = 2 * t;
			if ( t <= 1 ) {
				return linear( start, middle, t * t );
			}
			t -= 1;
			return linear( middle, end, t * t );
		}

		/**
			\brief Calculate spherical liner interpolation for two quaternions

			\param q0 Original quaternion. Must be unit length
			\param q1 Destination quaternion. Must be unit length

			\remarks This interpolation requires the use of quaternions only
		 */
		template< typename T >
		static Quaternion< T > slerp( const Quaternion< T > &q0, const Quaternion< T > q1, double t )
		{
			// calculate dot product for input quaternion
			// we'll use this as the cosine for the angle between two vectors
			double dot = q0.getReal() * q1.getReal() + q0.getImaginary() * q1.getImaginary();

			const double DOT_THRESHOLD = 0.9995;
			if ( dot > DOT_THRESHOLD ) {
				// if the inputs are too close for comfort, use linear interpolation
				// and normalize the result.
				Quaternion< T > result = linear( q0, q1, t );
				return result.normalize();
			}

			// clamp dot value to stay within the domain for acos()
			dot = Numericd::clamp( dot, -1, 1 );

			// calculate angle from q0 to q1, interpolating according the time value
			double theta = t * acos( dot );

			Quaternion< T > q = q1 - dot * q0;
			q.normalize();
			return cos( theta ) * q0 + sin( theta ) * q;
		}

	};

}

#endif

