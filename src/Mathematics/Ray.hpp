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

#ifndef CRIMILD_MATHEMATICS_RAY_
#define CRIMILD_MATHEMATICS_RAY_

#include "Vector.hpp"

namespace Crimild {

	/**
		\brief Defines a ray in any dimensional space

		A ray is represented by an origin and a direction
	 */
	template< unsigned int SIZE, typename PRECISION >
	class Ray {
	public:
		typedef Vector< SIZE, PRECISION > VectorImpl;

		Ray( void )
		{
		}

		Ray( const VectorImpl &origin, const VectorImpl &direction )
			: _origin( origin ),
			  _direction( direction )
		{
		}

		Ray( const Ray &ray )
			: _origin( ray._origin ),
			  _direction( ray._direction )
		{
		}

		~Ray( void )
		{
		}

		Ray &operator=( const Ray &ray )
		{
			_origin = ray._origin;
			_direction = ray._direction;
			return *this;
		}

		const VectorImpl &getOrigin( void ) const
		{
			return _origin;
		}

		VectorImpl &getOrigin( void )
		{
			return _origin;
		}

		void setOrigin( const VectorImpl &origin )
		{
			_origin = origin;
		}

		const VectorImpl &getDirection( void ) const
		{
			return _direction;
		}

		VectorImpl &getDirection( void )
		{
			return _direction;
		}

		void setDirection( const VectorImpl &direction )
		{
			_direction = direction;
		}

		bool operator==( const Ray &ray )
		{
			return ( _origin == ray._origin && _direction == ray._direction );
		}

		bool operator!=( const Ray &ray )
		{
			return !( *this == ray );
		}

	private:
		VectorImpl _origin;
		VectorImpl _direction;
	};

	typedef Ray< 3, int > Ray3i;
	typedef Ray< 3, float > Ray3f;
	typedef Ray< 3, double > Ray3d;

}

#endif

