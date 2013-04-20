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

