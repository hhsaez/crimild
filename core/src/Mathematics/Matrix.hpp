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

#ifndef CRIMILD_CORE_MATHEMATICS_MATRIX_
#define CRIMILD_CORE_MATHEMATICS_MATRIX_

#include "Vector.hpp"
#include "Numeric.hpp"

namespace crimild {

	/**
		\brief A geometric matrix
	 */
	template< crimild::Size SIZE, typename PRECISION >
	class Matrix {
	public:
		Matrix( void )
		{
		}

		explicit Matrix( const PRECISION *data )
		{
			memcpy( _data, data, sizeof( PRECISION ) * SIZE * SIZE );
		}

		Matrix( const Matrix &u )
		{
			memcpy( _data, u._data, sizeof( PRECISION ) * SIZE * SIZE );
		}

		Matrix( const Vector< SIZE, PRECISION > &axis, PRECISION angle )
		{
			fromAxisAngle( axis, angle );
		}

		Matrix( PRECISION a00, PRECISION a01,
				PRECISION a10, PRECISION a11 )
		{
			_data[ 0 ] = a00;
			_data[ 1 ] = a01;
			_data[ 2 ] = a10;
			_data[ 3 ] = a11;
		}

		Matrix( PRECISION a00, PRECISION a01, PRECISION a02,
				PRECISION a10, PRECISION a11, PRECISION a12,
				PRECISION a20, PRECISION a21, PRECISION a22 )
		{
			_data[ 0 ] = a00;
			_data[ 1 ] = a01;
			_data[ 2 ] = a02;
			_data[ 3 ] = a10;
			_data[ 4 ] = a11;
			_data[ 5 ] = a12;
			_data[ 6 ] = a20;
			_data[ 7 ] = a21;
			_data[ 8 ] = a22;
		}

		Matrix( PRECISION a00, PRECISION a01, PRECISION a02, PRECISION a03,
				PRECISION a10, PRECISION a11, PRECISION a12, PRECISION a13,
				PRECISION a20, PRECISION a21, PRECISION a22, PRECISION a23,
				PRECISION a30, PRECISION a31, PRECISION a32, PRECISION a33 )
		{
			_data[ 0 ] = a00;
			_data[ 1 ] = a01;
			_data[ 2 ] = a02;
			_data[ 3 ] = a03;
			_data[ 4 ] = a10;
			_data[ 5 ] = a11;
			_data[ 6 ] = a12;
			_data[ 7 ] = a13;
			_data[ 8 ] = a20;
			_data[ 9 ] = a21;
			_data[ 10 ] = a22;
			_data[ 11 ] = a23;
			_data[ 12 ] = a30;
			_data[ 13 ] = a31;
			_data[ 14 ] = a32;
			_data[ 15 ] = a33;
		}

		~Matrix( void )
		{
		}

		Matrix &operator=( const Matrix &u )
		{
			memcpy( _data, u._data, sizeof( PRECISION ) * SIZE * SIZE );
			return *this;
		}

		PRECISION operator[]( int i ) const
		{
			return _data[ i ];
		}

		PRECISION &operator[]( int i )
		{
			return _data[ i ];
		}

		// explicit conversion
		explicit operator PRECISION *(void) { return &_data[0]; }
		explicit operator const PRECISION *(void) const { return &_data[0]; }

		PRECISION *data( void ) { return _data; }
		const PRECISION *getData( void ) const { return _data; }

		bool operator==( const Matrix &u ) const
		{
			if ( SIZE == 3 ) {
				return ( Numeric< PRECISION >::equals( _data[ 0 ], u._data[ 0 ] ) ) &&
					   ( Numeric< PRECISION >::equals( _data[ 1 ], u._data[ 1 ] ) ) &&
					   ( Numeric< PRECISION >::equals( _data[ 2 ], u._data[ 2 ] ) ) &&
					   ( Numeric< PRECISION >::equals( _data[ 3 ], u._data[ 3 ] ) ) &&
					   ( Numeric< PRECISION >::equals( _data[ 4 ], u._data[ 4 ] ) ) &&
					   ( Numeric< PRECISION >::equals( _data[ 5 ], u._data[ 5 ] ) ) &&
					   ( Numeric< PRECISION >::equals( _data[ 6 ], u._data[ 6 ] ) ) &&
					   ( Numeric< PRECISION >::equals( _data[ 7 ], u._data[ 7 ] ) );
			}
			else if ( SIZE == 2 ) {
				return ( Numeric< PRECISION >::equals( _data[ 0 ], u._data[ 0 ] ) ) &&
					   ( Numeric< PRECISION >::equals( _data[ 1 ], u._data[ 1 ] ) ) &&
					   ( Numeric< PRECISION >::equals( _data[ 2 ], u._data[ 2 ] ) ) &&
					   ( Numeric< PRECISION >::equals( _data[ 3 ], u._data[ 3 ] ) );
			}
			else {
				bool areEqual = true;
				for ( crimild::Size i = 0; i < SIZE * SIZE; i++ ) {
					areEqual &= Numeric< PRECISION >::equals( _data[ i ], u._data[ i ] );
				}
				return areEqual;
			}
		}

