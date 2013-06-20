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

#ifndef CRIMILD_MATHEMATICS_FRUSTUM_
#define CRIMILD_MATHEMATICS_FRUSTUM_

#include "Numeric.hpp"
#include "Matrix.hpp"

namespace crimild {

	template< typename PRECISION >
	class Frustum {
	public:
		Frustum( void )
		{
		}

		Frustum( PRECISION rMin, PRECISION rMax, PRECISION uMin, PRECISION uMax, PRECISION dMin, PRECISION dMax )
		{
			_data[ 0 ] = rMin;
			_data[ 1 ] = rMax;
			_data[ 2 ] = uMin;
			_data[ 3 ] = uMax;
			_data[ 4 ] = dMin;
			_data[ 5 ] = dMax;
		}

		Frustum( PRECISION fov, PRECISION aspect, PRECISION near, PRECISION far )
		{
			PRECISION invE = static_cast< PRECISION >( tan( fov * Numeric< PRECISION >::DEG_TO_RAD / 2.0 ) );
			PRECISION temp = near * invE;

			_data[ 0 ] = -temp;
			_data[ 1 ] = +temp;
			_data[ 2 ] = ( -1.0 / aspect ) * temp;
			_data[ 3 ] = ( +1.0 / aspect ) * temp;
			_data[ 4 ] = near;
			_data[ 5 ] = far;
		}

		Frustum( const Frustum &frustum )
		{
			memcpy( _data, frustum._data, 6 * sizeof( PRECISION ) );
		}

		~Frustum( void )
		{
		}

		bool operator==( const Frustum &frustum ) const
		{
			return memcmp( _data, frustum._data, 6 * sizeof( PRECISION ) ) == 0;
		}

		bool operator!=( const Frustum &frustum ) const
		{
			return memcmp( _data, frustum._data, 6 * sizeof( PRECISION ) ) != 0;
		}

		PRECISION getRMin( void ) const { return _data[ 0 ]; }
		PRECISION getRMax( void ) const { return _data[ 1 ]; }
		PRECISION getUMin( void ) const { return _data[ 2 ]; }
		PRECISION getUMax( void ) const { return _data[ 3 ]; }
		PRECISION getDMin( void ) const { return _data[ 4 ]; }
		PRECISION getDMax( void ) const { return _data[ 5 ]; }

		Matrix< 4, PRECISION > computeProjectionMatrix( void )
		{
            float n = getDMin();
            float f = getDMax();
            float r = getRMax();
            float l = getRMin();
            float t = getUMax();
            float b = getUMin();

            Matrix< 4, PRECISION > projectionMatrix;
            projectionMatrix[ 0 ] = 2 * n / ( r - l );
            projectionMatrix[ 1 ] = 0;
            projectionMatrix[ 2 ] = 0;
            projectionMatrix[ 3 ] = 0;

            projectionMatrix[ 4 ] = 0;
            projectionMatrix[ 5 ] = 2 * n / ( t - b );
            projectionMatrix[ 6 ] = 0;
            projectionMatrix[ 7 ] = 0;

            projectionMatrix[ 8 ] = ( r + l ) / ( r - l );
            projectionMatrix[ 9 ] = ( t + b ) / ( t - b );
            projectionMatrix[ 10 ] = -( f + n ) / ( f - n );
            projectionMatrix[ 11 ] = -1.0f;

            projectionMatrix[ 12 ] = 0;
            projectionMatrix[ 13 ] = 0;
            projectionMatrix[ 14 ] = -2.0f * f * n / ( f - n );
            projectionMatrix[ 15 ] = 1.0f;
            return projectionMatrix;
		}

	private:
		PRECISION _data[ 6 ];
	};

	typedef Frustum< float > Frustumf;
	typedef Frustum< double > Frustumd;

}

#endif

