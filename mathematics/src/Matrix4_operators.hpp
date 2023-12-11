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

#ifndef CRIMILD_MATHEMATICS_MATRIX_4_OPERATORS_
#define CRIMILD_MATHEMATICS_MATRIX_4_OPERATORS_

#include "Matrix4.hpp"

#include <cassert>

namespace crimild {

    template< typename T >
    [[nodiscard]] inline constexpr Matrix4Impl< T > operator+( const Matrix4Impl< T > &a, const Matrix4Impl< T > &b ) noexcept
    {
        return Matrix4Impl< T > {
            a[ 0 ] + b[ 0 ],
            a[ 1 ] + b[ 1 ],
            a[ 2 ] + b[ 2 ],
            a[ 3 ] + b[ 3 ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Matrix4Impl< T > operator-( const Matrix4Impl< T > &a, const Matrix4Impl< T > &b ) noexcept
    {
        return Matrix4Impl< T > {
            a[ 0 ] - b[ 0 ],
            a[ 1 ] - b[ 1 ],
            a[ 2 ] - b[ 2 ],
            a[ 3 ] - b[ 3 ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Matrix4Impl< T > operator*( const Matrix4Impl< T > &a, real_t s ) noexcept
    {
        return Matrix4Impl< T > {
            a[ 0 ] * s,
            a[ 1 ] * s,
            a[ 2 ] * s,
            a[ 3 ] * s,
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Matrix4Impl< T > operator*( real_t s, const Matrix4Impl< T > &a ) noexcept
    {
        return Matrix4Impl< T > {
            a[ 0 ] * s,
            a[ 1 ] * s,
            a[ 2 ] * s,
            a[ 3 ] * s,
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Matrix4Impl< T > operator/( const Matrix4Impl< T > &a, real_t s ) noexcept
    {
        const auto invS = real_t( 1 ) / s;
        return a * invS;
    }

    template< typename T >
    [[nodiscard]] static constexpr Matrix4Impl< T > operator*( const Matrix4Impl< T > &A, const Matrix4Impl< T > &B ) noexcept
    {
        return Matrix4Impl< T > {
            {
                A[ 0 ][ 0 ] * B[ 0 ][ 0 ] + A[ 1 ][ 0 ] * B[ 0 ][ 1 ] + A[ 2 ][ 0 ] * B[ 0 ][ 2 ] + A[ 3 ][ 0 ] * B[ 0 ][ 3 ],
                A[ 0 ][ 1 ] * B[ 0 ][ 0 ] + A[ 1 ][ 1 ] * B[ 0 ][ 1 ] + A[ 2 ][ 1 ] * B[ 0 ][ 2 ] + A[ 3 ][ 1 ] * B[ 0 ][ 3 ],
                A[ 0 ][ 2 ] * B[ 0 ][ 0 ] + A[ 1 ][ 2 ] * B[ 0 ][ 1 ] + A[ 2 ][ 2 ] * B[ 0 ][ 2 ] + A[ 3 ][ 2 ] * B[ 0 ][ 3 ],
                A[ 0 ][ 3 ] * B[ 0 ][ 0 ] + A[ 1 ][ 3 ] * B[ 0 ][ 1 ] + A[ 2 ][ 3 ] * B[ 0 ][ 2 ] + A[ 3 ][ 3 ] * B[ 0 ][ 3 ],
            },
            {
                A[ 0 ][ 0 ] * B[ 1 ][ 0 ] + A[ 1 ][ 0 ] * B[ 1 ][ 1 ] + A[ 2 ][ 0 ] * B[ 1 ][ 2 ] + A[ 3 ][ 0 ] * B[ 1 ][ 3 ],
                A[ 0 ][ 1 ] * B[ 1 ][ 0 ] + A[ 1 ][ 1 ] * B[ 1 ][ 1 ] + A[ 2 ][ 1 ] * B[ 1 ][ 2 ] + A[ 3 ][ 1 ] * B[ 1 ][ 3 ],
                A[ 0 ][ 2 ] * B[ 1 ][ 0 ] + A[ 1 ][ 2 ] * B[ 1 ][ 1 ] + A[ 2 ][ 2 ] * B[ 1 ][ 2 ] + A[ 3 ][ 2 ] * B[ 1 ][ 3 ],
                A[ 0 ][ 3 ] * B[ 1 ][ 0 ] + A[ 1 ][ 3 ] * B[ 1 ][ 1 ] + A[ 2 ][ 3 ] * B[ 1 ][ 2 ] + A[ 3 ][ 3 ] * B[ 1 ][ 3 ],
            },
            {
                A[ 0 ][ 0 ] * B[ 2 ][ 0 ] + A[ 1 ][ 0 ] * B[ 2 ][ 1 ] + A[ 2 ][ 0 ] * B[ 2 ][ 2 ] + A[ 3 ][ 0 ] * B[ 2 ][ 3 ],
                A[ 0 ][ 1 ] * B[ 2 ][ 0 ] + A[ 1 ][ 1 ] * B[ 2 ][ 1 ] + A[ 2 ][ 1 ] * B[ 2 ][ 2 ] + A[ 3 ][ 1 ] * B[ 2 ][ 3 ],
                A[ 0 ][ 2 ] * B[ 2 ][ 0 ] + A[ 1 ][ 2 ] * B[ 2 ][ 1 ] + A[ 2 ][ 2 ] * B[ 2 ][ 2 ] + A[ 3 ][ 2 ] * B[ 2 ][ 3 ],
                A[ 0 ][ 3 ] * B[ 2 ][ 0 ] + A[ 1 ][ 3 ] * B[ 2 ][ 1 ] + A[ 2 ][ 3 ] * B[ 2 ][ 2 ] + A[ 3 ][ 3 ] * B[ 2 ][ 3 ],
            },
            {
                A[ 0 ][ 0 ] * B[ 3 ][ 0 ] + A[ 1 ][ 0 ] * B[ 3 ][ 1 ] + A[ 2 ][ 0 ] * B[ 3 ][ 2 ] + A[ 3 ][ 0 ] * B[ 3 ][ 3 ],
                A[ 0 ][ 1 ] * B[ 3 ][ 0 ] + A[ 1 ][ 1 ] * B[ 3 ][ 1 ] + A[ 2 ][ 1 ] * B[ 3 ][ 2 ] + A[ 3 ][ 1 ] * B[ 3 ][ 3 ],
                A[ 0 ][ 2 ] * B[ 3 ][ 0 ] + A[ 1 ][ 2 ] * B[ 3 ][ 1 ] + A[ 2 ][ 2 ] * B[ 3 ][ 2 ] + A[ 3 ][ 2 ] * B[ 3 ][ 3 ],
                A[ 0 ][ 3 ] * B[ 3 ][ 0 ] + A[ 1 ][ 3 ] * B[ 3 ][ 1 ] + A[ 2 ][ 3 ] * B[ 3 ][ 2 ] + A[ 3 ][ 3 ] * B[ 3 ][ 3 ],
            },
        };
    }

    template< ArithmeticType T, ArithmeticType U >
    [[nodiscard]] static constexpr auto operator*( const Matrix4Impl< T > &A, const Vector4Impl< U > &B ) noexcept
    {
        return Vector4Impl< decltype( T {} * U {} ) > {
            A[ 0 ][ 0 ] * B[ 0 ] + A[ 1 ][ 0 ] * B[ 1 ] + A[ 2 ][ 0 ] * B[ 2 ] + A[ 3 ][ 0 ] * B[ 3 ],
            A[ 0 ][ 1 ] * B[ 0 ] + A[ 1 ][ 1 ] * B[ 1 ] + A[ 2 ][ 1 ] * B[ 2 ] + A[ 3 ][ 1 ] * B[ 3 ],
            A[ 0 ][ 2 ] * B[ 0 ] + A[ 1 ][ 2 ] * B[ 1 ] + A[ 2 ][ 2 ] * B[ 2 ] + A[ 3 ][ 2 ] * B[ 3 ],
            A[ 0 ][ 3 ] * B[ 0 ] + A[ 1 ][ 3 ] * B[ 1 ] + A[ 2 ][ 3 ] * B[ 2 ] + A[ 3 ][ 3 ] * B[ 3 ],
        };
    }

}

#endif