		bool operator!=( const Matrix &u )
		{
			return ( memcmp( _data, u._data, sizeof( PRECISION ) * SIZE * SIZE ) != 0 );
		}

		Matrix &makeIdentity( void )
		{
			if ( SIZE == 3 ) {
				_data[ 0 ] = 1;
				_data[ 1 ] = 0;
				_data[ 2 ] = 0;
				_data[ 3 ] = 0;
				_data[ 4 ] = 1;
				_data[ 5 ] = 0;
				_data[ 6 ] = 0;
				_data[ 7 ] = 0;
				_data[ 8 ] = 1;
			}
			else if ( SIZE == 2 ) {
				_data[ 0 ] = 1;
				_data[ 1 ] = 0;
				_data[ 2 ] = 0;
				_data[ 3 ] = 1;
			}
			else {
				for ( crimild::Size i = 0; i < SIZE; i++ ) {
					for ( crimild::Size j = 0; j < SIZE; j++ ) {
						if ( i == j ) {
							_data[ i * SIZE + j ] = 1;
						}
						else {
							_data[ i * SIZE + j ] = 0;
						}
					}
				}
			}

			return *this;
		}

		Matrix getTranspose( void ) const
		{
			Matrix result;

			if ( SIZE == 3 ) {
				result._data[ 0 ] = _data[ 0 ];
				result._data[ 1 ] = _data[ 3 ];
				result._data[ 2 ] = _data[ 6 ];
				result._data[ 3 ] = _data[ 1 ];
				result._data[ 4 ] = _data[ 4 ];
				result._data[ 5 ] = _data[ 7 ];
				result._data[ 6 ] = _data[ 2 ];
				result._data[ 7 ] = _data[ 5 ];
				result._data[ 8 ] = _data[ 8 ];
			}
			else if ( SIZE == 2 ) {
				result._data[ 0 ] = _data[ 0 ];
				result._data[ 1 ] = _data[ 2 ];
				result._data[ 2 ] = _data[ 1 ];
				result._data[ 3 ] = _data[ 3 ];
			}
			else {
				for ( crimild::Size i = 0; i < SIZE; i++ ) {
					for ( crimild::Size j = 0; j < SIZE; j++ ) {
						result._data[ j * SIZE + i ] = _data[ i * SIZE + j ];
					}
				}
			}

			return result;
		}

		Matrix &makeTranspose( void ) 
		{
			*this = getTranspose();
			return *this;
		}

		double getDeterminant( void ) const
		{
			double result = 1.0;

			if ( SIZE == 3 ) {
				result = _data[ 0 ] * ( _data[ 4 ] * _data[ 8 ] - _data[ 5 ] * _data[ 7 ] )
						 - _data[ 1 ] * ( _data[ 3 ] * _data[ 8 ] - _data[ 5 ] * _data[ 6 ] )
						 + _data[ 2 ] * ( _data[ 3 ] * _data[ 7 ] - _data[ 4 ] * _data[ 6 ] );
			}
			else if ( SIZE == 2 ) {
				result = _data[ 0 ] * _data[ 3 ] - _data[ 1 ] * _data[ 2 ];
			}
            else if ( SIZE == 4 ) {
                PRECISION a00 = _data[ 0 ], a01 = _data[ 1 ], a02 = _data[ 2 ], a03 = _data[ 3 ],
        			  	  a10 = _data[ 4 ], a11 = _data[ 5 ], a12 = _data[ 6 ], a13 = _data[ 7 ],
        			  	  a20 = _data[ 8 ], a21 = _data[ 9 ], a22 = _data[ 10 ], a23 = _data[ 11 ],
        			  	  a30 = _data[ 12 ], a31 = _data[ 13 ], a32 = _data[ 14 ], a33 = _data[ 15 ];
                
                PRECISION b00 = a00 * a11 - a01 * a10;
                PRECISION b01 = a00 * a12 - a02 * a10;
                PRECISION b02 = a00 * a13 - a03 * a10;
                PRECISION b03 = a01 * a12 - a02 * a11;
                PRECISION b04 = a01 * a13 - a03 * a11;
                PRECISION b05 = a02 * a13 - a03 * a12;
                PRECISION b06 = a20 * a31 - a21 * a30;
                PRECISION b07 = a20 * a32 - a22 * a30;
                PRECISION b08 = a20 * a33 - a23 * a30;
                PRECISION b09 = a21 * a32 - a22 * a31;
                PRECISION b10 = a21 * a33 - a23 * a31;
                PRECISION b11 = a22 * a33 - a23 * a32;

                result = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
            }

			return result;
		}

