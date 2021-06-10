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

#include "Foundation/Containers/Array.hpp"
#include "Foundation/Types.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector3_constants.hpp"

namespace crimild {

    namespace math {

        template< typename Number >
        static Array< Number > fibonacci( Size N ) noexcept
        {
            return Array< Number >( N ).fill(
                [ a = Number( 0 ),
                  b = Number( 1 ) ]( auto i ) mutable {
                    if ( i < 2 ) {
                        return Number( i );
                    }
                    auto c = a + b;
                    auto ret = c;
                    a = b;
                    b = c;
                    return ret;
                } );
        }

        static Array< std::pair< Vector3f, Real32 > > fibonacciSquares( Size N ) noexcept
        {
            return Array< std::pair< Vector3f, Real32 > >( N ).fill(
                [ s0 = 1.0f,
                  s1 = 1.0f,
                  c0 = Vector3f::Constants::ZERO,
                  c1 = -Vector3f::Constants::UNIT_X ]( auto i ) mutable -> std::pair< Vector3f, Real32 > {
                    const auto OFFSET = Array< Vector3f > {
                        -Vector3f::Constants::UNIT_Y,
                        -Vector3f::Constants::UNIT_X,
                        Vector3f::Constants::UNIT_Y,
                        Vector3f::Constants::UNIT_X,
                    };

                    if ( i == 0 ) {
                        return std::make_pair( c0, 1.0f );
                    } else if ( i == 1 ) {
                        return std::make_pair( c1, 1.0f );
                    }

                    auto s = s0 + s1;
                    s0 = s1;
                    s1 = s;

                    auto c = c1 + 0.5f * ( s0 + s1 ) * OFFSET[ i % 4 ] + 0.5f * ( s1 - s0 ) * OFFSET[ ( i + 1 ) % 4 ];
                    c0 = c1;
                    c1 = c;
                    return std::make_pair( c, s );
                } );
        }

    }

}

#endif
