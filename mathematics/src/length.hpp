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

#ifndef CRIMILD_MATHEMATICS_LENGTH_
#define CRIMILD_MATHEMATICS_LENGTH_

#include "Normal3.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "sqrt.hpp"

namespace crimild {

    template< ArithmeticType T >
    [[nodiscard]] inline constexpr real_t lengthSquared( const Vector2Impl< T > &u ) noexcept
    {
        return u.x * u.x + u.y * u.y;
    }

    template< ArithmeticType T >
    [[nodiscard]] inline constexpr real_t length( const Vector2Impl< T > &u ) noexcept
    {
        return sqrt( lengthSquared( u ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr real_t lengthSquared( const Vector4< T > &u ) noexcept
    {
        return u.x * u.x + u.y * u.y + u.z * u.z + u.w * u.w;
    }

    template< typename T >
    [[nodiscard]] inline constexpr real_t length( const Vector4< T > &u ) noexcept
    {
        return sqrt( lengthSquared( u ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr real_t lengthSquared( const Vector3< T > &u ) noexcept
    {
        return u.x * u.x + u.y * u.y + u.z * u.z;
    }

    template< typename T >
    [[nodiscard]] inline constexpr real_t length( const Vector3< T > &u ) noexcept
    {
        return sqrt( lengthSquared( u ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr real_t lengthSquared( const Normal3< T > &u ) noexcept
    {
        return u.x * u.x + u.y * u.y + u.z * u.z;
    }

    template< typename T >
    [[nodiscard]] inline constexpr real_t length( const Normal3< T > &u ) noexcept
    {
        return sqrt( lengthSquared( u ) );
    }

}

#endif