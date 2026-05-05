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

#ifndef CRIMILD_MATHEMATICS_VECTOR_4_
#define CRIMILD_MATHEMATICS_VECTOR_4_

#include "Normal3.hpp"
#include "Point3.hpp"
#include "Tuple4.hpp"
#include "Vector3.hpp"
#include "types.hpp"

#include <limits>

namespace crimild {

    template< ArithmeticType T >
    class Vector4Impl : public Tuple4< Vector4Impl, T > {
    public:
        struct Constants {
            static constexpr Vector4Impl ZERO = { 0, 0, 0, 0 };
            static constexpr Vector4Impl ONE = { 1, 1, 1, 1 };
            static constexpr Vector4Impl POSITIVE_INFINITY = {
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity()
            };
            static constexpr Vector4Impl NEGATIVE_INFINITY = {
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity()
            };
            static constexpr Vector4Impl UNIT_X = { 1, 0, 0, 0 };
            static constexpr Vector4Impl UNIT_Y = { 0, 1, 0, 0 };
            static constexpr Vector4Impl UNIT_Z = { 0, 0, 1, 0 };
            static constexpr Vector4Impl UNIT_W = { 0, 0, 0, 1 };
        };

    public:
        using Tuple4< Vector4Impl, T >::x;
        using Tuple4< Vector4Impl, T >::y;
        using Tuple4< Vector4Impl, T >::z;
        using Tuple4< Vector4Impl, T >::w;

        constexpr Vector4Impl( void ) noexcept = default;

        constexpr Vector4Impl( T x, T y, T z, T w ) noexcept
            : Tuple4< Vector4Impl, T >( x, y, z, w )
        {
        }

        template< ArithmeticType U >
        constexpr explicit Vector4Impl( U value ) noexcept
            : Tuple4< Vector4Impl, T >( value ) { }

        template< ArithmeticType U >
        constexpr explicit Vector4Impl( const Point3Impl< U > &p ) noexcept
            : Tuple4< Vector4Impl, T >( p.x, p.y, p.z, 1 ) { }

        template< ArithmeticType U >
        constexpr explicit Vector4Impl( const Vector3Impl< U > &v ) noexcept
            : Tuple4< Vector4Impl, T >( v.x, v.y, v.z, 0 ) { }

        template< ArithmeticType U >
        constexpr explicit Vector4Impl( const Normal3Impl< U > &n ) noexcept
            : Tuple4< Vector4Impl, T >( n.x, n.y, n.z, 0 ) { }

        ~Vector4Impl( void ) noexcept = default;
    };

    using Vector4 = Vector4Impl< real_t >;
    using Vector4f = Vector4Impl< float >;
    using Vector4d = Vector4Impl< double >;
    using Vector4i = Vector4Impl< int32_t >;
    using Vector4ui = Vector4Impl< uint32_t >;

}

#endif
