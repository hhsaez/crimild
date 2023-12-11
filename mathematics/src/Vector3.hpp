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

#ifndef CRIMILD_MATHEMATICS_VECTOR_3_
#define CRIMILD_MATHEMATICS_VECTOR_3_

#include "Tuple3.hpp"
#include "Tuple4.hpp"
#include "Types.hpp"

#include <limits>

namespace crimild {

    template< ArithmeticType T >
    class Vector3Impl : public Tuple3< Vector3Impl, T > {
    public:
        struct Constants {
            static constexpr Vector3Impl ZERO = { 0, 0, 0 };
            static constexpr Vector3Impl ONE = { 1, 1, 1 };
            static constexpr Vector3Impl POSITIVE_INFINITY = {
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity(),
            };
            static constexpr Vector3Impl NEGATIVE_INFINITY = {
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity(),
            };
            static constexpr Vector3Impl UNIT_X = { 1, 0, 0 };
            static constexpr Vector3Impl UNIT_Y = { 0, 1, 0 };
            static constexpr Vector3Impl UNIT_Z = { 0, 0, 1 };
            static constexpr Vector3Impl RIGHT = { 1, 0, 0 };
            static constexpr Vector3Impl UP = { 0, 1, 0 };

            /**
             * @brief Forward vector
             *
             * @ref Readme.md
             *
             * [Readme](Readme.md)
             */
            static constexpr Vector3Impl FORWARD = { 0, 0, -1 };
        };

    public:
        using Tuple3< Vector3Impl, T >::x;
        using Tuple3< Vector3Impl, T >::y;
        using Tuple3< Vector3Impl, T >::z;

        constexpr Vector3Impl( void ) noexcept = default;

        constexpr Vector3Impl( T x, T y, T z ) noexcept
            : Tuple3< Vector3Impl, T >( x, y, z )
        {
        }

        template< ArithmeticType U >
        constexpr explicit Vector3Impl( U value ) noexcept
            : Tuple3< Vector3Impl, T >( value ) { }

        template< template< ArithmeticType > class OtherDerived, ArithmeticType U >
        constexpr explicit Vector3Impl( const Tuple3< OtherDerived, U > &other ) noexcept
            : Tuple3< Vector3Impl, T >( other.x, other.y, other.z ) { }

        template< template< ArithmeticType > class OtherDerived, ArithmeticType U >
        constexpr explicit Vector3Impl( const Tuple4< OtherDerived, U > &other ) noexcept
            : Tuple3< Vector3Impl, T >( other.x, other.y, other.z ) { }

        ~Vector3Impl( void ) noexcept = default;
    };

    using Vector3 = Vector3Impl< real_t >;
    using Vector3f = Vector3Impl< float >;
    using Vector3d = Vector3Impl< double >;
    using Vector3i = Vector3Impl< int32_t >;
    using Vector3ui = Vector3Impl< uint32_t >;

}

#endif
