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

#ifndef CRIMILD_MATHEMATICS_COLOR_RGBA_
#define CRIMILD_MATHEMATICS_COLOR_RGBA_

#include "Foundation/Types.hpp"
#include "Mathematics/isEqual.hpp"

#include <cmath>

namespace crimild {

    template< typename T >
    struct ColorRGBAImpl {
        struct Constants;

        T r;
        T g;
        T b;
        T a;

        [[nodiscard]] inline constexpr T operator[]( Size index ) const noexcept
        {
            switch ( index ) {
                case 0:
                    return r;
                case 1:
                    return g;
                case 2:
                    return b;
                case 3:
                default:
                    return a;
            }
        }

        [[nodiscard]] inline constexpr T &operator[]( Size index ) noexcept
        {
            switch ( index ) {
                case 0:
                    return r;
                case 1:
                    return g;
                case 2:
                    return b;
                case 3:
                default:
                    return a;
            }
        }

        [[nodiscard]] inline constexpr Bool operator==( const ColorRGBAImpl &other ) const noexcept
        {
            return isEqual( this->r, other.r ) && isEqual( this->g, other.g ) && isEqual( this->b, other.b ) && isEqual( this->a, other.a );
        }

        [[nodiscard]] inline constexpr Bool operator!=( const ColorRGBAImpl &other ) const noexcept
        {
            return !isEqual( this->r, other.r ) || !isEqual( this->g, other.g ) || !isEqual( this->b, other.b ) || !isEqual( this->a, other.a );
        }
    };

    template< typename T >
    struct ColorRGBAImpl< T >::Constants {
        static constexpr auto CLEAR = ColorRGBAImpl< T > { 0, 0, 0, 0 };
        static constexpr auto BLACK = ColorRGBAImpl< T > { 0, 0, 0, 1 };
        static constexpr auto WHITE = ColorRGBAImpl< T > { 1, 1, 1, 1 };
        static constexpr auto RED = ColorRGBAImpl< T > { 1, 0, 0, 1 };
        static constexpr auto GREEN = ColorRGBAImpl< T > { 0, 1, 0, 1 };
        static constexpr auto BLUE = ColorRGBAImpl< T > { 0, 0, 1, 1 };
    };

    using ColorRGBA = ColorRGBAImpl< Real >;
    using ColorRGBAf = ColorRGBAImpl< Real32 >;
    using ColorRGBAd = ColorRGBAImpl< Real64 >;

}

#endif