        bool isInvertible( void ) const
        {
            return getDeterminant() != 0;
        }

		Matrix getInverse( void ) const
		{
			double d = getDeterminant();
			assert( d != 0 );

			double invDet = static_cast< double >( 1.0 ) / static_cast< double >( d );

			Matrix result;

			if ( SIZE == 3 ) {
				result._data[ 0 ] = ( _data[ 4 ] * _data[ 8 ] - _data[ 5 ] * _data[ 7 ] ) * invDet;
				result._data[ 1 ] = ( _data[ 2 ] * _data[ 7 ] - _data[ 1 ] * _data[ 8 ] ) * invDet;
				result._data[ 2 ] = ( _data[ 1 ] * _data[ 5 ] - _data[ 2 ] * _data[ 4 ] ) * invDet;

				result._data[ 3 ] = ( _data[ 5 ] * _data[ 6 ] - _data[ 3 ] * _data[ 8 ] ) * invDet;
				result._data[ 4 ] = ( _data[ 0 ] * _data[ 8 ] - _data[ 2 ] * _data[ 6 ] ) * invDet;
				result._data[ 5 ] = ( _data[ 2 ] * _data[ 3 ] - _data[ 0 ] * _data[ 5 ] ) * invDet;

				result._data[ 6 ] = ( _data[ 3 ] * _data[ 7 ] - _data[ 4 ] * _data[ 6 ] ) * invDet;
				result._data[ 7 ] = ( _data[ 1 ] * _data[ 6 ] - _data[ 0 ] * _data[ 7 ] ) * invDet;
				result._data[ 8 ] = ( _data[ 0 ] * _data[ 4 ] - _data[ 1 ] * _data[ 3 ] ) * invDet;
			}
			else if ( SIZE == 4 ) {
				PRECISION a00 = _data[ 0 ], a01 = _data[ 1 ], a02 = _data[ 2 ], a03 = _data[ 3 ],
        			  	  a10 = _data[ 4 ], a11 = _data[ 5 ], a12 = _data[ 6 ], a13 = _data[ 7 ],
        			  	  a20 = _data[ 8 ], a21 = _data[ 9 ], a22 = _data[ 10 ], a23 = _data[ 11 ],
        			  	  a30 = _data[ 12 ], a31 = _data[ 13 ], a32 = _data[ 14 ], a33 = _data[ 15 ];

        		PRECISION b00 = a00 * a11 - a01 * a10;
        		PRECISION b01 = a00 * a12 - a02 * a10;
        		PRECISION b02 = a00 * a13 - a03 * a10;
        		PRECISION b03 = a01 * a12 - a02 * a11;
        		PRECISION b04 = a01 * a13 - a03 * a11;
        		PRECISION b05 = a02 * a13 - a03 * a12;
        		PRECISION b06 = a20 * a31 - a21 * a30;
        		PRECISION b07 = a20 * a32 - a22 * a30;
        		PRECISION b08 = a20 * a33 - a23 * a30;
        		PRECISION b09 = a21 * a32 - a22 * a31;
        		PRECISION b10 = a21 * a33 - a23 * a31;
        		PRECISION b11 = a22 * a33 - a23 * a32;

        		PRECISION det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

        		assert( det != 0 );

        		PRECISION invDet = 1.0 / det;

			    result._data[ 0 ] = ( a11 * b11 - a12 * b10 + a13 * b09 ) * invDet;
			    result._data[ 1 ] = ( a02 * b10 - a01 * b11 - a03 * b09 ) * invDet;
			    result._data[ 2 ] = ( a31 * b05 - a32 * b04 + a33 * b03 ) * invDet;
			    result._data[ 3 ] = ( a22 * b04 - a21 * b05 - a23 * b03 ) * invDet;
			    result._data[ 4 ] = ( a12 * b08 - a10 * b11 - a13 * b07 ) * invDet;
			    result._data[ 5 ] = ( a00 * b11 - a02 * b08 + a03 * b07 ) * invDet;
			    result._data[ 6 ] = ( a32 * b02 - a30 * b05 - a33 * b01 ) * invDet;
			    result._data[ 7 ] = ( a20 * b05 - a22 * b02 + a23 * b01 ) * invDet;
			    result._data[ 8 ] = ( a10 * b10 - a11 * b08 + a13 * b06 ) * invDet;
			    result._data[ 9 ] = ( a01 * b08 - a00 * b10 - a03 * b06 ) * invDet;
			    result._data[ 10 ] = ( a30 * b04 - a31 * b02 + a33 * b00 ) * invDet;
			    result._data[ 11 ] = ( a21 * b02 - a20 * b04 - a23 * b00 ) * invDet;
			    result._data[ 12 ] = ( a11 * b07 - a10 * b09 - a12 * b06 ) * invDet;
			    result._data[ 13 ] = ( a00 * b09 - a01 * b07 + a02 * b06 ) * invDet;
			    result._data[ 14 ] = ( a31 * b01 - a30 * b03 - a32 * b00 ) * invDet;
			    result._data[ 15 ] = ( a20 * b03 - a21 * b01 + a22 * b00 ) * invDet;				
			}

			return result;
		}

