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

#include "Ray3.hpp"
#include "Vector4.hpp"

#include <array>

namespace crimild {

    struct Quaternion;

    /**
     * @brief a 4x4 matrix representation
     *
     * Represented as 4 Vector4 columns
     */
    template< typename T >
    struct Matrix4Impl {
        struct Constants {
            static constexpr auto IDENTITY = Matrix4Impl(
                Vector4Impl< T > { 1, 0, 0, 0 },
                Vector4Impl< T > { 0, 1, 0, 0 },
                Vector4Impl< T > { 0, 0, 1, 0 },
                Vector4Impl< T > { 0, 0, 0, 1 }
            );

            static constexpr auto ZERO = Matrix4Impl(
                Vector4Impl< T > { 0, 0, 0, 0 },
                Vector4Impl< T > { 0, 0, 0, 0 },
                Vector4Impl< T > { 0, 0, 0, 0 },
                Vector4Impl< T > { 0, 0, 0, 0 }
            );

            static constexpr auto ONE = Matrix4Impl(
                Vector4Impl< T > { 1, 1, 1, 1 },
                Vector4Impl< T > { 1, 1, 1, 1 },
                Vector4Impl< T > { 1, 1, 1, 1 },
                Vector4Impl< T > { 1, 1, 1, 1 }
            );
        };

        constexpr Matrix4Impl( void ) noexcept = default;

        template< ArithmeticType U >
        constexpr explicit Matrix4Impl( const std::array< Vector4Impl< U >, 4 > &columns ) noexcept
            : columns( columns ) { }

        template< ArithmeticType U >
        constexpr Matrix4Impl( const Vector4Impl< U > &c0, const Vector4Impl< U > &c1, const Vector4Impl< U > &c2, const Vector4Impl< U > &c3 ) noexcept
            : columns( { c0, c1, c2, c3 } ) { }

        template< ArithmeticType U >
        constexpr Matrix4Impl( const Matrix4Impl< U > &other ) noexcept
            : columns( other.columns ) { }

        /**
         * @brief Creates a Matrix from a Quaternion
         *
         * @see Quaternion
         */
        constexpr explicit Matrix4Impl( const Quaternion &q ) noexcept;

        ~Matrix4Impl( void ) noexcept = default;

        [[nodiscard]] inline constexpr const Vector4Impl< T > &operator[]( size_t index ) const noexcept
        {
            return columns[ index ];
        }

        [[nodiscard]] inline Vector4Impl< T > &operator[]( size_t index ) noexcept
        {
            return columns[ index ];
        }

        template< typename U >
        [[nodiscard]] inline constexpr size_t operator==( const Matrix4Impl< U > &other ) const noexcept
        {
            return columns == other.columns;
        }

        template< typename U >
        [[nodiscard]] inline constexpr size_t operator!=( const Matrix4Impl< U > &other ) const noexcept
        {
            return !( *this == other );
        }

        [[nodiscard]] inline constexpr Ray3 operator*( const Ray3 &R ) const noexcept
        {
            return Ray3 {
                Point3( *this * Vector4( origin( R ) ) ),
                Vector3( *this * Vector4( direction( R ) ) ),
            };
        }

        std::array< Vector4Impl< T >, 4 > columns = Constants::IDENTITY.columns;
    };

    using Matrix4 = Matrix4Impl< real_t >;
    using Matrix4f = Matrix4Impl< float >;
    using Matrix4d = Matrix4Impl< double >;
    using Matrix4i = Matrix4Impl< int32_t >;
    using Matrix4ui = Matrix4Impl< int32_t >;

}

#endif
