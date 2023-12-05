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

#include "Mathematics/Tuple3.hpp"

#include <limits>

namespace crimild {

    template< concepts::Arithmetic T >
    class Vector3 : public Tuple3< Vector3, T > {
    public:
        static const Vector3 ZERO;
        static const Vector3 ONE;
        static const Vector3 POSITIVE_INFINITY;
        static const Vector3 NEGATIVE_INFINITY;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;
        static const Vector3 FORWARD;
        static const Vector3 UP;
        static const Vector3 RIGHT;

    public:
        using Tuple3< Vector3, T >::x;
        using Tuple3< Vector3, T >::y;
        using Tuple3< Vector3, T >::z;

        constexpr Vector3( void ) noexcept = default;

        constexpr Vector3( T x, T y, T z ) noexcept
            : Tuple3< Vector3, T >( x, y, z )
        {
        }

        template< concepts::Arithmetic U >
        constexpr explicit Vector3( U value ) noexcept
            : Tuple3< Vector3, T >( value ) { }

        template< template< concepts::Arithmetic > class OtherTuple, concepts::Arithmetic U >
        constexpr explicit Vector3( const OtherTuple< U > &other ) noexcept
            : Tuple3< Vector3, T >( other.x, other.y, other.z ) { }

        ~Vector3( void ) noexcept = default;
    };

    template< concepts::Arithmetic T >
    constexpr const Vector3< T > Vector3< T >::ZERO( 0 );

    template< concepts::Arithmetic T >
    constexpr const Vector3< T > Vector3< T >::ONE( 1 );

    template< concepts::Arithmetic T >
    constexpr const Vector3< T > Vector3< T >::POSITIVE_INFINITY(
        std::numeric_limits< T >::infinity(),
        std::numeric_limits< T >::infinity(),
        std::numeric_limits< T >::infinity()
    );

    template< concepts::Arithmetic T >
    constexpr const Vector3< T > Vector3< T >::NEGATIVE_INFINITY(
        -std::numeric_limits< T >::infinity(),
        -std::numeric_limits< T >::infinity(),
        -std::numeric_limits< T >::infinity()
    );

    template< concepts::Arithmetic T >
    constexpr const Vector3< T > Vector3< T >::UNIT_X( 1, 0, 0 );

    template< concepts::Arithmetic T >
    constexpr const Vector3< T > Vector3< T >::UNIT_Y( 0, 1, 0 );

    template< concepts::Arithmetic T >
    constexpr const Vector3< T > Vector3< T >::UNIT_Z( 0, 0, 1 );

    template< concepts::Arithmetic T >
    constexpr const Vector3< T > Vector3< T >::RIGHT( 1, 0, 0 );

    template< concepts::Arithmetic T >
    constexpr const Vector3< T > Vector3< T >::UP( 0, 1, 0 );

    template< concepts::Arithmetic T >
    constexpr const Vector3< T > Vector3< T >::FORWARD( 0, 0, 1 );

    using Vector3f = Vector3< Real32 >;
    using Vector3d = Vector3< Real64 >;
    using Vector3i = Vector3< Int32 >;
    using Vector3ui = Vector3< UInt32 >;

}

#endif
