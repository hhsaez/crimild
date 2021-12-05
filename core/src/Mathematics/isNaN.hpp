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

#ifndef CRIMILD_MATHEMATICS_IS_NAN_
#define CRIMILD_MATHEMATICS_IS_NAN_

#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Normal3.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector4.hpp"

#include <cmath>

namespace crimild {

    template< typename T >
    inline constexpr Bool isNaN( const T &value ) noexcept
    {
        // std::isnan cannot be used in constexpr
        // This might not be 100% correct in all platforms, but so far it's good
        return value != value;
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isNaN( const Vector2Impl< T > &t ) noexcept
    {
        return isNaN( t.x ) || isNaN( t.y );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isNaN( const Vector3Impl< T > &t ) noexcept
    {
        return isNaN( t.x ) || isNaN( t.y ) || isNaN( t.z );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isNaN( const Vector4Impl< T > &t ) noexcept
    {
        return isNaN( t.x ) || isNaN( t.y ) || isNaN( t.z ) || isNaN( t.w );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isNaN( const Normal3Impl< T > &t ) noexcept
    {
        return isNaN( t.x ) || isNaN( t.y ) || isNaN( t.z );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isNaN( const Point3Impl< T > &t ) noexcept
    {
        return isNaN( t.x ) || isNaN( t.y ) || isNaN( t.z );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isNaN( const ColorRGBImpl< T > &c ) noexcept
    {
        return isNaN( c.r ) || isNaN( c.g ) || isNaN( c.b );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isNaN( const ColorRGBAImpl< T > &c ) noexcept
    {
        return isNaN( c.r ) || isNaN( c.g ) || isNaN( c.b ) || isNaN( c.a );
    }

}

#endif