		Matrix &makeInverse( void )
		{
			*this = getInverse();
			return *this;
		}

		void fromXAxisAngle( double angle )
		{
			_data[ 0 ] = 1;
			_data[ 1 ] = 0;
			_data[ 2 ] = 0;
			_data[ 3 ] = 0;
			_data[ 4 ] = static_cast< PRECISION >( cos( angle ) );
			_data[ 5 ] = - static_cast< PRECISION >( sin( angle ) );
			_data[ 6 ] = 0;
			_data[ 7 ] = static_cast< PRECISION >( sin( angle ) );
			_data[ 8 ] = static_cast< PRECISION >( cos( angle ) );
		}

		void fromYAxisAngle( double angle )
		{
			_data[ 0 ] = static_cast< PRECISION >( cos( angle ) );
			_data[ 1 ] = 0;
			_data[ 2 ] = static_cast< PRECISION >( sin( angle ) );
			_data[ 3 ] = 0;
			_data[ 4 ] = 1;
			_data[ 5 ] = 0;
			_data[ 6 ] = - static_cast< PRECISION >( sin( angle ) );
			_data[ 7 ] = 0;
			_data[ 8 ] = static_cast< PRECISION >( cos( angle ) );
		}

		void fromZAxisAngle( double angle )
		{
			_data[ 0 ] = static_cast< PRECISION >( cos( angle ) );
			_data[ 1 ] = - static_cast< PRECISION >( sin( angle ) );
			_data[ 2 ] = 0;
			_data[ 3 ] = static_cast< PRECISION >( sin( angle ) );
			_data[ 4 ] = static_cast< PRECISION >( cos( angle ) );
			_data[ 5 ] = 0;
			_data[ 6 ] = 0;
			_data[ 7 ] = 0;
			_data[ 8 ] = 1;
		}

		void fromAxisAngle( const Vector< 3, PRECISION > &axis, double angle )
		{
			double cosine = std::cos( angle );
			double sine = std::sin( angle );

			_data[ 0 ] = static_cast< PRECISION >( cosine + ( 1 - cosine ) * axis[ 0 ] * axis[ 0 ] );
			_data[ 1 ] = static_cast< PRECISION >( ( 1 - cosine ) * axis[ 0 ] * axis[ 1 ] - sine * axis[ 2 ] );
			_data[ 2 ] = static_cast< PRECISION >( ( 1 - cosine ) * axis[ 0 ] * axis[ 2 ] + sine * axis[ 1 ] );

			_data[ 3 ] = static_cast< PRECISION >( ( 1 - cosine ) * axis[ 0 ] * axis[ 1 ] + sine * axis[ 2 ] );
			_data[ 4 ] = static_cast< PRECISION >( cosine + ( 1 - cosine ) * axis[ 1 ] * axis[ 1 ] );
			_data[ 5 ] = static_cast< PRECISION >( ( 1 - cosine ) * axis[ 1 ] * axis[ 2 ] - sine * axis[ 0 ] );

			_data[ 6 ] = static_cast< PRECISION >( ( 1 - cosine ) * axis[ 0 ] * axis[ 2 ] - sine * axis[ 1 ] );
			_data[ 7 ] = static_cast< PRECISION >( ( 1 - cosine ) * axis[ 1 ] * axis[ 2 ] + sine * axis[ 0 ] );
			_data[ 8 ] = static_cast< PRECISION >( cosine + ( 1 - cosine ) * axis[ 2 ] * axis[ 2 ] );
		}

