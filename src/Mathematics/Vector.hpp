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

#ifndef CRIMILD_CORE_MATHEMATICS_VECTOR_
#define CRIMILD_CORE_MATHEMATICS_VECTOR_

#include <memory>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include "Numeric.hpp"

namespace Crimild {

	/**
		\brief A geometric vector representation

		\todo This class needs proper unit tests
	 */
	template< unsigned int SIZE, typename PRECISION >
	class Vector {
	public:
		Vector( void )
		{
			// vector's data is not initialized in order to avoid overhead
		}

		Vector( PRECISION x, PRECISION y )
		{
			assert( SIZE >= 2 );

			_data[ 0 ] = x;
			_data[ 1 ] = y;

			if ( SIZE > 2 ) {
				memset( &_data[ 2 ], 0, sizeof( PRECISION ) * ( SIZE - 2 ) );
			}
		}

		Vector( PRECISION x, PRECISION y, PRECISION z )
		{
			assert( SIZE >= 3 );

			_data[ 0 ] = x;
			_data[ 1 ] = y;
			_data[ 2 ] = z;

			if ( SIZE > 3 ) {
				memset( &_data[ 3 ], 0, sizeof( PRECISION ) * ( SIZE - 3 ) );
			}
		}

		Vector( PRECISION x, PRECISION y, PRECISION z, PRECISION w )
		{
			assert( SIZE >= 4 );

			_data[ 0 ] = x;
			_data[ 1 ] = y;
			_data[ 2 ] = z;
			_data[ 3 ] = w;

			if ( SIZE > 4 ) {
				memset( &_data[ 4 ], 0, sizeof( PRECISION ) * ( SIZE - 4 ) );
			}
		}

		explicit Vector( const PRECISION *data )
		{
			memcpy( _data, data, sizeof( PRECISION ) * SIZE );
		}

		Vector( const Vector &u )
		{
			memcpy( _data, u._data, sizeof( PRECISION ) * SIZE );
		}
        
        template< typename U >
        Vector( const Vector< SIZE, U > &u )
        {
            for ( unsigned int i = 0; i < SIZE; i++ ) {
                _data[ i ] = static_cast< PRECISION >( u[ i ] );
            }
        }

		~Vector( void )
		{
		}

		Vector &operator=( const Vector &u )
		{
			memcpy( _data, u._data, sizeof( PRECISION ) * SIZE );
			return *this;
		}

		PRECISION x( void ) const { return _data[ 0 ]; }
		PRECISION y( void ) const { return _data[ 1 ]; }
		PRECISION z( void ) const { return _data[ 2 ]; }
		PRECISION w( void ) const { return _data[ 3 ]; }

		PRECISION &x( void ) { return _data[ 0 ]; }
		PRECISION &y( void ) { return _data[ 1 ]; }
		PRECISION &z( void ) { return _data[ 2 ]; }
		PRECISION &w( void ) { return _data[ 3 ]; }

		PRECISION r( void ) const { return _data[ 0 ]; }
		PRECISION g( void ) const { return _data[ 1 ]; }
		PRECISION b( void ) const { return _data[ 2 ]; }
		PRECISION a( void ) const { return _data[ 3 ]; }

		PRECISION &r( void ) { return _data[ 0 ]; }
		PRECISION &g( void ) { return _data[ 1 ]; }
		PRECISION &b( void ) { return _data[ 2 ]; }
		PRECISION &a( void ) { return _data[ 3 ]; }

		Vector< 2, PRECISION > xy( void ) const 
		{ 
			return Vector< 2, PRECISION >( _data[ 0 ], _data[ 1 ] ); 
		}

		Vector< 3, PRECISION > xyz( void ) const 
		{ 
			return Vector< 3, PRECISION >( _data[ 0 ], _data[ 1 ], ( SIZE >= 3 ? _data[ 2 ] : 0 ) ); 
		}

		Vector< 4, PRECISION > xyzw( void ) const 
		{ 
			return Vector< 4, PRECISION >( _data[ 0 ], _data[ 1 ], SIZE >= 3 ? _data[ 2 ] : 0, SIZE >= 4 ? _data[ 3 ] : 0 ); 
		}

		Vector< 3, PRECISION > rgb( void ) const 
		{ 
			return Vector< 3, PRECISION >( _data[ 0 ], _data[ 1 ], SIZE >= 3 ? _data[ 2 ] : 0 ); 
		}

