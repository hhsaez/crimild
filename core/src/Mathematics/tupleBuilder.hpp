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

#ifndef CRIMILD_MATHEMATICS_TUPLE_BUILDER_
#define CRIMILD_MATHEMATICS_TUPLE_BUILDER_

#include "Mathematics/Normal3.hpp"
#include "Mathematics/Point2.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector4.hpp"

namespace crimild {

    namespace traits {

        template< template< typename > class PointImpl >
        [[nodiscard]] constexpr Bool isPoint( void ) noexcept { return false; }

        template<>
        [[nodiscard]] constexpr Bool isPoint< Point2Impl >( void ) noexcept { return true; }

        template<>
        [[nodiscard]] constexpr Bool isPoint< Point3Impl >( void ) noexcept { return true; }

    }

    namespace traits {

        template< template< typename > class VectorImpl >
        [[nodiscard]] inline constexpr Bool isVector( void ) noexcept { return false; }

        template<>
        [[nodiscard]] inline constexpr Bool isVector< Vector2Impl >( void ) noexcept { return true; }

        template<>
        [[nodiscard]] inline constexpr Bool isVector< Vector3Impl >( void ) noexcept { return true; }

        template<>
        [[nodiscard]] inline constexpr Bool isVector< Vector4Impl >( void ) noexcept { return true; }
    }

    namespace traits {

        template< template< typename > class VectorImpl >
        [[nodiscard]] inline constexpr Bool isNormal( void ) noexcept { return false; }

        template<>
        [[nodiscard]] inline constexpr Bool isNormal< Normal3Impl >( void ) noexcept { return true; }
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr auto tuple2Builder( T x, T y ) noexcept
    {
        if constexpr ( traits::isPoint< TupleImpl >() ) {
            return Point2Impl< T > { x, y };
        } else if constexpr ( traits::isVector< TupleImpl >() ) {
            return Vector2Impl< T > { x, y };
        } else {
            return Tuple2Impl< T > { x, y };
        }
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr auto tuple3Builder( T x, T y, T z ) noexcept
    {
        if constexpr ( traits::isPoint< TupleImpl >() ) {
            return Point3Impl< T > { x, y, z };
        } else if constexpr ( traits::isVector< TupleImpl >() ) {
            return Vector3Impl< T > { x, y, z };
        } else if constexpr ( traits::isNormal< TupleImpl >() ) {
            return Normal3Impl< T > { x, y, z };
        } else {
            return Tuple3Impl< T > { x, y, z };
        }
    }

    template< template< typename > class TupleImpl, typename T >
    [[nodiscard]] inline constexpr auto tuple4Builder( T x, T y, T z, T w ) noexcept
    {
        if constexpr ( traits::isVector< TupleImpl >() ) {
            return Vector4Impl< T > { x, y, z, w };
        } else {
            return Tuple4Impl< T > { x, y, z, w };
        }
    }

}

#endif
