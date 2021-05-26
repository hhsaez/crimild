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

#ifndef CRIMILD_MATHEMATICS_PERMUTATION_
#define CRIMILD_MATHEMATICS_PERMUTATION_

#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Tuple2.hpp"
#include "Mathematics/Tuple3.hpp"
#include "Mathematics/Tuple4.hpp"
#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector4.hpp"

namespace crimild {

    template< typename T >
    [[nodiscard]] inline constexpr impl::Tuple2< T > permute( const impl::Tuple2< T > &u, Int x, Int y ) noexcept
    {
        return impl::Tuple2< T > {
            u[ x ],
            u[ y ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Tuple3< T > permute( const impl::Tuple3< T > &u, Int x, Int y, Int z ) noexcept
    {
        return impl::Tuple3< T > {
            u[ x ],
            u[ y ],
            u[ z ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Tuple4< T > permute( const impl::Tuple4< T > &u, Int x, Int y, Int z, Int w ) noexcept
    {
        return impl::Tuple4< T > {
            u[ x ],
            u[ y ],
            u[ z ],
            u[ w ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Vector2< T > permute( const impl::Vector2< T > &u, Int x, Int y ) noexcept
    {
        return impl::Vector2< T > {
            u[ x ],
            u[ y ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Vector3< T > permute( const impl::Vector3< T > &u, Int x, Int y, Int z ) noexcept
    {
        return impl::Vector3< T > {
            u[ x ],
            u[ y ],
            u[ z ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Vector4< T > permute( const impl::Vector4< T > &u, Int x, Int y, Int z, Int w ) noexcept
    {
        return impl::Vector4< T > {
            u[ x ],
            u[ y ],
            u[ z ],
            u[ w ]
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Point3< T > permute( const impl::Point3< T > &u, Int x, Int y, Int z ) noexcept
    {
        return impl::Point3< T > {
            u[ x ],
            u[ y ],
            u[ z ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::ColorRGB< T > permute( const impl::ColorRGB< T > &c, Int r, Int g, Int b ) noexcept
    {
        return impl::ColorRGB< T > {
            c[ r ],
            c[ g ],
            c[ b ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::ColorRGBA< T > permute( const impl::ColorRGBA< T > &c, Int r, Int g, Int b, Int a ) noexcept
    {
        return impl::ColorRGBA< T > {
            c[ r ],
            c[ g ],
            c[ b ],
            c[ a ],
        };
    }

}

#endif