		Matrix &times( const Matrix &other )
		{
			for ( crimild::Size i = 0; i < SIZE; i++ ) {
				_data[ i ] = _data[ i ] * other._data[ i ];
			}
			return *this;
		}

		template< typename U >
		friend Matrix< SIZE, U > operator-( const Matrix< SIZE, U > &a );

		template< typename U >
		friend Matrix< SIZE, U > operator+( const Matrix< SIZE, U > &a, const Matrix< SIZE, U > &b );

		template< typename U >
		friend Matrix< SIZE, U > operator-( const Matrix< SIZE, U > &a, const Matrix< SIZE, U > &b );

		template< typename U >
		friend Matrix< SIZE, U > operator/( const Matrix< SIZE, U > &a, U scalar );

		template< typename U >
		friend Matrix< SIZE, U > operator*( const Matrix< SIZE, U > &a, const Matrix< SIZE, U > &b );

		template< typename U >
		friend Matrix< SIZE, U > operator*( const Matrix< SIZE, U > &a, U scalar );

		template< typename U >
		friend Matrix< SIZE, U > operator*( U scalar, const Matrix< SIZE, U > &a );

		template< typename U >
		friend Vector< SIZE, U > operator*( const Matrix< SIZE, U > &a, const Vector< SIZE, U > &u );

		template< typename U >
		friend Vector< SIZE, U > operator*( const Vector< SIZE, U > &u, const Matrix< SIZE, U > &a );

		template< typename U >
		friend Matrix< SIZE, U > &operator+=( Matrix< SIZE, U > &a, const Matrix< SIZE, U > &b );

		template< typename U >
		friend Matrix< SIZE, U > &operator-=( Matrix< SIZE, U > &a, const Matrix< SIZE, U > &b );

		template< typename U >
		friend Matrix< SIZE, U > &operator/=( Matrix< SIZE, U > &a, U scalar );

		template< typename U >
		friend Matrix< SIZE, U > &operator*=( Matrix< SIZE, U > &a, U scalar );

		template< typename U >
		friend Matrix< SIZE, U > &operator*=( U scalar, Matrix< SIZE, U > &a );

		template< typename U >
		friend Matrix< SIZE, U > &operator*=( Matrix< SIZE, U > &a, const Matrix< SIZE, U > &b );

	protected:
		PRECISION _data[ SIZE * SIZE ];
	};

	template< typename U >
	Matrix< 3, U > operator-( const Matrix< 3, U > &a )
	{
		Matrix< 3, U > result;
		result._data[ 0 ] = -a._data[ 0 ];
		result._data[ 1 ] = -a._data[ 1 ];
		result._data[ 2 ] = -a._data[ 2 ];
		result._data[ 3 ] = -a._data[ 3 ];
		result._data[ 4 ] = -a._data[ 4 ];
		result._data[ 5 ] = -a._data[ 5 ];
		result._data[ 6 ] = -a._data[ 6 ];
		result._data[ 7 ] = -a._data[ 7 ];
		result._data[ 8 ] = -a._data[ 8 ];
		return result;
	}

	template< typename U >
	Matrix< 3, U > operator+( const Matrix< 3, U > &a, const Matrix< 3, U > &b )
	{
		Matrix< 3, U > result;
		result._data[ 0 ] = a._data[ 0 ] + b._data[ 0 ];
		result._data[ 1 ] = a._data[ 1 ] + b._data[ 1 ];
		result._data[ 2 ] = a._data[ 2 ] + b._data[ 2 ];
		result._data[ 3 ] = a._data[ 3 ] + b._data[ 3 ];
		result._data[ 4 ] = a._data[ 4 ] + b._data[ 4 ];
		result._data[ 5 ] = a._data[ 5 ] + b._data[ 5 ];
		result._data[ 6 ] = a._data[ 6 ] + b._data[ 6 ];
		result._data[ 7 ] = a._data[ 7 ] + b._data[ 7 ];
		result._data[ 8 ] = a._data[ 8 ] + b._data[ 8 ];
		return result;
	}

