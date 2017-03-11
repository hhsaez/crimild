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

#ifndef CRIMILD_MATHEMATICS_RANDOM_
#define CRIMILD_MATHEMATICS_RANDOM_

#include "Vector.hpp"

#include "Foundation/Macros.hpp"
#include "Foundation/Types.hpp"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <random>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <chrono>

namespace crimild {

	class Random {
	public:
		static void configure( void )
		{
			srand( time( 0 ) );
		}
		
		template< typename PRECISION >
		static PRECISION generate( void )
		{
            return generate< PRECISION >( 0.0, 1.0 );
		}

		template< typename PRECISION >
		static PRECISION generate( double max )
		{
            return generate< PRECISION >( 0.0, max );
		}

		template< crimild::Size SIZE, typename PRECISION >
		inline Vector< SIZE, PRECISION > generate( const Vector< SIZE, PRECISION > &min, const Vector< SIZE, PRECISION > &max )
		{
			if ( SIZE == 2 ) {
				return Vector< SIZE, PRECISION >(
					Random::generate< PRECISION >( min.x(), max.x() ),
					Random::generate< PRECISION >( min.y(), max.y() ) );
			}
			else if ( SIZE == 3 ) {
				return Vector< SIZE, PRECISION >(
					Random::generate< PRECISION >( min.x(), max.x() ),
					Random::generate< PRECISION >( min.y(), max.y() ),
					Random::generate< PRECISION >( min.z(), max.z() ) );
			}
			else if ( SIZE == 4 ) {
				return Vector< SIZE, PRECISION >(
					Random::generate< PRECISION >( min.x(), max.x() ),
					Random::generate< PRECISION >( min.y(), max.y() ),
					Random::generate< PRECISION >( min.z(), max.z() ),
					Random::generate< PRECISION >( min.w(), max.w() ) );
			}
			
			Vector< SIZE, PRECISION > result;
			for ( crimild::Size i = 0; i < SIZE; i++ ) {
				result[ i ] = Random::generate< PRECISION >( min[ i ], max[ i ] );
			}
			return result;
		}

		template< typename PRECISION >
		static PRECISION generate( const PRECISION &min, const PRECISION &max )
		{
			crimild::Real64 r = 0.01 * ( std::rand() % 100 );
            return min + r * ( max - min );
        }
        
        template< class T >
        static void shuffle( std::vector< T > &input )
        {
            auto seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::shuffle( input.begin(), input.end(), std::default_random_engine( seed ) );
        }
        
        template< class T >
        static void shuffle( std::list< T > &input )
        {
            std::vector< T > temp;
            std::copy( input.begin(), input.end(), std::back_inserter( temp ) );
            
            shuffle( temp );
            
            input.clear();
            std::copy( temp.begin(), temp.end(), std::back_inserter( input ) );
        }

	};

}

#endif

