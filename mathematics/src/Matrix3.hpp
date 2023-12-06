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

#include "Vector3.hpp"

namespace crimild {

    template< typename T >
    struct Matrix3Impl {
        struct Constants;

        Vector3< T > c0;
        Vector3< T > c1;
        Vector3< T > c2;

        [[nodiscard]] inline constexpr const Vector3< T > &operator[]( Index index ) const noexcept
        {
            switch ( index ) {
                case 0:
                    return c0;
                case 1:
                    return c1;
                case 2:
                default:
                    return c2;
            };
        }

        [[nodiscard]] inline Vector3< T > &operator[]( Index index ) noexcept
        {
            switch ( index ) {
                case 0:
                    return c0;
                case 1:
                    return c1;
                case 2:
                default:
                    return c2;
            };
        }

        [[nodiscard]] inline constexpr Bool operator==( const Matrix3Impl &other ) const noexcept;
        [[nodiscard]] inline constexpr Bool operator!=( const Matrix3Impl &other ) const noexcept;
    };

    using Matrix3 = Matrix3Impl< Real >;
    using Matrix3f = Matrix3Impl< Real32 >;
    using Matrix3d = Matrix3Impl< Real64 >;
    using Matrix3i = Matrix3Impl< Int32 >;
    using Matrix3ui = Matrix3Impl< UInt32 >;

}

#endif
