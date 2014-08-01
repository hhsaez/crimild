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

#ifndef CRIMILD_MATHEMATICS_DISTANCE_
#define CRIMILD_MATHEMATICS_DISTANCE_

#include "Ray.hpp"
#include "Vector.hpp"
#include "Plane.hpp"

namespace crimild {

	class Distance {
	public:
		template< unsigned int SIZE, typename PRECISION >
		static double compute( const Vector< SIZE, PRECISION > &u, const Vector< SIZE, PRECISION > &v ) 
		{
			return std::sqrt( computeSquared( u, v ) );
		}

		template< unsigned int SIZE, typename PRECISION >
		static double computeSquared( const Vector< SIZE, PRECISION > &u, const Vector< SIZE, PRECISION > &v ) 
		{
			return ( v - u ).getSquaredMagnitude();
		}

		template< unsigned int SIZE, typename PRECISION >
		static double compute( const Ray< SIZE, PRECISION > &ray, const Vector< SIZE, PRECISION > &point )
		{
			return std::sqrt( computeSquared( ray, point ) );
		}

		template< unsigned int SIZE, typename PRECISION >
		static double computeSquared( const Ray< SIZE, PRECISION > &ray, const Vector< SIZE, PRECISION > &point )
		{
			Vector< SIZE, PRECISION > v0 = point - ray.getOrigin();
			double v1 = v0 * ray.getDirection();
			return ( v0 * v0 - v1 * v1 / ( ray.getDirection().getSquaredMagnitude() ) );
		}

		template< unsigned int SIZE, typename PRECISION >
		static double compute( const Plane< SIZE, PRECISION > &plane, const Vector< SIZE, PRECISION > &point )
		{
			return ( plane.getNormal() * point ) + plane.getConstant();
		}
	};

}

#endif

