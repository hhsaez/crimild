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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_

#include "Vector.hpp"
#include "Quaternion.hpp"
#include "Matrix.hpp"
#include "Plane.hpp"
#include "Numeric.hpp"

namespace Crimild {

	template< typename PRECISION >
	class Transformation {
	public:
		typedef Vector< 3, PRECISION > Vector3Impl;
		typedef Matrix< 3, PRECISION > Matrix3Impl;
		typedef Matrix< 4, PRECISION > Matrix4Impl;
		typedef Quaternion< PRECISION > QuaternionImpl;
		typedef Plane< 3, PRECISION > PlaneImpl;

	public:
		Transformation( void )
		{
			makeIdentity();
		}

		explicit Transformation( const Vector3Impl &v )
		{
			makeIdentity();
			setTranslate( v );
		}

		explicit Transformation( const QuaternionImpl &q )
		{
			makeIdentity();
			setRotate( q );
		}

		explicit Transformation( PRECISION s )
		{
			makeIdentity();
			setScale( s );
		}

		Transformation( const Vector3Impl &t, const QuaternionImpl &r, PRECISION s )
		{
			setTranslate( t );
			setRotate( r );
			setScale( s );
		}

		Transformation( const Transformation &t0, const Transformation &t1 )
		{
			computeFrom( t0, t1 );
		}

		Transformation( const Transformation &t )
		{
			_translate = t._translate;
			_scale = t._scale;
			_rotate = t._rotate;
			_isIdentity = t._isIdentity;
		}

		~Transformation( void )
		{

		}

		Transformation &operator=( const Transformation &t )
		{
			_translate = t._translate;
			_scale = t._scale;
			_rotate = t._rotate;
			_isIdentity = t._isIdentity;
			return *this;
		}

		void setTranslate( const Vector3Impl &v )
		{
			_translate = v;
			_isIdentity = false;
		}

		void setTranslate( PRECISION x, PRECISION y, PRECISION z )
		{
			_translate[ 0 ] = x;
			_translate[ 1 ] = y;
			_translate[ 2 ] = z;
			_isIdentity = false;
		}

		const Vector3Impl &getTranslate( void ) const 
		{ 
			return _translate; 
		}
		
		Vector3Impl &translate( void )
		{
			_isIdentity = false;
			return _translate;
		}

		void setRotate( const QuaternionImpl &q )
		{
			_rotate = q;
			_isIdentity = false;
		}

		void setRotate( const Vector3Impl &axis, PRECISION angle )
		{
			_rotate.fromAxisAngle( axis, angle );
			_isIdentity = false;
		}

		const QuaternionImpl &getRotate( void ) const 
		{ 
			return _rotate; 
		}

		QuaternionImpl &rotate( void )
		{
			_isIdentity = false;
			return _rotate;
		}

		void setScale( PRECISION s )
		{
			_scale = s;
		}

		PRECISION getScale( void ) const 
		{ 
			return _scale; 
		}
		
		PRECISION &scale( void )
		{
			_isIdentity = false;
			return _scale;
		}

		void makeIdentity( void )
		{
			_translate = Vector3Impl( 0, 0, 0 );
			_rotate.makeIdentity();
			_scale = 1;
			_isIdentity = true;
		}

		bool isIdentity( void ) const 
		{ 
			return _isIdentity; 
		}

		void applyToPoint( const Vector3Impl &input, Vector3Impl &output ) const
		{
			output = _rotate * ( _scale * input ) + _translate;
		}

		void applyInverseToPoint( const Vector3Impl &input, Vector3Impl &output ) const
		{
			output = ( _rotate.getInverse() * ( input - _translate ) ) / _scale;
		}

		void applyToVector( const Vector3Impl &input, Vector3Impl &output ) const
		{
			output = _rotate * ( _scale * input );
		}

		void applyInverseToVector( const Vector3Impl &input, Vector3Impl &output ) const
		{
			output = ( _rotate.getInverse() * ( input ) ) / _scale;
		}

		void applyToUnitVector( const Vector3Impl &input, Vector3Impl &output ) const
		{
			output = _rotate * input;
		}

		void applyInverseToUnitVector( const Vector3Impl &input, Vector3Impl &output ) const
		{
			output = _rotate.getInverse() * input;
		}

		void applyToPlane( const PlaneImpl &input, PlaneImpl &output ) const
		{

		}

