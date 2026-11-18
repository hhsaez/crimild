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

#include <array>

namespace crimild {

    /**
     * A 3x3 column-based matrix representation
     */
    template< ArithmeticType T >
    class Matrix3Impl : public std::array< Vector3Impl< T >, 3 > {
    public:
        struct Constants;

    public:
        constexpr Matrix3Impl( void ) = default;

        constexpr Matrix3Impl( std::initializer_list< Vector3Impl< T > > cols ) noexcept
        {
            size_t i = 0;
            for ( auto &col : cols ) {
                ( *this )[ i++ ] = col;
            }
        }

        constexpr Matrix3Impl( std::initializer_list< T > xs ) noexcept
        {
            size_t i = 0;
            for ( auto &x : xs ) {
                ( *this )[ i % 3 ][ i / 3 ] = x;
                ++i;
            }
        }

        ~Matrix3Impl( void ) = default;

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator+( const Matrix3Impl< U > &M ) noexcept
        {
            return Matrix3Impl< decltype( T {} + U {} ) > {
                ( *this )[ 0 ] + M[ 0 ],
                ( *this )[ 1 ] + M[ 1 ],
                ( *this )[ 2 ] + M[ 2 ],
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator-( const Matrix3Impl< U > &M ) noexcept
        {
            return Matrix3Impl< decltype( T {} - U {} ) > {
                ( *this )[ 0 ] - M[ 0 ],
                ( *this )[ 1 ] - M[ 1 ],
                ( *this )[ 2 ] - M[ 2 ],
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator*( const U &s ) noexcept
        {
            return Matrix3Impl< decltype( T {} * U {} ) > {
                ( *this )[ 0 ] * s,
                ( *this )[ 1 ] * s,
                ( *this )[ 2 ] * s,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator/( const U &s ) noexcept
        {
            return Matrix3Impl< decltype( T {} * U {} ) > {
                ( *this )[ 0 ] / s,
                ( *this )[ 1 ] / s,
                ( *this )[ 2 ] / s,
            };
        }
    };

    template< ArithmeticType T >
    struct Matrix3Impl< T >::Constants {
        static constexpr auto IDENTITY = Matrix3Impl {
            { 1, 0, 0 },
            { 0, 1, 0 },
            { 0, 0, 1 },
        };

        static constexpr auto ZERO = Matrix3Impl {
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
        };

        static constexpr auto ONE = Matrix3Impl {
            { 1, 1, 1 },
            { 1, 1, 1 },
            { 1, 1, 1 },
        };
    };

    using Matrix3 = Matrix3Impl< real_t >;
    using Matrix3f = Matrix3Impl< float >;
    using Matrix3d = Matrix3Impl< double >;
    using Matrix3i = Matrix3Impl< int32_t >;
    using Matrix3ui = Matrix3Impl< uint32_t >;

}

#endif