	template< typename U >
	Matrix< 3, U > operator-( const Matrix< 3, U > &a, const Matrix< 3, U > &b )
	{
		Matrix< 3, U > result;
		result._data[ 0 ] = a._data[ 0 ] - b._data[ 0 ];
		result._data[ 1 ] = a._data[ 1 ] - b._data[ 1 ];
		result._data[ 2 ] = a._data[ 2 ] - b._data[ 2 ];
		result._data[ 3 ] = a._data[ 3 ] - b._data[ 3 ];
		result._data[ 4 ] = a._data[ 4 ] - b._data[ 4 ];
		result._data[ 5 ] = a._data[ 5 ] - b._data[ 5 ];
		result._data[ 6 ] = a._data[ 6 ] - b._data[ 6 ];
		result._data[ 7 ] = a._data[ 7 ] - b._data[ 7 ];
		result._data[ 8 ] = a._data[ 8 ] - b._data[ 8 ];
		return result;
	}

	template< typename U >
	Matrix< 3, U > operator/( const Matrix< 3, U > &a, U scalar )
	{
		assert( scalar != 0 && "Attempting to divide by zero" );

		U invScalar = static_cast< U >( 1 ) / scalar;

		Matrix< 3, U > result;
		result._data[ 0 ] = a._data[ 0 ] * invScalar;
		result._data[ 1 ] = a._data[ 1 ] * invScalar;
		result._data[ 2 ] = a._data[ 2 ] * invScalar;
		result._data[ 3 ] = a._data[ 3 ] * invScalar;
		result._data[ 4 ] = a._data[ 4 ] * invScalar;
		result._data[ 5 ] = a._data[ 5 ] * invScalar;
		result._data[ 6 ] = a._data[ 6 ] * invScalar;
		result._data[ 7 ] = a._data[ 7 ] * invScalar;
		result._data[ 8 ] = a._data[ 8 ] * invScalar;
		return result;
	}

	template< typename U >
	Matrix< 3, U > operator*( const Matrix< 3, U > &a, const Matrix< 3, U > &b )
	{
		Matrix< 3, U > result;
		result[ 0 ] = a._data[ 0 ] * b._data[ 0 ] + a._data[ 1 ] * b._data[ 3 ] + a._data[ 2 ] * b._data[ 6 ];
		result[ 1 ] = a._data[ 0 ] * b._data[ 1 ] + a._data[ 1 ] * b._data[ 4 ] + a._data[ 2 ] * b._data[ 7 ];
		result[ 2 ] = a._data[ 0 ] * b._data[ 2 ] + a._data[ 1 ] * b._data[ 5 ] + a._data[ 2 ] * b._data[ 8 ];
		result[ 3 ] = a._data[ 3 ] * b._data[ 0 ] + a._data[ 4 ] * b._data[ 3 ] + a._data[ 5 ] * b._data[ 6 ];
		result[ 4 ] = a._data[ 3 ] * b._data[ 1 ] + a._data[ 4 ] * b._data[ 4 ] + a._data[ 5 ] * b._data[ 7 ];
		result[ 5 ] = a._data[ 3 ] * b._data[ 2 ] + a._data[ 4 ] * b._data[ 5 ] + a._data[ 5 ] * b._data[ 8 ];
		result[ 6 ] = a._data[ 6 ] * b._data[ 0 ] + a._data[ 7 ] * b._data[ 3 ] + a._data[ 8 ] * b._data[ 6 ];
		result[ 7 ] = a._data[ 6 ] * b._data[ 1 ] + a._data[ 7 ] * b._data[ 4 ] + a._data[ 8 ] * b._data[ 7 ];
		result[ 8 ] = a._data[ 6 ] * b._data[ 2 ] + a._data[ 7 ] * b._data[ 5 ] + a._data[ 8 ] * b._data[ 8 ];
		return result;
	}

	template< typename U >
	Matrix< 3, U > operator*( const Matrix< 3, U > &a, U scalar )
	{
		Matrix< 3, U > result;
		result._data[ 0 ] = a._data[ 0 ] * scalar;
		result._data[ 1 ] = a._data[ 1 ] * scalar;
		result._data[ 2 ] = a._data[ 2 ] * scalar;
		result._data[ 3 ] = a._data[ 3 ] * scalar;
		result._data[ 4 ] = a._data[ 4 ] * scalar;
		result._data[ 5 ] = a._data[ 5 ] * scalar;
		result._data[ 6 ] = a._data[ 6 ] * scalar;
		result._data[ 7 ] = a._data[ 7 ] * scalar;
		result._data[ 8 ] = a._data[ 8 ] * scalar;
		return result;
	}

