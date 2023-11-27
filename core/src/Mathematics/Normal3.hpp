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

#include "Mathematics/Concepts.hpp"
#include "Mathematics/Tuple3.hpp"

namespace crimild {

    template< concepts::Arithmetic T >
    struct Normal3Impl : public Tuple3Impl< T > {
        struct Constants {
            static constexpr auto UNIT_X = Normal3Impl< T > { 1, 0, 0 };
            static constexpr auto UNIT_Y = Normal3Impl< T > { 0, 1, 0 };
            static constexpr auto UNIT_Z = Normal3Impl< T > { 0, 0, 1 };
        };

        using Tuple3Impl< T >::x;
        using Tuple3Impl< T >::y;
        using Tuple3Impl< T >::z;

        template< concepts::Arithmetic U >
        [[nodiscard]] inline constexpr Bool operator==( const Normal3Impl< U > &other ) const noexcept
        {
            return x == other.x && y == other.y && z == other.z;
        }

        template< concepts::Arithmetic U >
        [[nodiscard]] inline constexpr Bool operator!=( const Normal3Impl< U > &other ) const noexcept
        {
            return !( *this == other );
        }

        template< concepts::Arithmetic U >
        [[nodiscard]] inline constexpr Normal3Impl operator+( const Normal3Impl< U > &v ) const noexcept
        {
            return Normal3Impl {
                x + v.x,
                y + v.y,
                z + v.z,
            };
        }

        template< concepts::Arithmetic U >
        [[nodiscard]] inline constexpr Normal3Impl operator-( const Normal3Impl< U > &v ) const noexcept
        {
            return Normal3Impl {
                x - v.x,
                y - v.y,
                z - v.z,
            };
        }

        template< concepts::Arithmetic U >
        [[nodiscard]] inline constexpr Normal3Impl operator*( U s ) const noexcept
        {
            return Normal3Impl {
                x * s,
                y * s,
                z * s,
            };
        }

        template< concepts::Arithmetic U >
        [[nodiscard]] inline constexpr Normal3Impl operator*( const Normal3Impl< U > &v ) const noexcept
        {
            return Normal3Impl {
                x * v.x,
                y * v.y,
                z * v.z,
            };
        }

        template< concepts::Arithmetic U >
        [[nodiscard]] inline constexpr Normal3Impl operator/( U s ) const noexcept
        {
            const auto invS = crimild::Real( 1 ) / s;
            return *this * invS;
        }

        [[nodiscard]] inline constexpr Normal3Impl operator-( void ) const noexcept
        {
            return Normal3Impl {
                -x,
                -y,
                -z,
            };
        }
    };

    template< concepts::Arithmetic T, concepts::Arithmetic U >
    [[nodiscard]] inline constexpr auto operator*( U s, const Normal3Impl< T > &u ) noexcept
    {
        return u * s;
    }

    using Normal3 = Normal3Impl< Real >;
    using Normal3f = Normal3Impl< Real32 >;
    using Normal3d = Normal3Impl< Real64 >;

}

#endif
