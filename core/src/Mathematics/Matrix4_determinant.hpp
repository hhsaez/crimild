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

#ifndef CRIMILD_MATHEMATICS_MATRIX_4_DETERMINANT_
#define CRIMILD_MATHEMATICS_MATRIX_4_DETERMINANT_

#include "Mathematics/Matrix4.hpp"

namespace crimild {

    template< typename T >
    [[nodiscard]] constexpr Real determinant( const Matrix4Impl< T > &a ) noexcept
    {
        const auto m11 = a[ 0 ][ 0 ];
        const auto m12 = a[ 1 ][ 0 ];
        const auto m13 = a[ 2 ][ 0 ];
        const auto m14 = a[ 3 ][ 0 ];

        const auto m21 = a[ 0 ][ 1 ];
        const auto m22 = a[ 1 ][ 1 ];
        const auto m23 = a[ 2 ][ 1 ];
        const auto m24 = a[ 3 ][ 1 ];

        const auto m31 = a[ 0 ][ 2 ];
        const auto m32 = a[ 1 ][ 2 ];
        const auto m33 = a[ 2 ][ 2 ];
        const auto m34 = a[ 3 ][ 2 ];

        const auto m41 = a[ 0 ][ 3 ];
        const auto m42 = a[ 1 ][ 3 ];
        const auto m43 = a[ 2 ][ 3 ];
        const auto m44 = a[ 3 ][ 3 ];

        return m11 * ( m22 * ( m33 * m44 - m34 * m43 ) + m23 * ( m34 * m42 - m32 * m44 ) + m24 * ( m32 * m43 - m33 * m42 ) )
               - m12 * ( m21 * ( m33 * m44 - m34 * m43 ) + m23 * ( m34 * m41 - m31 * m44 ) + m24 * ( m31 * m43 - m33 * m41 ) )
               + m13 * ( m21 * ( m32 * m44 - m34 * m42 ) + m22 * ( m34 * m41 - m31 * m44 ) + m24 * ( m31 * m42 - m32 * m41 ) )
               - m14 * ( m21 * ( m32 * m43 - m33 * m42 ) + m22 * ( m33 * m41 - m31 * m43 ) + m23 * ( m31 * m42 - m32 * m41 ) );
    }

}

#endif
