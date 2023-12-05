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

#include "Mathematics/Tuple3.hpp"

namespace crimild {

    template< concepts::Arithmetic T >
    class Normal3 : public Tuple3< Normal3, T > {
    public:
        static const Normal3 UNIT_X;
        static const Normal3 UNIT_Y;
        static const Normal3 UNIT_Z;

    public:
        using Tuple3< Normal3, T >::x;
        using Tuple3< Normal3, T >::y;
        using Tuple3< Normal3, T >::z;

        constexpr Normal3( void ) noexcept = default;

        constexpr Normal3( T x, T y, T z ) noexcept
            : Tuple3< Normal3, T >( x, y, z )
        {
        }

        template< concepts::Arithmetic U >
        constexpr explicit Normal3( U value ) noexcept
            : Tuple3< Normal3, T >( value ) { }

        ~Normal3( void ) noexcept = default;
    };

    template< concepts::Arithmetic T >
    constexpr const Normal3< T > Normal3< T >::UNIT_X( 1, 0, 0 );

    template< concepts::Arithmetic T >
    constexpr const Normal3< T > Normal3< T >::UNIT_Y( 0, 1, 0 );

    template< concepts::Arithmetic T >
    constexpr const Normal3< T > Normal3< T >::UNIT_Z( 0, 0, 1 );

    using Normal3f = Normal3< Real32 >;
    using Normal3d = Normal3< Real64 >;

}

#endif
