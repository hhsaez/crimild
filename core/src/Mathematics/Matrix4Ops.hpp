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

#ifndef CRIMILD_MATHEMATICS_MATRIX_4_OPS_
#define CRIMILD_MATHEMATICS_MATRIX_4_OPS_

#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Vector4.hpp"

#include <cassert>

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix4< T > operator+( const crimild::impl::Matrix4< T > &a, const crimild::impl::Matrix4< T > &b ) noexcept
{
    return crimild::impl::Matrix4< T > {
        a[ 0 ] + b[ 0 ],
        a[ 1 ] + b[ 1 ],
        a[ 2 ] + b[ 2 ],
        a[ 3 ] + b[ 3 ],
        a[ 4 ] + b[ 4 ],
        a[ 5 ] + b[ 5 ],
        a[ 6 ] + b[ 6 ],
        a[ 7 ] + b[ 7 ],
        a[ 8 ] + b[ 8 ],
        a[ 9 ] + b[ 9 ],
        a[ 10 ] + b[ 10 ],
        a[ 11 ] + b[ 11 ],
        a[ 12 ] + b[ 12 ],
        a[ 13 ] + b[ 13 ],
        a[ 14 ] + b[ 14 ],
        a[ 15 ] + b[ 15 ],
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix4< T > operator-( const crimild::impl::Matrix4< T > &a, const crimild::impl::Matrix4< T > &b ) noexcept
{
    return crimild::impl::Matrix4< T > {
        a[ 0 ] - b[ 0 ],
        a[ 1 ] - b[ 1 ],
        a[ 2 ] - b[ 2 ],
        a[ 3 ] - b[ 3 ],
        a[ 4 ] - b[ 4 ],
        a[ 5 ] - b[ 5 ],
        a[ 6 ] - b[ 6 ],
        a[ 7 ] - b[ 7 ],
        a[ 8 ] - b[ 8 ],
        a[ 9 ] - b[ 9 ],
        a[ 10 ] - b[ 10 ],
        a[ 11 ] - b[ 11 ],
        a[ 12 ] - b[ 12 ],
        a[ 13 ] - b[ 13 ],
        a[ 14 ] - b[ 14 ],
        a[ 15 ] - b[ 15 ],
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix4< T > operator*( const crimild::impl::Matrix4< T > &a, const crimild::impl::Matrix4< T > &b ) noexcept
{
    // TODO: unroll these loops
    T res[ 16 ] = { 0 };
    for ( auto i = 0l; i < 4; ++i ) {
        for ( auto j = 0l; j < 4; ++j ) {
            for ( auto k = 0l; k < 4; ++k ) {
                res[ i * 4 + j ] += a[ i * 4 + k ] * b[ k * 4 + j ];
            }
        }
    }

    return crimild::impl::Matrix4< T > {
        res[ 0 ],
        res[ 1 ],
        res[ 2 ],
        res[ 3 ],
        res[ 4 ],
        res[ 5 ],
        res[ 6 ],
        res[ 7 ],
        res[ 8 ],
        res[ 9 ],
        res[ 10 ],
        res[ 11 ],
        res[ 12 ],
        res[ 13 ],
        res[ 14 ],
        res[ 15 ],
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix4< T > operator*( const crimild::impl::Matrix4< T > &a, crimild::Real s ) noexcept
{
    return crimild::impl::Matrix4< T > {
        a[ 0 ] * s,
        a[ 1 ] * s,
        a[ 2 ] * s,
        a[ 3 ] * s,
        a[ 4 ] * s,
        a[ 5 ] * s,
        a[ 6 ] * s,
        a[ 7 ] * s,
        a[ 8 ] * s,
        a[ 9 ] * s,
        a[ 10 ] * s,
        a[ 11 ] * s,
        a[ 12 ] * s,
        a[ 13 ] * s,
        a[ 14 ] * s,
        a[ 15 ] * s,
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix4< T > operator*( crimild::Real s, const crimild::impl::Matrix4< T > &a ) noexcept
{
    return crimild::impl::Matrix4< T > {
        a[ 0 ] * s,
        a[ 1 ] * s,
        a[ 2 ] * s,
        a[ 3 ] * s,
        a[ 4 ] * s,
        a[ 5 ] * s,
        a[ 6 ] * s,
        a[ 7 ] * s,
        a[ 8 ] * s,
        a[ 9 ] * s,
        a[ 10 ] * s,
        a[ 11 ] * s,
        a[ 12 ] * s,
        a[ 13 ] * s,
        a[ 14 ] * s,
        a[ 15 ] * s,
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix4< T > operator/( const crimild::impl::Matrix4< T > &a, crimild::Real s ) noexcept
{
    const auto invS = crimild::Real( 1 ) / s;
    return a * invS;
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Vector4< T > operator*( const crimild::impl::Matrix4< T > &a, const crimild::impl::Vector4< T > &v ) noexcept
{
    return crimild::impl::Vector4< T > {
        a[ 0 ] * v[ 0 ] + a[ 1 ] * v[ 1 ] + a[ 2 ] * v[ 2 ] + a[ 3 ] * v[ 3 ],
        a[ 4 ] * v[ 0 ] + a[ 5 ] * v[ 1 ] + a[ 6 ] * v[ 2 ] + a[ 7 ] * v[ 3 ],
        a[ 8 ] * v[ 0 ] + a[ 9 ] * v[ 1 ] + a[ 10 ] * v[ 2 ] + a[ 11 ] * v[ 3 ],
        a[ 12 ] * v[ 0 ] + a[ 13 ] * v[ 1 ] + a[ 14 ] * v[ 2 ] + a[ 15 ] * v[ 3 ],
    };
}

namespace crimild {

    template< typename T >
    [[nodiscard]] constexpr impl::Matrix4< T > transpose( const impl::Matrix4< T > &a ) noexcept
    {
        return impl::Matrix4< T > {
            a[ 0 ],
            a[ 4 ],
            a[ 8 ],
            a[ 12 ],
            a[ 1 ],
            a[ 5 ],
            a[ 9 ],
            a[ 13 ],
            a[ 2 ],
            a[ 6 ],
            a[ 10 ],
            a[ 14 ],
            a[ 3 ],
            a[ 7 ],
            a[ 11 ],
            a[ 15 ],
        };
    }

    template< typename T >
    [[nodiscard]] constexpr Real determinant( const impl::Matrix4< T > &a ) noexcept
    {
        const auto m11 = a[ 0 ];
        const auto m12 = a[ 1 ];
        const auto m13 = a[ 2 ];
        const auto m14 = a[ 3 ];
        const auto m21 = a[ 4 ];
        const auto m22 = a[ 5 ];
        const auto m23 = a[ 6 ];
        const auto m24 = a[ 7 ];
        const auto m31 = a[ 8 ];
        const auto m32 = a[ 9 ];
        const auto m33 = a[ 10 ];
        const auto m34 = a[ 11 ];
        const auto m41 = a[ 12 ];
        const auto m42 = a[ 13 ];
        const auto m43 = a[ 14 ];
        const auto m44 = a[ 15 ];

        return m11 * ( m22 * ( m33 * m44 - m34 * m43 ) + m23 * ( m34 * m42 - m32 * m44 ) + m24 * ( m32 * m43 - m33 * m42 ) )
               - m12 * ( m21 * ( m33 * m44 - m34 * m43 ) + m23 * ( m34 * m41 - m31 * m44 ) + m24 * ( m31 * m43 - m33 * m41 ) )
               + m13 * ( m21 * ( m32 * m44 - m34 * m42 ) + m22 * ( m34 * m41 - m31 * m44 ) + m24 * ( m31 * m42 - m32 * m41 ) )
               - m14 * ( m21 * ( m32 * m43 - m33 * m42 ) + m22 * ( m33 * m41 - m31 * m43 ) + m23 * ( m31 * m42 - m32 * m41 ) );
    }

    template< typename T >
    [[nodiscard]] constexpr impl::Matrix4< T > inverse( const impl::Matrix4< T > &a ) noexcept
    {
        const T a00 = a[ 0 ], a01 = a[ 1 ], a02 = a[ 2 ], a03 = a[ 3 ],
                a10 = a[ 4 ], a11 = a[ 5 ], a12 = a[ 6 ], a13 = a[ 7 ],
                a20 = a[ 8 ], a21 = a[ 9 ], a22 = a[ 10 ], a23 = a[ 11 ],
                a30 = a[ 12 ], a31 = a[ 13 ], a32 = a[ 14 ], a33 = a[ 15 ];

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

        return impl::Matrix4< T > {
            ( a11 * b11 - a12 * b10 + a13 * b09 ) * invDet,
            ( a02 * b10 - a01 * b11 - a03 * b09 ) * invDet,
            ( a31 * b05 - a32 * b04 + a33 * b03 ) * invDet,
            ( a22 * b04 - a21 * b05 - a23 * b03 ) * invDet,
            ( a12 * b08 - a10 * b11 - a13 * b07 ) * invDet,
            ( a00 * b11 - a02 * b08 + a03 * b07 ) * invDet,
            ( a32 * b02 - a30 * b05 - a33 * b01 ) * invDet,
            ( a20 * b05 - a22 * b02 + a23 * b01 ) * invDet,
            ( a10 * b10 - a11 * b08 + a13 * b06 ) * invDet,
            ( a01 * b08 - a00 * b10 - a03 * b06 ) * invDet,
            ( a30 * b04 - a31 * b02 + a33 * b00 ) * invDet,
            ( a21 * b02 - a20 * b04 - a23 * b00 ) * invDet,
            ( a11 * b07 - a10 * b09 - a12 * b06 ) * invDet,
            ( a00 * b09 - a01 * b07 + a02 * b06 ) * invDet,
            ( a31 * b01 - a30 * b03 - a32 * b00 ) * invDet,
            ( a20 * b03 - a21 * b01 + a22 * b00 ) * invDet,
        };
    }

}

#endif
