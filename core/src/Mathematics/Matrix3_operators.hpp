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

#ifndef CRIMILD_MATHEMATICS_MATRIX_3_OPS_
#define CRIMILD_MATHEMATICS_MATRIX_3_OPS_

#include "Mathematics/Matrix3.hpp"

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix3< T > operator+( const crimild::impl::Matrix3< T > &a, const crimild::impl::Matrix3< T > &b ) noexcept
{
    return impl::Matrix3< T > {
        a[ 0 ] + b[ 0 ],
        a[ 1 ] + b[ 1 ],
        a[ 2 ] + b[ 2 ],
        a[ 3 ] + b[ 3 ],
        a[ 4 ] + b[ 4 ],
        a[ 5 ] + b[ 5 ],
        a[ 6 ] + b[ 6 ],
        a[ 7 ] + b[ 7 ],
        a[ 8 ] + b[ 8 ],
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix3< T > operator-( const crimild::impl::Matrix3< T > &a, const crimild::impl::Matrix3< T > &b ) noexcept
{
    return impl::Matrix3< T > {
        a[ 0 ] - b[ 0 ],
        a[ 1 ] - b[ 1 ],
        a[ 2 ] - b[ 2 ],
        a[ 3 ] - b[ 3 ],
        a[ 4 ] - b[ 4 ],
        a[ 5 ] - b[ 5 ],
        a[ 6 ] - b[ 6 ],
        a[ 7 ] - b[ 7 ],
        a[ 8 ] - b[ 8 ],
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix3< T > operator*( const crimild::impl::Matrix3< T > &a, const crimild::impl::Matrix3< T > &b ) noexcept
{
    return impl::Matrix3< T > {
        a[ 0 ] * b[ 0 ] + a[ 1 ] * b[ 3 ] + a[ 2 ] * b[ 6 ],
        a[ 0 ] * b[ 1 ] + a[ 1 ] * b[ 4 ] + a[ 2 ] * b[ 7 ],
        a[ 0 ] * b[ 2 ] + a[ 1 ] * b[ 5 ] + a[ 2 ] * b[ 8 ],
        a[ 3 ] * b[ 0 ] + a[ 4 ] * b[ 3 ] + a[ 5 ] * b[ 6 ],
        a[ 3 ] * b[ 1 ] + a[ 4 ] * b[ 4 ] + a[ 5 ] * b[ 7 ],
        a[ 3 ] * b[ 2 ] + a[ 4 ] * b[ 5 ] + a[ 5 ] * b[ 8 ],
        a[ 6 ] * b[ 0 ] + a[ 7 ] * b[ 3 ] + a[ 8 ] * b[ 6 ],
        a[ 6 ] * b[ 1 ] + a[ 7 ] * b[ 4 ] + a[ 8 ] * b[ 7 ],
        a[ 6 ] * b[ 2 ] + a[ 7 ] * b[ 5 ] + a[ 8 ] * b[ 8 ],
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix3< T > operator*( const crimild::impl::Matrix3< T > &a, crimild::Real s ) noexcept
{
    return impl::Matrix3< T > {
        a[ 0 ] * s,
        a[ 1 ] * s,
        a[ 2 ] * s,
        a[ 3 ] * s,
        a[ 4 ] * s,
        a[ 5 ] * s,
        a[ 6 ] * s,
        a[ 7 ] * s,
        a[ 8 ] * s,
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix3< T > operator*( crimild::Real s, const crimild::impl::Matrix3< T > &a ) noexcept
{
    return impl::Matrix3< T > {
        a[ 0 ] * s,
        a[ 1 ] * s,
        a[ 2 ] * s,
        a[ 3 ] * s,
        a[ 4 ] * s,
        a[ 5 ] * s,
        a[ 6 ] * s,
        a[ 7 ] * s,
        a[ 8 ] * s,
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Matrix3< T > operator/( const crimild::impl::Matrix3< T > &a, crimild::Real s ) noexcept
{
    const auto invS = crimild::Real( 1 ) / s;
    return a * invS;
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Vector3< T > operator*( const crimild::impl::Matrix3< T > &a, const crimild::impl::Vector3< T > &v ) noexcept
{
    return crimild::impl::Vector3< T > {
        a[ 0 ] * u[ 0 ] + a[ 1 ] * u[ 1 ] + a[ 2 ] * u[ 2 ],
        a[ 3 ] * u[ 0 ] + a[ 4 ] * u[ 1 ] + a[ 5 ] * u[ 2 ],
        a[ 6 ] * u[ 0 ] + a[ 7 ] * u[ 1 ] + a[ 8 ] * u[ 2 ],
    };
}

#endif
