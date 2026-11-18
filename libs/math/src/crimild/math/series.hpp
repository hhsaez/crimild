/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_MATHEMATICS_SERIES_
#define CRIMILD_MATHEMATICS_SERIES_

#include "Vector3.hpp"

#include <vector>

namespace crimild {

    template< typename T >
    static std::vector< T > fibonacci( size_t N ) noexcept
    {
        std::vector< T > ret( N );
        T a = 0;
        T b = 1;
        for ( size_t i = 0; i < N; ++i ) {
            if ( i < 2 ) {
                ret[ i ] = i;
            } else {
                ret[ i ] = a + b;
                a = b;
                b = ret[ i ];
            }
        }
        return ret;
    }

    static auto fibonacciSquares( size_t N ) noexcept
    {
        auto ret = std::vector< std::pair< Vector3, float > >( N );
        real_t s0 = 1.0f;
        real_t s1 = 1.0f;
        Vector3 c0 = Vector3::Constants::ZERO;
        Vector3 c1 = -Vector3::Constants::UNIT_X;
        const auto OFFSET = std::array< Vector3, 4 > {
            -Vector3::Constants::UNIT_Y,
            -Vector3::Constants::UNIT_X,
            Vector3::Constants::UNIT_Y,
            Vector3::Constants::UNIT_X,
        };
        for ( size_t i = 0; i < N; ++i ) {

            if ( i == 0 ) {
                ret[ i ] = std::make_pair( c0, 1.0f );
            } else if ( i == 1 ) {
                ret[ i ] = std::make_pair( c1, 1.0f );
            } else {
                auto s = s0 + s1;
                s0 = s1;
                s1 = s;

                auto c = c1 + 0.5f * ( s0 + s1 ) * OFFSET[ i % 4 ] + 0.5f * ( s1 - s0 ) * OFFSET[ ( i + 1 ) % 4 ];
                c0 = c1;
                c1 = c;
                ret[ i ] = std::make_pair( c, s );
            }
        }

        return ret;
    }

}

#endif
