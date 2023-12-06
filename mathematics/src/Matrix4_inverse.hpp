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

#ifndef CRIMILD_MATHEMATICS_MATRIX_4_INVERSE_
#define CRIMILD_MATHEMATICS_MATRIX_4_INVERSE_

#include "Matrix4.hpp"

#include <cassert>

namespace crimild {

    template< typename T >
    [[nodiscard]] constexpr Matrix4Impl< T > inverse( const Matrix4Impl< T > &a ) noexcept
    {
        const T a00 = a[ 0 ][ 0 ], a01 = a[ 1 ][ 0 ], a02 = a[ 2 ][ 0 ], a03 = a[ 3 ][ 0 ],
                a10 = a[ 0 ][ 1 ], a11 = a[ 1 ][ 1 ], a12 = a[ 2 ][ 1 ], a13 = a[ 3 ][ 1 ],
                a20 = a[ 0 ][ 2 ], a21 = a[ 1 ][ 2 ], a22 = a[ 2 ][ 2 ], a23 = a[ 3 ][ 2 ],
                a30 = a[ 0 ][ 3 ], a31 = a[ 1 ][ 3 ], a32 = a[ 2 ][ 3 ], a33 = a[ 3 ][ 3 ];

        const T b00 = a00 * a11 - a01 * a10;
        const T b01 = a00 * a12 - a02 * a10;
        const T b02 = a00 * a13 - a03 * a10;
        const T b03 = a01 * a12 - a02 * a11;
        const T b04 = a01 * a13 - a03 * a11;
        const T b05 = a02 * a13 - a03 * a12;
        const T b06 = a20 * a31 - a21 * a30;
        const T b07 = a20 * a32 - a22 * a30;
        const T b08 = a20 * a33 - a23 * a30;
        const T b09 = a21 * a32 - a22 * a31;
        const T b10 = a21 * a33 - a23 * a31;
        const T b11 = a22 * a33 - a23 * a32;

        const Real det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
        assert( det != 0 );
        const Real invDet = Real( 1 ) / det;

        return Matrix4Impl< T > {
            {
                ( a11 * b11 - a12 * b10 + a13 * b09 ) * invDet,
                ( a12 * b08 - a10 * b11 - a13 * b07 ) * invDet,
                ( a10 * b10 - a11 * b08 + a13 * b06 ) * invDet,
                ( a11 * b07 - a10 * b09 - a12 * b06 ) * invDet,
            },
            {
                ( a02 * b10 - a01 * b11 - a03 * b09 ) * invDet,
                ( a00 * b11 - a02 * b08 + a03 * b07 ) * invDet,
                ( a01 * b08 - a00 * b10 - a03 * b06 ) * invDet,
                ( a00 * b09 - a01 * b07 + a02 * b06 ) * invDet,
            },
            {
                ( a31 * b05 - a32 * b04 + a33 * b03 ) * invDet,
                ( a32 * b02 - a30 * b05 - a33 * b01 ) * invDet,
                ( a30 * b04 - a31 * b02 + a33 * b00 ) * invDet,
                ( a31 * b01 - a30 * b03 - a32 * b00 ) * invDet,
            },
            {
                ( a22 * b04 - a21 * b05 - a23 * b03 ) * invDet,
                ( a20 * b05 - a22 * b02 + a23 * b01 ) * invDet,
                ( a21 * b02 - a20 * b04 - a23 * b00 ) * invDet,
                ( a20 * b03 - a21 * b01 + a22 * b00 ) * invDet,
            },
        };
    }

}

#endif
