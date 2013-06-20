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

#ifndef CRIMILD_MATHEMATICS_INTERPOLATION_
#define CRIMILD_MATHEMATICS_INTERPOLATION_

#include "Numeric.hpp"
#include "Quaternion.hpp"

namespace crimild {

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

