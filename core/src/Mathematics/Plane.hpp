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

#ifndef CRIMILD_CORE_MATHEMATICS_PLANE_
#define CRIMILD_CORE_MATHEMATICS_PLANE_

#include "Vector.hpp"

namespace crimild {

	/**
		\brief Defines a plane in any dimensional space

		A plane is represented by a normal vector and a constant
	 */
	template< crimild::Size SIZE, typename PRECISION >
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

	template< crimild::Size SIZE, typename PRECISION >
	std::ostream &operator<<( std::ostream &out, const Plane< SIZE, PRECISION > &p )
	{
		out << std::setiosflags( std::ios::fixed | std::ios::showpoint  )
			<< std::setprecision( 10 )
			<< "[" << p.getNormal() << ", " << p.getConstant() << "]";
		return out;
	}

	typedef Plane< 3, int > Plane3i;
	typedef Plane< 3, float > Plane3f;
	typedef Plane< 3, double > Plane3d;

}

#endif

