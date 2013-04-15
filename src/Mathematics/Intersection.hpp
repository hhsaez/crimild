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

#ifndef CRIMILD_MATHEMATICS_INTERSECTION_
#define CRIMILD_MATHEMATICS_INTERSECTION_

#include "Numeric.hpp"
#include "Plane.hpp"
#include "Sphere.hpp"
#include "Ray.hpp"
#include "Root.hpp"
#include "Vector.hpp"

namespace Crimild {

	class Intersection {
	public:
		/**
			\brief Test for intersection between a sphere and a ray
		 */
		template< typename T >
		static bool test( const Sphere< T > &sphere, const Ray< 3, T > &ray )
		{
			Vector< 3, T > centerDiff( ray.getOrigin() - sphere.getCenter() );
			T a = ray.getDirection().getSquaredMagnitude();
			T b = 2 * ( centerDiff * ray.getDirection() );
			T c = centerDiff.getSquaredMagnitude() - ( sphere.getRadius() * sphere.getRadius() );

	        T t0, t1;
			if ( Root::compute( a, b, c, t0, t1 ) > 0 ) {
				return ( Numeric< T >::max( t0, t1 ) >= 0.0f );
			}

			return false;
		}

		/**
			\brief Find the intersection between a sphere and a ray
			\return The intersection time (-1 if they do not intersect)
		 */
		template< typename T >
		static T find( const Sphere< T > &sphere, const Ray< 3, T > &ray )
		{
			Vector< 3, T > centerDiff( ray.getOrigin() - sphere.getCenter() );
			T a = ray.getDirection().getSquaredMagnitude();
			T b = 2 * ( centerDiff * ray.getDirection() );
			T c = centerDiff.getSquaredMagnitude() - ( sphere.getRadius() * sphere.getRadius() );

	        T t0, t1;
			if ( Root::compute( a, b, c, t0, t1 ) > 0 ) {
				return Numeric< T >::max( t0, t1 );
			}
			else {
				return -1;
			}
		}

		/**
			\brief Test for intersection between two spheres
			\returns true if spheres intersect
		 */
		template< typename T >
		static bool test( const Sphere< T > &sphere1, const Sphere< T > &sphere2 )
		{
			T centerDiffSqr = ( sphere1.getCenter() - sphere2.getCenter() ).getSquaredMagnitude();
			T combinedRadius = ( sphere1.getRadius() + sphere2.getRadius() );
			return ( centerDiffSqr < ( combinedRadius * combinedRadius ) );
		}

		/**
			\brief Find the intersection between two spheres
			\todo This method is not implemented yet
		 */
		template< typename T >
		static T find( const Sphere< T > &sphere1, const Sphere< T > &sphere2 )
		{
			return -1;
		}

		/**
			\brief Test for intersection between a sphere and a ray
		 */
		template< typename T >
		static bool test( const Plane< 3, T > &plane, const Ray< 3, T > &ray )
		{
			if ( ( plane.getNormal() * ray.getDirection() ) == 0 ) {
				return false;
			}

			return true;
		}

		/**
			\brief Find the intersection between a plane and a ray
			\return The intersection time (-1 if they do not intersect)
		 */
		template< typename T >
		static T find( const Plane< 3, T > &plane, const Ray< 3, T > &ray )
		{
			T nv = plane.getNormal() * ray.getDirection();
			if ( nv != 0 ) {
				return ( ( -plane.getNormal() * ray.getOrigin() ) + plane.getConstant() ) / nv;
			}
			else {
				return -1;
			}
		}

		/**
			\brief Ray-Triangle intersection test

			Test for intersection between a ray and a triangle given by three points
		 */
		template< typename T >
		static bool test( const Ray< 3, T > &ray, const Vector< 3, T > &p0, const Vector< 3, T > &p1, const Vector< 3, T > &p2 )
		{
			Vector< 3, T > edge1 = p1 - p0;
			Vector< 3, T > edge2 = p2 - p0;
			Vector< 3, T > triNormal = edge1 ^ edge2;
			if ( ( triNormal * ray.getDirection() ) == 0 ) {
				return false;
			}

			return true;
		}

		/**
			\brief Intersection between three planes
		 */
		template< typename T >
		static Vector< 3, T > find( const Plane< 3, T > &plane1, const Plane< 3, T > &plane2, const Plane< 3, T > &plane3 )
		{
			return Vector< 3, T >( 0, 0, 0 );
		}

		/**
			\brief Ray-Triangle intersection

			Find the intersection between a ray and a triangle given by three points
		 */
		template< typename T >
		static T find( const Ray< 3, T > &ray, const Vector< 3, T > &p0, const Vector< 3, T > &p1, const Vector< 3, T > &p2 )
		{
			return -1;
		}

	};

}

#endif