		Vector< 4, PRECISION > rgba( void ) const 
		{ 
			return Vector< 4, PRECISION >( _data[ 0 ], _data[ 1 ], SIZE >= 3 ? _data[ 2 ] : 0, SIZE >= 4 ? _data[ 3 ] : 0 ); 
		}

		PRECISION operator[]( int i ) const { return _data[ i ]; }
		PRECISION &operator[]( int i ) { return _data[ i ]; }

		PRECISION *data( void ) { return _data; }
		const PRECISION *getData( void ) const { return _data; }

		bool operator==( const Vector &u ) const
		{ 
			if ( SIZE == 2 ) {
				return Numeric< PRECISION >::equals( _data[ 0 ], u._data[ 0 ] ) && 
					   Numeric< PRECISION >::equals( _data[ 1 ], u._data[ 1 ] );
			}
			else if ( SIZE == 3 ) {
				return Numeric< PRECISION >::equals( _data[ 0 ], u._data[ 0 ] ) && 
					   Numeric< PRECISION >::equals( _data[ 1 ], u._data[ 1 ] ) &&
					   Numeric< PRECISION >::equals( _data[ 2 ], u._data[ 2 ] );

			}
			else if ( SIZE == 4 ) {
				return Numeric< PRECISION >::equals( _data[ 0 ], u._data[ 0 ] ) && 
					   Numeric< PRECISION >::equals( _data[ 1 ], u._data[ 1 ] ) &&
					   Numeric< PRECISION >::equals( _data[ 2 ], u._data[ 2 ] ) &&
					   Numeric< PRECISION >::equals( _data[ 3 ], u._data[ 3 ] );

			}
			else {
				bool areEqual = true;
				for ( unsigned int i = 0; i < SIZE; i++ ) {
					areEqual &= Numeric< PRECISION >::equals( _data[ i ], u._data[ i ] );
				}
				return areEqual;
			}
		}

		bool operator!=( const Vector &u ) const
		{ 
			return !( *this == u );
		}

		double getSquaredMagnitude( void ) const { return static_cast< double >( ( *this ) * ( *this ) ); }
		double getMagnitude( void ) const { return std::sqrt( getSquaredMagnitude() ); }

		Vector getNormalized( void ) const { return ( *this / getMagnitude() ); }
		Vector &normalize( void ) { *this /= getMagnitude(); return *this; }

		/**
		 	\brief Projects the vector on the given one

		 	This method calculates the vector projection.
		 */
		Vector project( const Vector &base )
		{
			return ( static_cast< PRECISION >( *this * base / ( base.getSquaredMagnitude() ) ) * base );
		}

		template< typename U >
		friend Vector< SIZE, U > operator-( const Vector< SIZE, U > &u );

		template< typename U >
		friend Vector< SIZE, U > operator+( const Vector< SIZE, U > &u, const Vector< SIZE, U > &v );

		template< typename U >
		friend Vector< SIZE, U > operator-( const Vector< SIZE, U > &u, const Vector< SIZE, U > &v );

		template< typename U, typename V >
		friend Vector< SIZE, U > operator*( const Vector< SIZE, U > &u, V scalar );

		template< typename U, typename V >
		friend Vector< SIZE, U > operator*( V scalar, const Vector< SIZE, U > &u );

		template< typename U, typename V >
		friend Vector< SIZE, U > operator/( const Vector< SIZE, U > &u, V scalar );

		template< typename U >
		friend U operator*( const Vector< SIZE, U > &u, const Vector< SIZE, U > &v );

		template< typename U >
		friend Vector< SIZE, U > operator^( const Vector< SIZE, U > &u, const Vector< SIZE, U > &v );

		template< typename U >
		friend Vector< SIZE, U > &operator+=( Vector< SIZE, U > &u, const Vector< SIZE, U > &v );

		template< typename U >
		friend Vector< SIZE, U > &operator-=( Vector< SIZE, U > &u, const Vector< SIZE, U > &v );

		template< typename U, typename V >
		friend Vector< SIZE, U > &operator*=( Vector< SIZE, U > &u, V scalar );

		template< typename U, typename V >
		friend Vector< SIZE, U > &operator/=( Vector< SIZE, U > &u, V scalar );

	protected:
		PRECISION _data[ SIZE ];
	};

	template< typename PRECISION >
	Vector< 2, PRECISION > operator-( const Vector< 2, PRECISION > &u ) 
	{
		Vector< 2, PRECISION > result;
		result._data[ 0 ] = -u._data[ 0 ];
		result._data[ 1 ] = -u._data[ 1 ];
		return result;
	}

