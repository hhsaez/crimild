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

#ifndef CRIMILD_MATHEMATICS_ABS_
#define CRIMILD_MATHEMATICS_ABS_

#include "Traits.hpp"
#include "Tuple2.hpp"
#include "Tuple3.hpp"
#include "Tuple4.hpp"

namespace crimild {

    template< concepts::Arithmetic T >
    inline constexpr T abs( const T &x ) noexcept
    {
        if constexpr ( traits::isReal< T >() ) {
            // std::fabs cannot be used in constexpr
            return x > 0 ? x : -x;
        } else {
            return std::abs( x );
        }
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto abs( const Tuple2< Derived, T > &t ) noexcept
    {
        return Derived< T > {
            abs( t.x ),
            abs( t.y ),
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto abs( const Tuple3< Derived, T > &t ) noexcept
    {
        return Derived< T > {
            abs( t.x ),
            abs( t.y ),
            abs( t.z ),
        };
    }

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto abs( const Tuple4< Derived, T > &t ) noexcept
    {
        return Derived< T > {
            abs( t.x ),
            abs( t.y ),
            abs( t.z ),
            abs( t.w ),
        };
    }

}

#endif
