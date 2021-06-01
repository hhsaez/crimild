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

#include "Foundation/Macros.hpp"
#include "Foundation/Types.hpp"
#include "Mathematics/ColorRGBAOps.hpp"
#include "Mathematics/Vector3.hpp"

#include <algorithm>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <list>
#include <random>
#include <vector>

namespace crimild {

    class Random {
    public:
        class Generator {
        public:
            Generator( void );
            Generator( crimild::UInt32 seed );
            ~Generator( void );

            crimild::Real64 generate( void );
            crimild::Real64 generate( crimild::Real64 max );
            crimild::Real64 generate( crimild::Real64 min, crimild::Real64 max );

        private:
            std::mt19937_64 _generator;
            std::uniform_real_distribution< double > _distribution;
        };

    public:
        template< typename PRECISION >
        inline static PRECISION generate( void )
        {
            return generate< PRECISION >( 0.0, 1.0 );
        }

        template< typename PRECISION >
        inline static PRECISION generate( double max )
        {
            return generate< PRECISION >( 0.0, max );
        }

        template< typename T >
        inline static T generate( const T &min, const T &max )
        {
            T result;
            generateImpl( result, min, max );
            return result;
        }

    private:
        template< typename T >
        inline static void generateImpl( impl::Vector3< T > &result, const impl::Vector3< T > &min, const impl::Vector3< T > &max ) noexcept
        {
            result = impl::Vector3< T > {
                Random::generate< T >( min[ 0 ], max[ 0 ] ),
                Random::generate< T >( min[ 1 ], max[ 1 ] ),
                Random::generate< T >( min[ 2 ], max[ 2 ] ),
            };
        }

        template< typename PRECISION >
        inline static void generateImpl( PRECISION &result, const PRECISION &min, const PRECISION &max )
        {
            static Random::Generator defaultGenerator;

            crimild::Real64 r = defaultGenerator.generate();
            result = min + r * ( max - min );
        }

    public:
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
