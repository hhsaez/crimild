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

#ifndef CRIMILD_MATHEMATICS_DECOMPOSE_
#define CRIMILD_MATHEMATICS_DECOMPOSE_

#include "Matrix4.hpp"
#include "inverse.hpp"
#include "max.hpp"
#include "transpose.hpp"

namespace crimild {

    constexpr void decompose( const Matrix4 &M, Matrix4 &T, Matrix4 &R, Matrix4 &S ) noexcept
    {
        // Extracting translation is simple. Just get the values form the last column
        T = Matrix4 {
            { 1, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 1, 0 },
            { M[ 3 ][ 0 ], M[ 3 ][ 1 ], M[ 3 ][ 2 ], 1 },
        };

        // Compute new matrix without translation
        // Since transformation is affine, removing translation results in a matrix with
        // only rotation and scaling together.
        const auto RS = Matrix4 {
            { M[ 0 ][ 0 ], M[ 0 ][ 1 ], M[ 0 ][ 2 ], M[ 0 ][ 3 ] },
            { M[ 1 ][ 0 ], M[ 1 ][ 1 ], M[ 1 ][ 2 ], M[ 1 ][ 3 ] },
            { M[ 2 ][ 0 ], M[ 2 ][ 1 ], M[ 2 ][ 2 ], M[ 1 ][ 3 ] },
            { 0, 0, 0, 1 },
        };

        // Extract rotation from RS by using polar decomposition, successively averaging RS
        // with its inverse transpose. This is a fairly complex process, but most of the time
        // convergence should only require a few loops.
        R = RS;
        real_t norm = 0;
        int count = 0;
        do {
            // Compute next matrix in the series
            auto next = Matrix4 {};
            Matrix4 current = inverse( transpose( R ) );
            for ( int i = 0; i < 4; ++i ) {
                for ( int j = 0; j < 4; ++j ) {
                    next[ i ][ j ] = 0.5 * ( R[ i ][ j ] + current[ i ][ j ] );
                }
            }

            // Compute norm of difference between R and next
            norm = 0;
            for ( int i = 0; i < 3; ++i ) {
                real_t n = abs( R[ i ][ 0 ] - next[ i ][ 0 ] )
                           + abs( R[ i ][ 0 ] - next[ i ][ 0 ] )
                           + abs( R[ i ][ 0 ] - next[ i ][ 0 ] );
                norm = max( norm, n );
            }

            R = next;
        } while ( ++count < 100 && norm > 0.001 );

        // Finally, the scalling matrix is computed by simply removing the rotation part from RS
        // matrix using the inverse operation
        S = inverse( R ) * RS;
    }

}

#endif
