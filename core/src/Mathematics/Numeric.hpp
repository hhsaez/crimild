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

#ifndef CRIMILD_MATHEMATICS_NUMERIC_
#define CRIMILD_MATHEMATICS_NUMERIC_

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <random>

namespace crimild {
    
    template< typename PRECISION >
    class Numeric {
    public:
        static const PRECISION ZERO_TOLERANCE;
        static const PRECISION PI;
        static const PRECISION HALF_PI;
        static const PRECISION TWO_PI;
        static const PRECISION SQRT_TWO;
        static const PRECISION SQRT_TWO_DIV_TWO;
        
        static const PRECISION DEG_TO_RAD;
        static const PRECISION RAD_TO_DEG;
        
        static const PRECISION COS_0;
        static const PRECISION COS_45;
        static const PRECISION COS_90;
        static const PRECISION COS_135;
        static const PRECISION COS_180;
        
        static bool isZero( PRECISION number )
        {
            return ( std::fabs( number ) <= ZERO_TOLERANCE );
        }
        
        static bool equals( PRECISION x, PRECISION y )
        {
            return ( std::fabs( x - y ) <= ZERO_TOLERANCE );
        }
        
        static bool notEqual( PRECISION x, PRECISION y )
        {
            return !equal( x, y );
        }
        
        static bool less( PRECISION x, PRECISION y )
        {
            return x < y;
        }
        
        static bool lEqual( PRECISION x, PRECISION y )
        {
            return x <= y;
        }
        
        static bool greater( PRECISION x, PRECISION y )
        {
            return x > y;
        }
        
        static bool gEqual( PRECISION x, PRECISION y )
        {
            return x >= y;
        }
        
        static PRECISION max( PRECISION x, PRECISION y )
        {
            return ( x > y ? x : y );
        }
        
        static PRECISION min( PRECISION x, PRECISION y )
        {
            return ( x < y ? x : y );
        }
        
        static PRECISION squared( PRECISION x )
        {
            return ( x * x );
        }
        
        static PRECISION pow( PRECISION x, PRECISION e )
        {
            return std::pow( x, e );
        }
        
        static PRECISION exp( PRECISION x )
        {
            return ::exp( x );
        }
        
        static PRECISION noiseFunc( PRECISION reference )
        {
            return sqrt( -2 * log( ( PRECISION ) std::rand() / RAND_MAX ) ) * sin( 2 * PI * ( PRECISION ) std::rand() / RAND_MAX ) * reference;
        }
        
        static PRECISION clamp( PRECISION input )
        {
            return clamp( input, 0, 1 );
        }
        
        static PRECISION clamp( PRECISION input, PRECISION min, PRECISION max )
        {
            PRECISION result = input;
            if ( result > max ) {
                result = max;
            }
            if ( result < min ) {
                result = min;
            }
            return result;
        }
        
        static int sign( PRECISION input )
        {
            return input < 0 ? -1 : 1;
        }
        
        static PRECISION fract( PRECISION x )
        {
            return x - ( long ) x;
        }
        
        static PRECISION roundInc( PRECISION n, PRECISION max, PRECISION step = 1 )
        {
            return ( n + step ) % max;
        }
        
        static PRECISION roundDec( PRECISION n, PRECISION max, PRECISION step = -1 )
        {
            return ( n + step + max ) % max;
        }
        
        static PRECISION cos( double angle )
        {
            return static_cast< PRECISION >( std::cos( angle ) );
        }
        
        static PRECISION acos( double value )
        {
            return static_cast< PRECISION >( std::acos( value ) );
        }
        
        static PRECISION sin( double angle )
        {
            return static_cast< PRECISION >( std::sin( angle ) );
        }
        
        static PRECISION tan( double angle )
        {
            return static_cast< PRECISION >( std::tan( angle ) );
        }
        
        static PRECISION atan( double angle )
        {
            return static_cast< PRECISION >( std::atan( angle ) );
        }
        
        static void swap( PRECISION &a, PRECISION &b )
        {
            PRECISION temp = a;
            a = b;
            b = temp;
        }
        
        static PRECISION sqrt( double n )
        {
            return std::sqrt( n );
        }
        
        static PRECISION fabs( PRECISION n )
        {
            return std::fabs( n );
        }
        
        static PRECISION log( PRECISION n )
        {
            return ( PRECISION ) ::log( n );
        }
        
        static PRECISION log2( PRECISION n )
        {
            return ( PRECISION ) ::log2( n );
        }
        
        static PRECISION ceil( double n )
        {
            return ( PRECISION ) ::ceil( n );
        }
    };
    
    template< typename T > const T Numeric< T >::ZERO_TOLERANCE = static_cast< T >( 1e-06 );
    template< typename T > const T Numeric< T >::PI = static_cast< T >( 3.1415926535897932384626433832795 );
    template< typename T > const T Numeric< T >::HALF_PI = static_cast< T >( 3.1415926535897932384626433832795 / 2.0 );
    template< typename T > const T Numeric< T >::TWO_PI = static_cast< T >( 3.1415926535897932384626433832795 * 2 );
    template< typename T > const T Numeric< T >::SQRT_TWO = static_cast< T >( std::sqrt( 2 ) );
    template< typename T > const T Numeric< T >::SQRT_TWO_DIV_TWO = static_cast< T >( 0.5 * Numeric< double >::sqrt( 2 ) );
    
    template< typename T > const T Numeric< T >::DEG_TO_RAD = static_cast< T >( 3.1415926535897932384626433832795 / 180.0 );
    template< typename T > const T Numeric< T >::RAD_TO_DEG = static_cast< T >( 180.0 / 3.1415926535897932384626433832795 );
    
    template< typename T > const T Numeric< T >::COS_0 = static_cast< T >( 1.0 );
    template< typename T > const T Numeric< T >::COS_45 = static_cast< T >( 0.5 * Numeric< double >::sqrt( 2.0 ) );
    template< typename T > const T Numeric< T >::COS_90 = static_cast< T >( 0.0 );
    template< typename T > const T Numeric< T >::COS_135 = static_cast< T >( -0.5 * Numeric< double >::sqrt( 2.0 ) );
    template< typename T > const T Numeric< T >::COS_180 = static_cast< T >( -1.0 );
    
    typedef Numeric< int > Numerici;
    typedef Numeric< float > Numericf;
    typedef Numeric< double > Numericd;
    
}

#endif

