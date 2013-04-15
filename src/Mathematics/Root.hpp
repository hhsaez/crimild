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

#ifndef CRIMILD_MATHEMATICS_ROOT_
#define CRIMILD_MATHEMATICS_ROOT_

namespace Crimild {

	/**
		\brief Find the roots for a polynomial
	 */
	class Root {
	public:
		/**
			\brief Determine if the polynomial has real roots
		 */
		template< typename T >
		static bool hasRealRoots( T a, T b, T c )
		{
			T discriminant = b * b - 4 * a * c;

			if ( discriminant < 0 ) {
				return false;	// no real roots
			}
			else {
				return true;	// real roots exist
			}
		}

		/**
			\brief Calculate roots for a quadratic polynomial
			\returns Number of roots found (0, 1 or 2)

			If there is no real roots for this polynomial, the
			calculate() function returns 0
		 */
		template< typename T >
		static int compute( T a, T b, T c, T &t0, T &t1 )
		{
			T discriminant = b * b - 4 * a * c;

			if ( discriminant < 0 ) {
				return 0;	// no real roots
			}
			else {
				T s = -b / 2 * a;

				if ( discriminant == 0 ) {
					t0 = t1 = -b / 2 * a;
					return 1;	// one real root
				}
				else {
					T sqrtDiscriminant = static_cast< T >( std::sqrt( discriminant ) / a );
					t0 = s + sqrtDiscriminant;
					t1 = s - sqrtDiscriminant;
					return 2;	// two real roots
				}
			}
		}
	};

}

#endif

