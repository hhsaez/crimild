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

#ifndef CRIMILD_MATHEMATICS_MATRIX_4_
#define CRIMILD_MATHEMATICS_MATRIX_4_

#include "Mathematics/Vector4.hpp"

namespace crimild {

    template< typename T >
    struct Matrix4Impl {
        struct Constants;

        Vector4Impl< T > c0;
        Vector4Impl< T > c1;
        Vector4Impl< T > c2;
        Vector4Impl< T > c3;

        [[nodiscard]] inline constexpr const Vector4Impl< T > &operator[]( Index index ) const noexcept
        {
            switch ( index ) {
                case 0:
                    return c0;
                case 1:
                    return c1;
                case 2:
                    return c2;
                case 3:
                default:
                    return c3;
            };
        }

        [[nodiscard]] inline Vector4Impl< T > &operator[]( Index index ) noexcept
        {
            switch ( index ) {
                case 0:
                    return c0;
                case 1:
                    return c1;
                case 2:
                    return c2;
                case 3:
                default:
                    return c3;
            };
        }

        template< typename U >
        [[nodiscard]] inline constexpr Bool operator==( const Matrix4Impl< U > &other ) const noexcept
        {
            return c0 == other.c0 && c1 == other.c1 && c2 == other.c2 && c3 == other.c3;
        }

        template< typename U >
        [[nodiscard]] inline constexpr Bool operator!=( const Matrix4Impl< U > &other ) const noexcept
        {
            return !( *this == other );
        }
    };

    using Matrix4 = Matrix4Impl< Real >;
    using Matrix4f = Matrix4Impl< Real32 >;
    using Matrix4d = Matrix4Impl< Real64 >;
    using Matrix4i = Matrix4Impl< Int32 >;
    using Matrix4ui = Matrix4Impl< UInt32 >;

}

#endif