		void applyInverseToPlane( const PlaneImpl &input, PlaneImpl &output ) const
		{

		}

		void computeFrom( const Transformation &a, const Transformation &b )
		{
			if ( a.isIdentity() ) {
				*this = b;
			}
			else if ( b.isIdentity() ) {
				*this = a;
			}
			else {
				_translate = a._translate + a._rotate * ( a._scale * b._translate );
				_rotate = a._rotate * b._rotate;
				_scale = a._scale * b._scale;
				_isIdentity = false;
			}
		}

		Vector3Impl computeDirection( void ) const
		{
			Vector3Impl result;
			applyToVector( Vector3Impl( 0, 0, -1 ), result );
			return result;
		}

		Vector3Impl computeDirection( Vector3Impl &result ) const
		{
			applyToVector( Vector3Impl( 0, 0, -1 ), result );
			return result;
		}

		Vector3Impl computeUp( void ) const
		{
			Vector3Impl result;
			applyToVector( Vector3Impl( 0, 1, 0 ), result );
			return result;
		}

		Vector3Impl computeUp( Vector3Impl &result ) const
		{
			applyToVector( Vector3Impl( 0, 1, 0 ), result );
			return result;
		}

		Vector3Impl computeRight( void ) const
		{
			Vector3Impl result;
			applyToVector( Vector3Impl( 1, 0, 0 ), result );
			return result;
		}

		Vector3Impl computeRight( Vector3Impl &result ) const
		{
			applyToVector( Vector3Impl( 1, 0, 0 ), result );
			return result;
		}

		void lookAt( const Vector3Impl &targetPosition, const Vector3Impl &upReference )
		{
		}
        
        Transformation &fromMatrix( const Matrix4Impl &m )
        {
            Matrix3f viewRotation;
            viewRotation[ 0 ] = m[ 0 ];
            viewRotation[ 1 ] = m[ 1 ];
            viewRotation[ 2 ] = m[ 2 ];
            viewRotation[ 3 ] = m[ 4 ];
            viewRotation[ 4 ] = m[ 5 ];
            viewRotation[ 5 ] = m[ 6 ];
            viewRotation[ 6 ] = m[ 8 ];
            viewRotation[ 7 ] = m[ 9 ];
            viewRotation[ 8 ] = m[ 10 ];
            
            rotate().fromRotationMatrix( viewRotation );
            setTranslate( m[ 12 ], m[ 13 ], m[ 14 ] );

            return *this;
        }

		Matrix< 4, float > computeModelMatrix( void ) const
		{
			Matrix< 4, float > result;
			if ( _isIdentity ) {
				result.makeIdentity();
			}
			else {
				float x = getRotate().getImaginary()[0];
				float y = getRotate().getImaginary()[1];
				float z = getRotate().getImaginary()[2];
				float w = getRotate().getReal();
				float s = getScale();

		        float x2 = x + x;
		        float y2 = y + y;
		        float z2 = z + z;

		        float xx = x * x2;
		        float xy = x * y2;
		        float xz = x * z2;
		        float yy = y * y2;
		        float yz = y * z2;
		        float zz = z * z2;
		        float wx = w * x2;
		        float wy = w * y2;
		        float wz = w * z2;

		    	result[0] = _scale * ( s * ( 1 - (yy + zz) ) );
		    	result[1] = _scale * ( s * ( xy + wz ) );
		    	result[2] = _scale * ( s * ( xz - wy ) );
		    	result[3] = 0;

			    result[4] = s * ( xy - wz );
			    result[5] = s * ( 1 - (xx + zz) );
		    	result[6] = s * ( yz + wx );
		    	result[7] = 0;

		    	result[8] = s * ( xz + wy );
		    	result[9] = s * ( yz - wx );
		    	result[10] = s * ( 1 - (xx + yy) );
		    	result[11] = 0;

				result[ 12 ] = getTranslate()[ 0 ];
				result[ 13 ] = getTranslate()[ 1 ];
				result[ 14 ] = getTranslate()[ 2 ];
		    	result[15] = 1;
			}

			return result;
		}

	private:
		Vector3Impl _translate;
		QuaternionImpl _rotate;
		PRECISION _scale;
		bool _isIdentity;
	};

	typedef Transformation< float > TransformationImpl;

}

#endif

