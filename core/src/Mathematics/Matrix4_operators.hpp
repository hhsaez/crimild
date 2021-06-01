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
[[nodiscard]] static constexpr crimild::impl::Matrix4< T > operator*( const crimild::impl::Matrix4< T > &A, const crimild::impl::Matrix4< T > &B ) noexcept
{
    return crimild::impl::Matrix4< T > {
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

template< typename T >
[[nodiscard]] static constexpr crimild::impl::Vector4< T > operator*( const crimild::impl::Matrix4< T > &A, const crimild::impl::Vector4< T > &B ) noexcept
{
    return crimild::impl::Vector4< T > {
        A[ 0 ][ 0 ] * B[ 0 ] + A[ 1 ][ 0 ] * B[ 1 ] + A[ 2 ][ 0 ] * B[ 2 ] + A[ 3 ][ 0 ] * B[ 3 ],
        A[ 0 ][ 1 ] * B[ 0 ] + A[ 1 ][ 1 ] * B[ 1 ] + A[ 2 ][ 1 ] * B[ 2 ] + A[ 3 ][ 1 ] * B[ 3 ],
        A[ 0 ][ 2 ] * B[ 0 ] + A[ 1 ][ 2 ] * B[ 1 ] + A[ 2 ][ 2 ] * B[ 2 ] + A[ 3 ][ 2 ] * B[ 3 ],
        A[ 0 ][ 3 ] * B[ 0 ] + A[ 1 ][ 3 ] * B[ 1 ] + A[ 2 ][ 3 ] * B[ 2 ] + A[ 3 ][ 3 ] * B[ 3 ],
    };
}

#endif
