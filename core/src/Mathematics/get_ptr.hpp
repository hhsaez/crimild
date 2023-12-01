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

#ifndef CRIMILD_MATHEMATICS_GET_PTR_
#define CRIMILD_MATHEMATICS_GET_PTR_

#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Matrix3.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Tuple2.hpp"
#include "Mathematics/Tuple3.hpp"
#include "Mathematics/Tuple4.hpp"

namespace crimild {

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    inline constexpr const T *get_ptr( const Tuple2< Derived, T > &u )
    {
        return static_cast< const Derived< T > * >( &u.x );
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    inline constexpr T *get_ptr( Tuple2< Derived, T > &u )
    {
        return static_cast< Derived< T > * >( &u.x );
    }

    template< typename T >
    inline constexpr const T *get_ptr( const Tuple3Impl< T > &u )
    {
        return static_cast< const T * >( &u.x );
    }

    template< typename T >
    inline constexpr T *get_ptr( Tuple3Impl< T > &u )
    {
        return static_cast< T * >( &u.x );
    }

    template< typename T >
    inline constexpr const T *get_ptr( const Tuple4Impl< T > &u )
    {
        return static_cast< const T * >( &u.x );
    }

    template< typename T >
    inline constexpr T *get_ptr( Tuple4Impl< T > &u )
    {
        return static_cast< T * >( &u.x );
    }

    template< typename T >
    inline constexpr const T *get_ptr( const Matrix3Impl< T > &m ) noexcept
    {
        return static_cast< const T * >( &m.c0.x );
    }

    template< typename T >
    inline constexpr T *get_ptr( Matrix3Impl< T > &m ) noexcept
    {
        return static_cast< T * >( &m.c0.x );
    }

    template< typename T >
    inline constexpr const T *get_ptr( const Matrix4Impl< T > &m ) noexcept
    {
        return static_cast< const T * >( &m.c0.x );
    }

    template< typename T >
    inline constexpr T *get_ptr( Matrix4Impl< T > &m ) noexcept
    {
        return static_cast< T * >( &m.c0.x );
    }

    template< typename T >
    inline constexpr const T *get_ptr( const ColorRGBImpl< T > &c )
    {
        return static_cast< const T * >( &c.r );
    }

    template< typename T >
    inline constexpr T *get_ptr( ColorRGBImpl< T > &c )
    {
        return static_cast< T * >( &c.r );
    }

    template< typename T >
    inline constexpr const T *get_ptr( const ColorRGBAImpl< T > &c )
    {
        return static_cast< const T * >( &c.r );
    }

    template< typename T >
    inline constexpr T *get_ptr( ColorRGBAImpl< T > &c )
    {
        return static_cast< T * >( &c.r );
    }

}

#endif
