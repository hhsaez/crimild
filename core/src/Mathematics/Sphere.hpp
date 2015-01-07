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

#ifndef CRIMILD_MATHEMATICS_SPHERE_
#define CRIMILD_MATHEMATICS_SPHERE_

#include "Vector.hpp"
#include "Numeric.hpp"
#include "Plane.hpp"
#include "Distance.hpp"

namespace crimild {

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
			double d = Distance::compute( plane, _center );
			if ( d < -_radius ) {
				return -1; // behind
			}
			else if ( d > _radius ) {
				return 1; // front
			}

			return 0; // intersecting
		}

	private:
		VectorImpl _center;
		T _radius;
	};

	typedef Sphere< float > Sphere3f;
	typedef Sphere< double > Sphere3d;

}

#endif

