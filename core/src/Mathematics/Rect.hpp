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

#ifndef CRIMILD_MATHEMATICS_RECT_
#define CRIMILD_MATHEMATICS_RECT_

#include <memory>

namespace crimild {

	template< typename PRECISION >
	class Rect {
	public:
		Rect( void )
		{
		}

		Rect( PRECISION x, PRECISION y, PRECISION width, PRECISION height )
		{
			_data[ 0 ] = x;
			_data[ 1 ] = y;
			_data[ 2 ] = width;
			_data[ 3 ] = height;
		}

		Rect( const Rect &rect )
		{
			*this = rect;
		}

		~Rect( void )
		{
		}

		Rect &operator=( const Rect &rect )
		{
			memcpy( _data, rect._data, 4 * sizeof( PRECISION ) );
			return *this;
		}

		PRECISION &x( void ) { return _data[ 0 ]; }
		PRECISION &y( void ) { return _data[ 1 ]; }
		PRECISION &width( void ) { return _data[ 2 ]; }
		PRECISION &height( void ) { return _data[ 3 ]; }

		PRECISION getX( void ) const { return _data[ 0 ]; }
		PRECISION getY( void ) const { return _data[ 1 ]; }
		PRECISION getWidth( void ) const { return _data[ 2 ]; }
		PRECISION getHeight( void ) const { return _data[ 3 ]; }

	private:
		PRECISION _data[ 4 ];
	};

	typedef Rect< int > Recti;
	typedef Rect< float > Rectf;

}

#endif

