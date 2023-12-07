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
#include "Types.hpp"
#include "Vector3.hpp"

#include <limits>

namespace crimild {

    template< ArithmeticType T >
    class Vector4 : public Tuple4< Vector4, T > {
    public:
        struct Constants {
            static constexpr Vector4 ZERO = { 0, 0, 0, 0 };
            static constexpr Vector4 ONE = { 1, 1, 1, 1 };
            static constexpr Vector4 POSITIVE_INFINITY = {
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity()
            };
            static constexpr Vector4 NEGATIVE_INFINITY = {
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity()
            };
            static constexpr Vector4 UNIT_X = { 1, 0, 0, 0 };
            static constexpr Vector4 UNIT_Y = { 0, 1, 0, 0 };
            static constexpr Vector4 UNIT_Z = { 0, 0, 1, 0 };
            static constexpr Vector4 UNIT_W = { 0, 0, 0, 1 };
        };

    public:
        using Tuple4< Vector4, T >::x;
        using Tuple4< Vector4, T >::y;
        using Tuple4< Vector4, T >::z;
        using Tuple4< Vector4, T >::w;

        constexpr Vector4( void ) noexcept = default;

        constexpr Vector4( T x, T y, T z, T w ) noexcept
            : Tuple4< Vector4, T >( x, y, z, w )
        {
        }

        template< ArithmeticType U >
        constexpr explicit Vector4( U value ) noexcept
            : Tuple4< Vector4, T >( value ) { }

        template< ArithmeticType U >
        constexpr explicit Vector4( const Point3< U > &p ) noexcept
            : Tuple4< Vector4, T >( p.x, p.y, p.z, 1 ) { }

        template< ArithmeticType U >
        constexpr explicit Vector4( const Vector3< U > &v ) noexcept
            : Tuple4< Vector4, T >( v.x, v.y, v.z, 0 ) { }

        template< ArithmeticType U >
        constexpr explicit Vector4( const Normal3< U > &n ) noexcept
            : Tuple4< Vector4, T >( n.x, n.y, n.z, 1 ) { }

        ~Vector4( void ) noexcept = default;
    };

    using Vector4f = Vector4< float >;
    using Vector4d = Vector4< double >;
    using Vector4i = Vector4< int32_t >;
    using Vector4ui = Vector4< uint32_t >;

}

#endif
