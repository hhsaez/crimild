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

#ifndef CRIMILD_MATHEMATICS_MATRIX_3_INVERSE_
#define CRIMILD_MATHEMATICS_MATRIX_3_INVERSE_

#include "Matrix3.hpp"

namespace crimild {

    template< typename T >
    [[nodiscard]] constexpr Matrix3Impl< T > inverse( const Matrix3Impl< T > &a ) noexcept
    {
        const auto det = determinant( a );
        assert( det != 0 );
        const auto invDet = Real( 1.0 ) / det;
        return Matrix3Impl< T > {
            ( a[ 4 ] * a[ 8 ] - a[ 5 ] * a[ 7 ] ) * invDet,
            ( a[ 2 ] * a[ 7 ] - a[ 1 ] * a[ 8 ] ) * invDet,
            ( a[ 1 ] * a[ 5 ] - a[ 2 ] * a[ 4 ] ) * invDet,
            ( a[ 5 ] * a[ 6 ] - a[ 3 ] * a[ 8 ] ) * invDet,
            ( a[ 0 ] * a[ 8 ] - a[ 2 ] * a[ 6 ] ) * invDet,
            ( a[ 2 ] * a[ 3 ] - a[ 0 ] * a[ 5 ] ) * invDet,
            ( a[ 3 ] * a[ 7 ] - a[ 4 ] * a[ 6 ] ) * invDet,
            ( a[ 1 ] * a[ 6 ] - a[ 0 ] * a[ 7 ] ) * invDet,
            ( a[ 0 ] * a[ 4 ] - a[ 1 ] * a[ 3 ] ) * invDet,
        };
    }

}

#endif
