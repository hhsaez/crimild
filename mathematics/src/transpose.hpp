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

#ifndef CRIMILD_MATHEMATICS_TRANSPOSE_
#define CRIMILD_MATHEMATICS_TRANSPOSE_

#include "Matrix3.hpp"
#include "Matrix4.hpp"

namespace crimild {

    template< ArithmeticType T >
    [[nodiscard]] constexpr auto transpose( const Matrix3Impl< T > &M ) noexcept
    {
        return Matrix3Impl< T > {
            { M[ 0 ][ 0 ], M[ 1 ][ 0 ], M[ 2 ][ 0 ] },
            { M[ 0 ][ 1 ], M[ 1 ][ 1 ], M[ 2 ][ 1 ] },
            { M[ 0 ][ 2 ], M[ 1 ][ 2 ], M[ 2 ][ 2 ] },
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Matrix4Impl< T > transpose( const Matrix4Impl< T > &A ) noexcept
    {
        return Matrix4Impl< T > {
            Vector4Impl< T > { A[ 0 ][ 0 ], A[ 1 ][ 0 ], A[ 2 ][ 0 ], A[ 3 ][ 0 ] },
            Vector4Impl< T > { A[ 0 ][ 1 ], A[ 1 ][ 1 ], A[ 2 ][ 1 ], A[ 3 ][ 1 ] },
            Vector4Impl< T > { A[ 0 ][ 2 ], A[ 1 ][ 2 ], A[ 2 ][ 2 ], A[ 3 ][ 2 ] },
            Vector4Impl< T > { A[ 0 ][ 3 ], A[ 1 ][ 3 ], A[ 2 ][ 3 ], A[ 3 ][ 3 ] }
        };
    }
}

#endif