	template< typename U >
	Matrix< 3, U > operator*( U scalar, const Matrix< 3, U > &a )
	{
		Matrix< 3, U > result;
		result._data[ 0 ] = a._data[ 0 ] * scalar;
		result._data[ 1 ] = a._data[ 1 ] * scalar;
		result._data[ 2 ] = a._data[ 2 ] * scalar;
		result._data[ 3 ] = a._data[ 3 ] * scalar;
		result._data[ 4 ] = a._data[ 4 ] * scalar;
		result._data[ 5 ] = a._data[ 5 ] * scalar;
		result._data[ 6 ] = a._data[ 6 ] * scalar;
		result._data[ 7 ] = a._data[ 7 ] * scalar;
		result._data[ 8 ] = a._data[ 8 ] * scalar;
		return result;
	}

	template< typename U >
	Vector< 3, U > operator*( const Matrix< 3, U > &a, const Vector< 3, U > &u )
	{
		Vector< 3, U > result;
		result[ 0 ] = a._data[ 0 ] * u[ 0 ] + a._data[ 1 ] * u[ 1 ] + a._data[ 2 ] * u[ 2 ];
		result[ 1 ] = a._data[ 3 ] * u[ 0 ] + a._data[ 4 ] * u[ 1 ] + a._data[ 5 ] * u[ 2 ];
		result[ 2 ] = a._data[ 6 ] * u[ 0 ] + a._data[ 7 ] * u[ 1 ] + a._data[ 8 ] * u[ 2 ];
		return result;
	}

	template< typename U >
	Vector< 3, U > operator*( const Vector< 3, U > &u, const Matrix< 3, U > &a )
	{
		Vector< 3, U > result;
		result[ 0 ] = a._data[ 0 ] * u[ 0 ] + a._data[ 1 ] * u[ 1 ] + a._data[ 2 ] * u[ 2 ];
		result[ 1 ] = a._data[ 3 ] * u[ 0 ] + a._data[ 4 ] * u[ 1 ] + a._data[ 5 ] * u[ 2 ];
		result[ 2 ] = a._data[ 6 ] * u[ 0 ] + a._data[ 7 ] * u[ 1 ] + a._data[ 8 ] * u[ 2 ];
		return result;
	}

	template< typename U >
	Matrix< 3, U > &operator+=( Matrix< 3, U > &a, const Matrix< 3, U > &b )
	{
		a._data[ 0 ] += b._data[ 0 ];
		a._data[ 1 ] += b._data[ 1 ];
		a._data[ 2 ] += b._data[ 2 ];
		a._data[ 3 ] += b._data[ 3 ];
		a._data[ 4 ] += b._data[ 4 ];
		a._data[ 5 ] += b._data[ 5 ];
		a._data[ 6 ] += b._data[ 6 ];
		a._data[ 7 ] += b._data[ 7 ];
		a._data[ 8 ] += b._data[ 8 ];
		return a;
	}

	template< typename U >
	Matrix< 3, U > &operator-=( Matrix< 3, U > &a, const Matrix< 3, U > &b )
	{
		a._data[ 0 ] -= b._data[ 0 ];
		a._data[ 1 ] -= b._data[ 1 ];
		a._data[ 2 ] -= b._data[ 2 ];
		a._data[ 3 ] -= b._data[ 3 ];
		a._data[ 4 ] -= b._data[ 4 ];
		a._data[ 5 ] -= b._data[ 5 ];
		a._data[ 6 ] -= b._data[ 6 ];
		a._data[ 7 ] -= b._data[ 7 ];
		a._data[ 8 ] -= b._data[ 8 ];
		return a;
	}

	template< typename U >
	Matrix< 3, U > &operator/=( Matrix< 3, U > &a, U scalar )
	{
		assert( scalar != 0 && "Attempting to divide by zero" );

		U invScalar = static_cast< U >( 1 ) / scalar;

		a._data[ 0 ] *= invScalar;
		a._data[ 1 ] *= invScalar;
		a._data[ 2 ] *= invScalar;
		a._data[ 3 ] *= invScalar;
		a._data[ 4 ] *= invScalar;
		a._data[ 5 ] *= invScalar;
		a._data[ 6 ] *= invScalar;
		a._data[ 7 ] *= invScalar;
		a._data[ 8 ] *= invScalar;
		return a;
	}

