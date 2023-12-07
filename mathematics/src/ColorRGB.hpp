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

#ifndef CRIMILD_MATHEMATICS_COLOR_RGB_
#define CRIMILD_MATHEMATICS_COLOR_RGB_

#include "Types.hpp"

#include <cmath>

namespace crimild {

    template< typename T >
    struct ColorRGBImpl {
        struct Constants;

        T r;
        T g;
        T b;

        [[nodiscard]] inline constexpr T operator[]( size_t index ) const noexcept
        {
            switch ( index ) {
                case 0:
                    return r;
                case 1:
                    return g;
                case 2:
                default:
                    return b;
            }
        }

        [[nodiscard]] inline constexpr T &operator[]( size_t index ) noexcept
        {
            switch ( index ) {
                case 0:
                    return r;
                case 1:
                    return g;
                case 2:
                default:
                    return b;
            }
        }

        [[nodiscard]] inline constexpr bool operator==( const ColorRGBImpl &other ) const noexcept
        {
            return r == other.r && g == other.g && b == other.b;
        }

        [[nodiscard]] inline constexpr bool operator!=( const ColorRGBImpl &other ) const noexcept
        {
            return !( *this == other );
        }
    };

    template< typename T >
    struct ColorRGBImpl< T >::Constants {
        static constexpr auto BLACK = ColorRGBImpl< T > { 0, 0, 0 };
        static constexpr auto WHITE = ColorRGBImpl< T > { 1, 1, 1 };
        static constexpr auto RED = ColorRGBImpl< T > { 1, 0, 0 };
        static constexpr auto GREEN = ColorRGBImpl< T > { 0, 1, 0 };
        static constexpr auto BLUE = ColorRGBImpl< T > { 0, 0, 1 };
    };

    using ColorRGB = ColorRGBImpl< real_t >;
    using ColorRGBf = ColorRGBImpl< float >;
    using ColorRGBd = ColorRGBImpl< double >;

}

#endif
