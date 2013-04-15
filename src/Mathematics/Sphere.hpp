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

#ifndef CRIMILD_MATHEMATICS_SPHERE_
#define CRIMILD_MATHEMATICS_SPHERE_

#include "Vector.hpp"
#include "Numeric.hpp"
#include "Plane.hpp"
#include "Distance.hpp"

namespace Crimild {

	template< typename T >
	class Sphere {
	public:
		typedef Vector< 3, T > VectorImpl;
		typedef Plane< 3, T > PlaneImpl;

		Sphere( void )
		{
		}

		Sphere( const VectorImpl &c, T r )
			: _center( c ),
			  _radius( r )
		{
		}

		Sphere( const Sphere &sphere )
			: _center( sphere._center ),
			  _radius( sphere._radius )
		{
		}

		~Sphere( void )
		{
		}

		Sphere &operator=( const Sphere &sphere )
		{
			_center = sphere._center;
			_radius = sphere._radius;
		}

		void setCenter( const VectorImpl &c )
		{
			_center = c;
		}

		VectorImpl &getCenter( void )
		{
			return _center;
		}

		const VectorImpl &getCenter( void ) const
		{
			return _center;
		}

		VectorImpl &center( void )
		{
			return _center;
		}

		void setRadius( T r )
		{
			_radius = r;
		}

		T getRadius( void ) const
		{
			return _radius;
		}

		T &radius( void )
		{
			return _radius;
		}

		void expandToContain( const Sphere &sphere )
		{
			VectorImpl centerDiff = sphere._center - _center;
		   	T lengthSqr = static_cast< T >( centerDiff.getSquaredMagnitude() );
		   	T radiusDiff = sphere._radius - _radius;
		   	T radiusDiffSqr = radiusDiff * radiusDiff;

		   	if ( radiusDiffSqr >= lengthSqr ) {
		   		if ( radiusDiff >= 0 ) {
		   			_center = sphere._center;
		   			_radius = sphere._radius;
		   		}
		   	}
		   	else {
		   		T length = std::sqrt( lengthSqr );
		   		if ( length > Numeric< T >::ZERO_TOLERANCE ) {
		   			T coeff = static_cast< T >( ( length + radiusDiff ) / ( 2.0 * length ) );
		   			_center = _center + coeff * centerDiff;
		   		}

		   		_radius = static_cast< T >( 0.5 * ( length + _radius + sphere._radius ) );
		   	}
		}

		int whichSide( const PlaneImpl &plane ) const
		{
			T d = Distance::compute( plane, _center );

			if ( d < -_radius ) {
				return -1;
			}
			else if ( d > +_radius ) {
				return +1;
			}
			else {
				return 0;
			}
		}

	private:
		VectorImpl _center;
		T _radius;
	};

	typedef Sphere< float > Sphere3f;
	typedef Sphere< double > Sphere3d;

}

#endif

