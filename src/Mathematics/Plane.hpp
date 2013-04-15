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

#ifndef CRIMILD_CORE_MATHEMATICS_PLANE_
#define CRIMILD_CORE_MATHEMATICS_PLANE_

#include "Vector.hpp"

namespace Crimild {

	/**
		\brief Defines a plane in any dimensional space

		A plane is represented by a normal vector and a constant
	 */
	template< unsigned int SIZE, typename PRECISION >
	class Plane {
	public:
		typedef Vector< SIZE, PRECISION > VectorImpl;

		Plane( void )
		{
		}

		Plane( const VectorImpl &normal, PRECISION constant, bool forceNormalize = true )
		{
			setNormal( normal, forceNormalize );
			setConstant( constant );
		}

		Plane( const VectorImpl &normal, const VectorImpl &point, bool forceNormalize = true )
		{
			setNormal( normal, forceNormalize );
			setConstant( _normal * point );
		}

		Plane( const VectorImpl &p0, const VectorImpl &p1, const VectorImpl p2 )
		{
			setNormal( ( p2 - p1 ) ^ ( p0 - p1 ) );
			setConstant( _normal * p0 );
		}

		Plane( const Plane &plane )
			: _normal( plane._normal ),
			  _constant( plane._constant )
		{
		}

		~Plane( void )
		{
		}

		Plane &operator=( const Plane &plane )
		{
			_normal = plane._normal;
			_constant = plane._constant;
			return *this;
		}

		bool operator==( const Plane &plane )
		{
			return ( _normal == plane._normal && _constant == plane._constant );
		}

		bool operator!=( const Plane &plane )
		{
			return !( *this == plane );
		}

		void setNormal( const VectorImpl &normal, bool forceNormalize = true )
		{
			_normal = normal;
			if ( forceNormalize ) {
				_normal.normalize();
			}
		}

		VectorImpl &getNormal( void )
		{
			return _normal;
		}

		const VectorImpl &getNormal( void ) const
		{
			return _normal;
		}

		void setConstant( PRECISION constant )
		{
			_constant = constant;
		}

		PRECISION getConstant( void ) const
		{
			return _constant;
		}

	private:
		VectorImpl _normal;
		PRECISION _constant;
	};

	typedef Plane< 3, int > Plane3i;
	typedef Plane< 3, float > Plane3f;
	typedef Plane< 3, double > Plane3d;

}

#endif

