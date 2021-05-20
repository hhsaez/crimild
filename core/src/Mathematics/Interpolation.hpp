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

#ifndef CRIMILD_MATHEMATICS_INTERPOLATION_
#define CRIMILD_MATHEMATICS_INTERPOLATION_

#include "Numeric.hpp"
#include "Quaternion.hpp"

namespace crimild {

    /**
		\brief Implements different interpolation calculations

		This class is used to group different interpolation calculations using
		templates which make it useful for several types of objects.

		Check the following URL out to see a visual example of each of the
		easing functions:
		http://www.timotheegroleau.com/Flash/experiments/easing_function_generator.htm
	 */
    class Interpolation {
    public:
        /**
		 	\brief assign the to value to the result

		 	This method is useful for debugging purposes

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void none( const T &from, const T &to, PRECISION t, T &result )
        {
            result = to;
        }

        /**
		 	\brief Implements 'linear' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void linear( const T &from, const T &to, PRECISION t, T &result )
        {
            result = from + t * ( to - from );
        }

        /**
		 	\brief Implements 'cuadratic in' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cuadraticIn( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            result = from + ts * ( to - from );
        }

        /**
		 	\brief Implements 'cubic in' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cubicIn( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION tc = t * t * t;
            result = from + tc * ( to - from );
        }

        /**
		 	\brief Implements 'cubic out' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cubicOut( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = t * t * t;
            result = from + ( tc - 3 * ts + 3 * t ) * ( to - from );
        }

        /**
		 	\brief Implements 'cubic in-out' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cubicInOut( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( -2 * tc + 3 * ts ) * ( to - from );
        }

        /**
		 	\brief Implements 'cubic out-in' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cubicOutIn( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( 4 * tc - 6 * ts + 3 * t ) * ( to - from );
        }

        /**
		 	\brief Implements 'cubic back-in' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cubicBackIn( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( 4 * tc - 3 * ts ) * ( to - from );
        }

        /**
		 	\brief Implements 'cubic back-out' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cubicBackOut( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( 4 * tc - 9 * ts + 6 * t ) * ( to - from );
        }

        /**
		 	\brief Implements 'cuartic in' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cuarticIn( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            result = from + ( ts * ts ) * ( to - from );
        }

        /**
		 	\brief Implements 'cuartic out' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cuarticOut( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( -1 * ts * ts + 4 * tc - 6 * ts + 4 * t ) * ( to - from );
        }

        /**
		 	\brief Implements 'cuartic out-in' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cuarticOutIn( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( 6 * tc - 9 * ts + 4 * t ) * ( to - from );
        }

        /**
		 	\brief Implements 'cuartic back-in' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cuarticBackIn( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( 2 * ts * ts + 2 * tc - 3 * ts ) * ( to - from );
        }

        /**
		 	\brief Implements 'cuartic back-out' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void cuarticBackOut( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( -2 * ts * ts + 10 * tc - 15 * ts + 8 * t ) * ( to - from );
        }

        /**
		 	\brief Implements 'quintic in' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void quinticIn( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( ts * tc ) * ( to - from );
        }

        /**
		 	\brief Implements 'quintic out' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void quinticOut( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( tc * ts - 5 * ts * ts + 10 * tc - 10 * ts + 5 * t ) * ( to - from );
        }

        /**
		 	\brief Implements 'quintic in-out' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void quinticInOut( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( 6 * tc * ts - 15 * ts * ts + 10 * tc ) * ( to - from );
        }

        /**
		 	\brief Implements 'elastic in (small)' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void elasticInSmall( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( 33 * tc * ts - 59 * ts * ts + 32 * tc - 5 * ts ) * ( to - from );
        }

        /**
		 	\brief Implements 'elastic in (big)' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void elasticInBig( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( 56 * tc * ts - 105 * ts * ts + 60 * tc - 10 * ts ) * ( to - from );
        }

        /**
		 	\brief Implements 'elastic out (small)' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void elasticOutSmall( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( 33 * tc * ts - 106 * ts * ts + 126 * tc - 67 * ts + 15 * t ) * ( to - from );
        }

        /**
		 	\brief Implements 'elastic out (big)' interpolation between two values

		 	\remarks Input can be any object type implementing addition and
		 	scalar multiplication, like scalars or vectors.
		 */
        template< typename T, typename PRECISION >
        static void elasticOutBig( const T &from, const T &to, PRECISION t, T &result )
        {
            PRECISION ts = t * t;
            PRECISION tc = ts * t;
            result = from + ( 56 * tc * ts - 175 * ts * ts + 200 * tc - 100 * ts + 20 * t ) * ( to - from );
        }

        template< typename T, typename PRECISION >
        static void sin2Pi( const T &from, const T &to, PRECISION t, T &result )
        {
            T diff = to - from;
            result = std::sin( t * Numericf::TWO_PI ) * diff + from;
        }

        template< typename T, typename PRECISION >
        static void sinPi( const T &from, const T &to, PRECISION t, T &result )
        {
            T diff = to - from;
            result = std::sin( t * Numericf::PI ) * diff + from;
        }

        template< typename T, typename PRECISION >
        static void cos2Pi( const T &from, const T &to, PRECISION t, T &result )
        {
            T diff = to - from;
            result = std::cos( t * Numericf::TWO_PI ) * diff + from;
        }

        template< typename T, typename PRECISION >
        static void cosPi( const T &from, const T &to, PRECISION t, T &result )
        {
            T diff = to - from;
            result = std::cos( t * Numericf::PI ) * diff + from;
        }

        template< typename T, typename PRECISION >
        static void slerp( const impl::Quaternion< T > &from, const impl::Quaternion< T > &to, PRECISION t, impl::Quaternion< T > &result )
        {
            result = slerp( from, to, t );
        }

        /**
			\brief Calculate spherical liner interpolation for two quaternions

			\param q0 Original quaternion. Must be unit length
			\param q1 Destination quaternion. Must be unit length

			\remarks This interpolation requires the use of quaternions only
		 */
        template< typename PRECISION >
        static impl::Quaternion< PRECISION > slerp( const impl::Quaternion< PRECISION > &a, const impl::Quaternion< PRECISION > b, double t )
        {
            PRECISION cosTheta = ( a.getReal() * b.getReal() ) + dot( a.getImaginary(), b.getImaginary() );
            impl::Quaternion< PRECISION > b1 = b;
            if ( cosTheta < 0.0 ) {
                // fix rotation for big angles
                b1 = -b;
                cosTheta = a.getReal() * b1.getReal() + dot( a.getImaginary(), b1.getImaginary() );
            }

            PRECISION w1, w2;
            PRECISION theta = Numeric< PRECISION >::acos( cosTheta );
            PRECISION sinTheta = Numeric< PRECISION >::sin( theta );

            if ( sinTheta > 0.0001 ) {
                w1 = ( PRECISION )( Numeric< PRECISION >::sin( ( 1.0 - t ) * theta ) / sinTheta );
                w2 = ( PRECISION )( Numeric< PRECISION >::sin( t * theta ) / sinTheta );
            } else {
                w1 = 1.0 - t;
                w2 = t;
            }

            impl::Quaternion< PRECISION > result = a * w1 + b1 * w2;
            result.normalize();
            return result;
        }
    };

}

#endif
