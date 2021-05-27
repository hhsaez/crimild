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

#include "Foundation/Types.hpp"
#include "Mathematics/isEqual.hpp"

#include <cmath>

namespace crimild {

    namespace impl {

        template< typename T >
        struct Matrix4 {
            struct Constants;

            T m00, m01, m02, m03;
            T m10, m11, m12, m13;
            T m20, m21, m22, m23;
            T m30, m31, m32, m33;

            [[nodiscard]] inline constexpr T operator[]( Size index ) const noexcept
            {
                switch ( index ) {
                    case 0:
                        return m00;
                    case 1:
                        return m01;
                    case 2:
                        return m02;
                    case 3:
                        return m03;

                    case 4:
                        return m10;
                    case 5:
                        return m11;
                    case 6:
                        return m12;
                    case 7:
                        return m13;

                    case 8:
                        return m20;
                    case 9:
                        return m21;
                    case 10:
                        return m22;
                    case 11:
                        return m23;

                    case 12:
                        return m30;
                    case 13:
                        return m31;
                    case 14:
                        return m32;
                    case 15:
                        return m33;

                    default:
                        return NAN;
                }
            }

            [[nodiscard]] inline constexpr Bool operator==( const Matrix4 &other ) const noexcept
            {
                auto ret = true;
                for ( auto i = 0l; i < 16; ++i ) {
                    ret = ret && isEqual( ( *this )[ i ], other[ i ] );
                }
                return ret;
            }

            [[nodiscard]] inline constexpr Bool operator!=( const Matrix4 &other ) const noexcept
            {
                auto ret = false;
                for ( auto i = 0l; i < 16; ++i ) {
                    ret = ret || !isEqual( ( *this )[ i ], other[ i ] );
                }
                return ret;
            }
        };

        template< typename T >
        struct Matrix4< T >::Constants {
            static constexpr auto ZERO = Matrix4< T > { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            static constexpr auto IDENTITY = Matrix4< T > { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
        };

    }

    using Matrix4 = impl::Matrix4< Real >;
    using Matrix4f = impl::Matrix4< Real32 >;
    using Matrix4d = impl::Matrix4< Real64 >;
    using Matrix4i = impl::Matrix4< Int32 >;
    using Matrix4ui = impl::Matrix4< UInt32 >;

}

#endif