	template< typename PRECISION >
	Vector< 3, PRECISION > operator-( const Vector< 3, PRECISION > &u ) 
	{
		Vector< 3, PRECISION > result;
		result._data[ 0 ] = -u._data[ 0 ];
		result._data[ 1 ] = -u._data[ 1 ];
		result._data[ 2 ] = -u._data[ 2 ];
		return result;
	}
	
	template< typename PRECISION >
	Vector< 4, PRECISION > operator-( const Vector< 4, PRECISION > &u ) 
	{
		Vector< 4, PRECISION > result;
		result._data[ 0 ] = -u._data[ 0 ];
		result._data[ 1 ] = -u._data[ 1 ];
		result._data[ 2 ] = -u._data[ 2 ];
		result._data[ 3 ] = -u._data[ 3 ];
		return result;
	}

	template< typename PRECISION >
	Vector< 2, PRECISION > operator+( const Vector< 2, PRECISION > &u, const Vector< 2, PRECISION > &v )
	{
		Vector< 2, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] + v._data[ 0 ];
		result._data[ 1 ] = u._data[ 1 ] + v._data[ 1 ];
		return result;
	}

	template< typename PRECISION >
	Vector< 3, PRECISION > operator+( const Vector< 3, PRECISION > &u, const Vector< 3, PRECISION > &v )
	{
		Vector< 3, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] + v._data[ 0 ];
		result._data[ 1 ] = u._data[ 1 ] + v._data[ 1 ];
		result._data[ 2 ] = u._data[ 2 ] + v._data[ 2 ];
		return result;
	}

	template< typename PRECISION >
	Vector< 4, PRECISION > operator+( const Vector< 4, PRECISION > &u, const Vector< 4, PRECISION > &v )
	{
		Vector< 4, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] + v._data[ 0 ];
		result._data[ 1 ] = u._data[ 1 ] + v._data[ 1 ];
		result._data[ 2 ] = u._data[ 2 ] + v._data[ 2 ];
		result._data[ 3 ] = u._data[ 3 ] + v._data[ 3 ];
		return result;
	}

	template< typename PRECISION >
	Vector< 2, PRECISION > operator-( const Vector< 2, PRECISION > &u, const Vector< 2, PRECISION > &v )
	{
		Vector< 2, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] - v._data[ 0 ];
		result._data[ 1 ] = u._data[ 1 ] - v._data[ 1 ];
		return result;
	}

	template< typename PRECISION >
	Vector< 3, PRECISION > operator-( const Vector< 3, PRECISION > &u, const Vector< 3, PRECISION > &v )
	{
		Vector< 3, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] - v._data[ 0 ];
		result._data[ 1 ] = u._data[ 1 ] - v._data[ 1 ];
		result._data[ 2 ] = u._data[ 2 ] - v._data[ 2 ];
		return result;
	}

	template< typename PRECISION >
	Vector< 4, PRECISION > operator-( const Vector< 4, PRECISION > &u, const Vector< 4, PRECISION > &v )
	{
		Vector< 4, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] - v._data[ 0 ];
		result._data[ 1 ] = u._data[ 1 ] - v._data[ 1 ];
		result._data[ 2 ] = u._data[ 2 ] - v._data[ 2 ];
		result._data[ 3 ] = u._data[ 3 ] - v._data[ 3 ];
		return result;
	}

	template< typename PRECISION, typename V >
	Vector< 2, PRECISION > operator*( const Vector< 2, PRECISION > &u, V scalar )
	{
		Vector< 2, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] * scalar;
		result._data[ 1 ] = u._data[ 1 ] * scalar;
		return result;
	}

	template< typename PRECISION, typename V >
	Vector< 3, PRECISION > operator*( const Vector< 3, PRECISION > &u, V scalar )
	{
		Vector< 3, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] * scalar;
		result._data[ 1 ] = u._data[ 1 ] * scalar;
		result._data[ 2 ] = u._data[ 2 ] * scalar;
		return result;
	}

	template< typename PRECISION, typename V >
	Vector< 4, PRECISION > operator*( const Vector< 4, PRECISION > &u, V scalar )
	{
		Vector< 4, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] * scalar;
		result._data[ 1 ] = u._data[ 1 ] * scalar;
		result._data[ 2 ] = u._data[ 2 ] * scalar;
		result._data[ 3 ] = u._data[ 3 ] * scalar;
		return result;
	}

	template< typename PRECISION, typename V >
	Vector< 2, PRECISION > operator*( V scalar, const Vector< 2, PRECISION > &u )
	{
		Vector< 2, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] * scalar;
		result._data[ 1 ] = u._data[ 1 ] * scalar;
		return result;
	}

	template< typename PRECISION, typename V >
	Vector< 3, PRECISION > operator*( V scalar, const Vector< 3, PRECISION > &u )
	{
		Vector< 3, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] * scalar;
		result._data[ 1 ] = u._data[ 1 ] * scalar;
		result._data[ 2 ] = u._data[ 2 ] * scalar;
		return result;
	}

	template< typename PRECISION, typename V >
	Vector< 4, PRECISION > operator*( V scalar, const Vector< 4, PRECISION > &u )
	{
		Vector< 4, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] * scalar;
		result._data[ 1 ] = u._data[ 1 ] * scalar;
		result._data[ 2 ] = u._data[ 2 ] * scalar;
		result._data[ 3 ] = u._data[ 3 ] * scalar;
		return result;
	}

	template< typename PRECISION, typename V >
	Vector< 2, PRECISION > operator/( const Vector< 2, PRECISION > &u, V scalar )
	{
		V invS = 1.0 / scalar;

		Vector< 2, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] * invS;
		result._data[ 1 ] = u._data[ 1 ] * invS;
		return result;
	}

	template< typename PRECISION, typename V >
	Vector< 3, PRECISION > operator/( const Vector< 3, PRECISION > &u, V scalar )
	{
		V invS = 1.0 / scalar;

		Vector< 3, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] * invS;
		result._data[ 1 ] = u._data[ 1 ] * invS;
		result._data[ 2 ] = u._data[ 2 ] * invS;
		return result;
	}

	template< typename PRECISION, typename V >
	Vector< 4, PRECISION > operator/( const Vector< 4, PRECISION > &u, V scalar )
	{
		V invS = 1.0 / scalar;

		Vector< 4, PRECISION > result;
		result._data[ 0 ] = u._data[ 0 ] * invS;
		result._data[ 1 ] = u._data[ 1 ] * invS;
		result._data[ 2 ] = u._data[ 2 ] * invS;
		result._data[ 3 ] = u._data[ 3 ] * invS;
		return result;
	}

	template< typename PRECISION >
	PRECISION operator*( const Vector< 2, PRECISION > &u, const Vector< 2, PRECISION > &v )
	{
		return u._data[ 0 ] * v._data[ 0 ] + 
			   u._data[ 1 ] * v._data[ 1 ];
	}

	template< typename PRECISION >
	PRECISION operator*( const Vector< 3, PRECISION > &u, const Vector< 3, PRECISION > &v )
	{
		return u._data[ 0 ] * v._data[ 0 ] + 
			   u._data[ 1 ] * v._data[ 1 ] +
			   u._data[ 2 ] * v._data[ 2 ];
	}

	template< typename PRECISION >
	PRECISION operator*( const Vector< 4, PRECISION > &u, const Vector< 4, PRECISION > &v )
	{
		return u._data[ 0 ] * v._data[ 0 ] + 
			   u._data[ 1 ] * v._data[ 1 ] +
			   u._data[ 2 ] * v._data[ 2 ] +
			   u._data[ 3 ] * v._data[ 3 ];
	}

	template< typename PRECISION >
	Vector< 3, PRECISION > operator^( const Vector< 3, PRECISION > &u, const Vector< 3, PRECISION > &v )
	{
		Vector< 3, PRECISION > result;
		result._data[ 0 ] = u._data[ 1 ] * v._data[ 2 ] - u._data[ 2 ] * v._data[ 1 ];
		result._data[ 1 ] = u._data[ 2 ] * v._data[ 0 ] - u._data[ 0 ] * v._data[ 2 ];
		result._data[ 2 ] = u._data[ 0 ] * v._data[ 1 ] - u._data[ 1 ] * v._data[ 0 ];
		return result;
	}

	template< typename PRECISION >
	Vector< 2, PRECISION > &operator+=( Vector< 2, PRECISION > &u, const Vector< 2, PRECISION > &v )
	{
		u._data[ 0 ] += v._data[ 0 ];
		u._data[ 1 ] += v._data[ 1 ];
		return u;
	}

	template< typename PRECISION >
	Vector< 3, PRECISION > &operator+=( Vector< 3, PRECISION > &u, const Vector< 3, PRECISION > &v )
	{
		u._data[ 0 ] += v._data[ 0 ];
		u._data[ 1 ] += v._data[ 1 ];
		u._data[ 2 ] += v._data[ 2 ];
		return u;
	}

	template< typename PRECISION >
	Vector< 4, PRECISION > &operator+=( Vector< 4, PRECISION > &u, const Vector< 4, PRECISION > &v )
	{
		u._data[ 0 ] += v._data[ 0 ];
		u._data[ 1 ] += v._data[ 1 ];
		u._data[ 2 ] += v._data[ 2 ];
		u._data[ 3 ] += v._data[ 3 ];
		return u;
	}

	template< typename PRECISION >
	Vector< 2, PRECISION > &operator-=( Vector< 2, PRECISION > &u, const Vector< 2, PRECISION > &v )
	{
		u._data[ 0 ] -= v._data[ 0 ];
		u._data[ 1 ] -= v._data[ 1 ];
		return u;
	}

	template< typename PRECISION >
	Vector< 3, PRECISION > &operator-=( Vector< 3, PRECISION > &u, const Vector< 3, PRECISION > &v )
	{
		u._data[ 0 ] -= v._data[ 0 ];
		u._data[ 1 ] -= v._data[ 1 ];
		u._data[ 2 ] -= v._data[ 2 ];
		return u;
	}

	template< typename PRECISION >
	Vector< 4, PRECISION > &operator-=( Vector< 4, PRECISION > &u, const Vector< 4, PRECISION > &v )
	{
		u._data[ 0 ] -= v._data[ 0 ];
		u._data[ 1 ] -= v._data[ 1 ];
		u._data[ 2 ] -= v._data[ 2 ];
		u._data[ 3 ] -= v._data[ 3 ];
		return u;
	}

	template< typename PRECISION, typename U >
	Vector< 2, PRECISION > &operator*=( Vector< 2, PRECISION > &u, U scalar )
	{
		u._data[ 0 ] *= scalar;
		u._data[ 1 ] *= scalar;
		return u;
	}

	template< typename PRECISION, typename U >
	Vector< 3, PRECISION > &operator*=( Vector< 3, PRECISION > &u, U scalar )
	{
		u._data[ 0 ] *= scalar;
		u._data[ 1 ] *= scalar;
		u._data[ 2 ] *= scalar;
		return u;
	}

	template< typename PRECISION, typename U >
	Vector< 4, PRECISION > &operator*=( Vector< 4, PRECISION > &u, U scalar )
	{
		u._data[ 0 ] *= scalar;
		u._data[ 1 ] *= scalar;
		u._data[ 2 ] *= scalar;
		u._data[ 3 ] *= scalar;
		return u;
	}

	template< typename PRECISION, typename U >
	Vector< 2, PRECISION > &operator/=( Vector< 2, PRECISION > &u, U scalar )
	{
		U invS = 1.0 / scalar;
		u._data[ 0 ] *= invS;
		u._data[ 1 ] *= invS;
		return u;
	}

	template< typename PRECISION, typename U >
	Vector< 3, PRECISION > &operator/=( Vector< 3, PRECISION > &u, U scalar )
	{
		U invS = 1.0 / scalar;
		u._data[ 0 ] *= invS;
		u._data[ 1 ] *= invS;
		u._data[ 2 ] *= invS;
		return u;
	}

	template< typename PRECISION, typename U >
	Vector< 4, PRECISION > &operator/=( Vector< 4, PRECISION > &u, U scalar )
	{
		U invS = 1.0 / scalar;
		u._data[ 0 ] *= invS;
		u._data[ 1 ] *= invS;
		u._data[ 2 ] *= invS;
		u._data[ 3 ] *= invS;
		return u;
	}

	template< unsigned int SIZE, typename PRECISION >
	std::ostream &operator<<( std::ostream &out, const Vector< SIZE, PRECISION > &v )
	{
		out << std::setiosflags( std::ios::fixed | std::ios::showpoint  )
			<< std::setprecision( 10 );
		out << "(" << v[ 0 ];
		for ( unsigned int i = 1; i < SIZE; i++ ) {
			out << ", " << v[ i ];
		}
		out << ")";
		return out;
	}

	typedef Vector< 2, unsigned int > Vector2ui;
	typedef Vector< 2, int > Vector2i;
	typedef Vector< 2, float > Vector2f;

	typedef Vector< 3, int > Vector3i;
	typedef Vector< 3, short > Vector3s;
	typedef Vector< 3, char > Vector3c;
	typedef Vector< 3, unsigned char > Vector3uc;
	typedef Vector< 3, float > Vector3f;
	typedef Vector< 3, double > Vector3d;

	typedef Vector< 4, int > Vector4i;
	typedef Vector< 4, float > Vector4f;

	typedef Vector< 4, float > RGBAColorf;
	typedef Vector< 3, float > RGBColorf;

}

#endif

