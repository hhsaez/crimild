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

#ifndef CRIMILD_MATHEMATICS_POINT_2_
#define CRIMILD_MATHEMATICS_POINT_2_

#include "Mathematics/Tuple2.hpp"

namespace crimild {

    template< concepts::Arithmetic T = Real >
    class Point2 : public Tuple2< Point2, T > {
    public:
        static const Point2 ZERO;
        static const Point2 ONE;
        static const Point2 POSITIVE_INFINITY;
        static const Point2 NEGATIVE_INFINITY;

    public:
        using Tuple2< Point2, T >::x;
        using Tuple2< Point2, T >::y;

        constexpr Point2( void ) noexcept = default;

        constexpr Point2( T x, T y ) noexcept
            : Tuple2< Point2, T >( x, y )
        {
        }

        template< concepts::Arithmetic U >
        constexpr explicit Point2( U value ) noexcept
            : Tuple2< Point2, T >( value ) { }

        ~Point2( void ) noexcept = default;
    };

    template< concepts::Arithmetic T >
    constexpr const Point2< T > Point2< T >::ZERO( 0 );

    template< concepts::Arithmetic T >
    constexpr const Point2< T > Point2< T >::ONE( 1 );

    template< concepts::Arithmetic T >
    constexpr const Point2< T > Point2< T >::POSITIVE_INFINITY(
        std::numeric_limits< T >::infinity(),
        std::numeric_limits< T >::infinity()
    );

    template< concepts::Arithmetic T >
    constexpr const Point2< T > Point2< T >::NEGATIVE_INFINITY(
        -std::numeric_limits< T >::infinity(),
        -std::numeric_limits< T >::infinity()
    );

    using Point2f = Point2< Real32 >;
    using Point2d = Point2< Real64 >;
    using Point2i = Point2< Int32 >;
    using Point2ui = Point2< UInt32 >;

}

#endif
