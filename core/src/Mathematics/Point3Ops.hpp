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

#ifndef CRIMILD_MATHEMATICS_POINT_3_OPERATION_
#define CRIMILD_MATHEMATICS_POINT_3_OPERATION_

#include "Mathematics/Point3.hpp"
#include "Mathematics/Vector3.hpp"

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Point3< T > operator+( const crimild::impl::Point3< T > &u, const crimild::impl::Vector3< T > &v ) noexcept
{
    return crimild::impl::Point3< T > {
        u.x + v.x,
        u.y + v.y,
        u.z + v.z,
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Point3< T > operator+( const crimild::impl::Point3< T > &u, const crimild::impl::Point3< T > &v ) noexcept
{
    return crimild::impl::Point3< T > {
        u.x + v.x,
        u.y + v.y,
        u.z + v.z,
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Vector3< T > operator-( const crimild::impl::Point3< T > &u, const crimild::impl::Point3< T > &v ) noexcept
{
    return crimild::impl::Vector3< T > {
        u.x - v.x,
        u.y - v.y,
        u.z - v.z,
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Point3< T > operator-( const crimild::impl::Point3< T > &u, const crimild::impl::Vector3< T > &v ) noexcept
{
    return crimild::impl::Point3< T > {
        u.x - v.x,
        u.y - v.y,
        u.z - v.z,
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Point3< T > operator*( const crimild::impl::Point3< T > &u, crimild::Real s ) noexcept
{
    return crimild::impl::Point3< T > {
        u.x * s,
        u.y * s,
        u.z * s,
    };
}

template< typename T >
[[nodiscard]] inline constexpr crimild::impl::Point3< T > operator*( crimild::Real s, const crimild::impl::Point3< T > &u ) noexcept
{
    return crimild::impl::Point3< T > {
        u.x * s,
        u.y * s,
        u.z * s,
    };
}

#endif