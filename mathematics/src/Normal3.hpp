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

#ifndef CRIMILD_MATHEMATICS_NORMAL_3_
#define CRIMILD_MATHEMATICS_NORMAL_3_

#include "Tuple3.hpp"
#include "Tuple4.hpp"
#include "Types.hpp"

namespace crimild {

    template< ArithmeticType T >
    class Normal3 : public Tuple3< Normal3, T > {
    public:
        struct Constants {
            static constexpr Normal3 UNIT_X = { 1, 0, 0 };
            static constexpr Normal3 UNIT_Y = { 0, 1, 0 };
            static constexpr Normal3 UNIT_Z = { 0, 0, 1 };
        };

    public:
        using Tuple3< Normal3, T >::x;
        using Tuple3< Normal3, T >::y;
        using Tuple3< Normal3, T >::z;

        constexpr Normal3( void ) noexcept = default;

        constexpr Normal3( T x, T y, T z ) noexcept
            : Tuple3< Normal3, T >( x, y, z )
        {
        }

        template< ArithmeticType U >
        constexpr explicit Normal3( U value ) noexcept
            : Tuple3< Normal3, T >( value ) { }

        template< template< ArithmeticType > class OtherDerived, ArithmeticType U >
        constexpr Normal3( const Tuple3< OtherDerived, U > &other ) noexcept
            : Tuple3< Normal3, T >( other.x, other.y, other.z ) { }

        template< template< ArithmeticType > class OtherDerived, ArithmeticType U >
        constexpr Normal3( const Tuple4< OtherDerived, U > &other ) noexcept
            : Tuple3< Normal3, T >( other.x, other.y, other.z ) { }

        ~Normal3( void ) noexcept = default;
    };

    using Normal3f = Normal3< float >;
    using Normal3d = Normal3< double >;

}

#endif