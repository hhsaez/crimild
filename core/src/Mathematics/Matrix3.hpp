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

#ifndef CRIMILD_MATHEMATICS_MATRIX_3_
#define CRIMILD_MATHEMATICS_MATRIX_3_

#include "Foundation/Types.hpp"

#include <cmath>

namespace crimild {

    namespace impl {

        template< typename T >
        struct Matrix3 {
            struct Constants;

            T m00, m01, m02;
            T m10, m11, m12;
            T m20, m21, m22;

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
                        return m10;
                    case 4:
                        return m11;
                    case 5:
                        return m12;
                    case 6:
                        return m20;
                    case 7:
                        return m21;
                    case 8:
                        return m22;
                    default:
                        return NAN;
                }
            }
        };

        template< typename T >
        struct Matrix3< T >::Constants {
            static constexpr auto ZERO = Matrix3< T > { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            static constexpr auto IDENTITY = Matrix3< T > { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
        };

    }

    using Matrix3 = impl::Matrix3< Real >;
    using Matrix3f = impl::Matrix3< Real32 >;
    using Matrix3d = impl::Matrix3< Real64 >;
    using Matrix3i = impl::Matrix3< Int32 >;
    using Matrix3ui = impl::Matrix3< UInt32 >;

}

#endif
