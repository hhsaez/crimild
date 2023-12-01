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

    template< template< concepts::Arithmetic > class Derived, concepts::Arithmetic T >
    [[nodiscard]] inline constexpr auto permute( const Tuple2< Derived, T > &u, size_t x, size_t y ) noexcept
    {
        return Derived< T > {
            u[ x ],
            u[ y ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Vector3Impl< T > permute( const Vector3Impl< T > &u, Int x, Int y, Int z ) noexcept
    {
        return Vector3Impl< T > {
            u[ x ],
            u[ y ],
            u[ z ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Vector4Impl< T > permute( const Vector4Impl< T > &u, Int x, Int y, Int z, Int w ) noexcept
    {
        return Vector4Impl< T > {
            u[ x ],
            u[ y ],
            u[ z ],
            u[ w ]
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr Point3Impl< T > permute( const Point3Impl< T > &u, Int x, Int y, Int z ) noexcept
    {
        return Point3Impl< T > {
            u[ x ],
            u[ y ],
            u[ z ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr ColorRGBImpl< T > permute( const ColorRGBImpl< T > &c, Int r, Int g, Int b ) noexcept
    {
        return ColorRGBImpl< T > {
            c[ r ],
            c[ g ],
            c[ b ],
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr ColorRGBAImpl< T > permute( const ColorRGBAImpl< T > &c, Int r, Int g, Int b, Int a ) noexcept
    {
        return ColorRGBAImpl< T > {
            c[ r ],
            c[ g ],
            c[ b ],
            c[ a ],
        };
    }

}

#endif
