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

#ifndef CRIMILD_MATHEMATICS_IS_EQUAL_
#define CRIMILD_MATHEMATICS_IS_EQUAL_

#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Matrix3.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Normal3.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Tuple2.hpp"
#include "Mathematics/Tuple3.hpp"
#include "Mathematics/Tuple4.hpp"
#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector4.hpp"
#include "Mathematics/isZero.hpp"

namespace crimild {

    template< typename T >
    inline constexpr Bool isEqual( const T &x, const T &y ) noexcept
    {
        if constexpr ( traits::isReal< T >() ) {
            return isZero( x - y );
        } else {
            return x == y;
        }
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::Tuple2< T > &t, const impl::Tuple2< T > &u ) noexcept
    {
        return isEqual( t.x, u.x ) && isEqual( t.y, u.y );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::Tuple3< T > &t, const impl::Tuple3< T > &u ) noexcept
    {
        return isEqual( t.x, u.x ) && isEqual( t.y, u.y ) && isEqual( t.z, u.z );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::Tuple4< T > &t, const impl::Tuple4< T > &u ) noexcept
    {
        return isEqual( t.x, u.x ) && isEqual( t.y, u.y ) && isEqual( t.z, u.z ) && isEqual( t.w, u.w );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::Vector2< T > &t, const impl::Vector2< T > &u ) noexcept
    {
        return isEqual( t.x, u.x ) && isEqual( t.y, u.y );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::Vector3< T > &t, const impl::Vector3< T > &u ) noexcept
    {
        return isEqual( t.x, u.x ) && isEqual( t.y, u.y ) && isEqual( t.z, u.z );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::Vector4< T > &t, const impl::Vector4< T > &u ) noexcept
    {
        return isEqual( t.x, u.x ) && isEqual( t.y, u.y ) && isEqual( t.z, u.z ) && isEqual( t.w, u.w );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::Point3< T > &t, const impl::Point3< T > &u ) noexcept
    {
        return isEqual( t.x, u.x ) && isEqual( t.y, u.y ) && isEqual( t.z, u.z );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::Normal3< T > &t, const impl::Normal3< T > &u ) noexcept
    {
        return isEqual( t.x, u.x ) && isEqual( t.y, u.y ) && isEqual( t.z, u.z );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::ColorRGB< T > &a, const impl::ColorRGB< T > &b ) noexcept
    {
        return isEqual( a.r, b.r ) && isEqual( a.g, b.g ) && isEqual( a.b, b.b );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::ColorRGBA< T > &a, const impl::ColorRGBA< T > &b ) noexcept
    {
        return isEqual( a.r, b.r ) && isEqual( a.g, b.g ) && isEqual( a.b, b.b ) && isEqual( a.a, b.a );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::Matrix3< T > &a, const impl::Matrix3< T > &b ) noexcept
    {
        Bool ret = true;
        for ( auto i = 0l; i < 9; ++i ) {
            ret = ret && isEqual( a[ i ], b[ i ] );
        }
        return ret;
    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isEqual( const impl::Matrix4< T > &a, const impl::Matrix4< T > &b ) noexcept
    {
        Bool ret = true;
        for ( auto i = 0l; i < 16; ++i ) {
            ret = ret && isEqual( a[ i ], b[ i ] );
        }
        return ret;
    }

}

#endif