	template< typename U >
	Matrix< 3, U > &operator*=( Matrix< 3, U > &a, U scalar )
	{
		a._data[ 0 ] *= scalar;
		a._data[ 1 ] *= scalar;
		a._data[ 2 ] *= scalar;
		a._data[ 3 ] *= scalar;
		a._data[ 4 ] *= scalar;
		a._data[ 5 ] *= scalar;
		a._data[ 6 ] *= scalar;
		a._data[ 7 ] *= scalar;
		a._data[ 8 ] *= scalar;
		return a;
	}

	template< typename U >
	Matrix< 3, U > &operator*=( Matrix< 3, U > &a, const Matrix< 3, U > &b )
	{
		a = a * b;
		return a;
	}

	template< typename U >
	Matrix< 4, U > operator*( const Matrix< 4, U > &a, const Matrix< 4, U > &b )
	{
		Matrix< 4, U > result;
		memset( &result[ 0 ], 0, sizeof( U ) * 16 );

		for ( crimild::Size i = 0; i < 4; i++ ) {
			for ( crimild::Size j = 0; j < 4; j++ ) {
				for ( crimild::Size k = 0; k < 4; k++ ) {
					result[ i * 4 + j ] += a[ i * 4 + k ] * b[ k * 4 + j ];
				}
			}
		}

		return result;
	}

	template< typename U >
	Vector< 4, U > operator*( const Matrix< 4, U > &m, const Vector< 4, U > &v )
	{
		Vector< 4, U > result;
        result[ 0 ] = m[ 0 ] * v[ 0 ] +
                      m[ 1 ] * v[ 1 ] +
                      m[ 2 ] * v[ 2 ] +
                      m[ 3 ] * v[ 3 ];
        result[ 1 ] = m[ 4 ] * v[ 0 ] +
                      m[ 5 ] * v[ 1 ] +
                      m[ 6 ] * v[ 2 ] +
                      m[ 7 ] * v[ 3 ];
        result[ 2 ] = m[ 8 ] * v[ 0 ] +
                      m[ 9 ] * v[ 1 ] +
                      m[ 10 ] * v[ 2 ] +
                      m[ 11 ] * v[ 3 ];
        result[ 3 ] = m[ 12 ] * v[ 0 ] +
                      m[ 13 ] * v[ 1 ] +
                      m[ 14 ] * v[ 2 ] +
                      m[ 15 ] * v[ 3 ];
        
		return result;
	}

	template< typename U >
	Vector< 4, U > operator*( const Vector< 4, U > &v, const Matrix< 4, U > &m )
	{
		Vector< 4, U > result;
        result[ 0 ] = m[ 0 ] * v[ 0 ] +
                      m[ 1 ] * v[ 1 ] +
                      m[ 2 ] * v[ 2 ] +
                      m[ 3 ] * v[ 3 ];
        result[ 1 ] = m[ 4 ] * v[ 0 ] +
                      m[ 5 ] * v[ 1 ] +
                      m[ 6 ] * v[ 2 ] +
                      m[ 7 ] * v[ 3 ];
        result[ 2 ] = m[ 8 ] * v[ 0 ] +
                      m[ 9 ] * v[ 1 ] +
                      m[ 10 ] * v[ 2 ] +
                      m[ 11 ] * v[ 3 ];
        result[ 3 ] = m[ 12 ] * v[ 0 ] +
                      m[ 13 ] * v[ 1 ] +
                      m[ 14 ] * v[ 2 ] +
                      m[ 15 ] * v[ 3 ];
        
		return result;
	}
    
	template< crimild::Size SIZE, typename PRECISION >
	std::ostream &operator<<( std::ostream &out, const Matrix< SIZE, PRECISION > &m )
	{
		out << std::setiosflags( std::ios::fixed | std::ios::showpoint  )
        << std::setprecision( 10 );
		out << "(";
		for ( crimild::Size i = 0; i < SIZE; i++ ) {
            for ( crimild::Size j = 0; j < SIZE; j++ ) {
                out << m[ i * SIZE + j ];
                if ( j < SIZE - 1 || i < SIZE - 1 ) {
                    out << ", ";
                }
            }
            if ( i < SIZE - 1 ) {
                out << "\n";
            }
		}
		out << ")";
		return out;
	}
    
	typedef Matrix< 3, int > Matrix3i;
	typedef Matrix< 3, float > Matrix3f;
	typedef Matrix< 3, double > Matrix3d;

	typedef Matrix< 4, int > Matrix4i;
	typedef Matrix< 4, float > Matrix4f;
	typedef Matrix< 4, double > Matrix4d;

}

#endif

