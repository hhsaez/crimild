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

#ifndef CRIMILD_MATHEMATICS_VECTOR_2_
#define CRIMILD_MATHEMATICS_VECTOR_2_

#include "Tuple2.hpp"

namespace crimild {

    template< concepts::Arithmetic T >
    class Vector2 : public Tuple2< Vector2, T > {
    public:
        struct Constants {
            static constexpr Vector2 ZERO = { 0, 0 };
            static constexpr Vector2 ONE = { 1, 1 };
            static constexpr Vector2 POSITIVE_INFINITY = {
                std::numeric_limits< T >::infinity(),
                std::numeric_limits< T >::infinity(),
            };
            static constexpr Vector2 NEGATIVE_INFINITY = {
                -std::numeric_limits< T >::infinity(),
                -std::numeric_limits< T >::infinity(),
            };
            static constexpr Vector2 UNIT_X = { 1, 0 };
            static constexpr Vector2 UNIT_Y = { 0, 1 };
        };

    public:
        using Tuple2< Vector2, T >::x;
        using Tuple2< Vector2, T >::y;

        constexpr Vector2( void ) noexcept = default;

        constexpr Vector2( T x, T y ) noexcept
            : Tuple2< Vector2, T >( x, y )
        {
        }

        template< concepts::Arithmetic U >
        constexpr explicit Vector2( U value ) noexcept
            : Tuple2< Vector2, T >( value ) { }

        ~Vector2( void ) noexcept = default;
    };

    using Vector2f = Vector2< Real32 >;
    using Vector2d = Vector2< Real64 >;
    using Vector2i = Vector2< Int32 >;
    using Vector2ui = Vector2< UInt32 >;

}

#endif
