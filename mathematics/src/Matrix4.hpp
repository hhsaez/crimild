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
     * @brief a 4x4 column-based matrix representation
     *
     * Represented as 4 Vector4 columns
     */
    template< ArithmeticType T >
    class Matrix4Impl : public std::array< Vector4Impl< T >, 4 > {
    public:
        struct Constants;

    public:
        constexpr Matrix4Impl( void ) noexcept = default;

        constexpr Matrix4Impl( std::initializer_list< Vector4Impl< T > > cols ) noexcept
        {
            size_t i = 0;
            for ( auto &col : cols ) {
                ( *this )[ i++ ] = col;
            }
        }

        template< ArithmeticType U >
        constexpr Matrix4Impl( const Vector4Impl< U > &c0, const Vector4Impl< U > &c1, const Vector4Impl< U > &c2, const Vector4Impl< U > &c3 ) noexcept
            : std::array< Vector4Impl< T >, 4 >( { c0, c1, c2, c3 } )
        {
            // no-op
        }

        /**
         * @brief Creates a Matrix from a Quaternion
         *
         * @see Quaternion
         */
        constexpr explicit Matrix4Impl( const Quaternion &q ) noexcept;

        ~Matrix4Impl( void ) noexcept = default;

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator+( const Matrix4Impl< U > &M ) const noexcept
        {
            return Matrix4Impl< decltype( T {} + U {} ) > {
                ( *this )[ 0 ] + M[ 0 ],
                ( *this )[ 1 ] + M[ 1 ],
                ( *this )[ 2 ] + M[ 2 ],
                ( *this )[ 3 ] + M[ 3 ],
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator-( const Matrix4Impl< U > &M ) const noexcept
        {
            return Matrix4Impl< decltype( T {} - U {} ) > {
                ( *this )[ 0 ] - M[ 0 ],
                ( *this )[ 1 ] - M[ 1 ],
                ( *this )[ 2 ] - M[ 2 ],
                ( *this )[ 3 ] - M[ 3 ],
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator*( const U &s ) const noexcept
        {
            return Matrix4Impl< decltype( T {} * U {} ) > {
                ( *this )[ 0 ] * s,
                ( *this )[ 1 ] * s,
                ( *this )[ 2 ] * s,
                ( *this )[ 3 ] * s,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] friend inline constexpr auto operator*( const U &s, const Matrix4Impl< T > &M ) noexcept
        {
            return Matrix4Impl< decltype( T {} * U {} ) > {
                M[ 0 ] * s,
                M[ 1 ] * s,
                M[ 2 ] * s,
                M[ 3 ] * s,
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator/( const U &s ) const noexcept
        {
            const real_t invS = real_t( 1 ) / s;
            return ( *this * invS );
        }

        template< ArithmeticType U >
        [[nodiscard]] constexpr auto operator*( const Matrix4Impl< U > &B ) const noexcept
        {
            const auto &a0 = ( *this )[ 0 ];
            const auto &a1 = ( *this )[ 1 ];
            const auto &a2 = ( *this )[ 2 ];
            const auto &a3 = ( *this )[ 3 ];
            const auto &a00 = a0[ 0 ];
            const auto &a01 = a0[ 1 ];
            const auto &a02 = a0[ 2 ];
            const auto &a03 = a0[ 3 ];
            const auto &a10 = a1[ 0 ];
            const auto &a11 = a1[ 1 ];
            const auto &a12 = a1[ 2 ];
            const auto &a13 = a1[ 3 ];
            const auto &a20 = a2[ 0 ];
            const auto &a21 = a2[ 1 ];
            const auto &a22 = a2[ 2 ];
            const auto &a23 = a2[ 3 ];
            const auto &a30 = a3[ 0 ];
            const auto &a31 = a3[ 1 ];
            const auto &a32 = a3[ 2 ];
            const auto &a33 = a3[ 3 ];

            const auto &b0 = B[ 0 ];
            const auto &b1 = B[ 1 ];
            const auto &b2 = B[ 2 ];
            const auto &b3 = B[ 3 ];
            const auto &b00 = b0[ 0 ];
            const auto &b01 = b0[ 1 ];
            const auto &b02 = b0[ 2 ];
            const auto &b03 = b0[ 3 ];
            const auto &b10 = b1[ 0 ];
            const auto &b11 = b1[ 1 ];
            const auto &b12 = b1[ 2 ];
            const auto &b13 = b1[ 3 ];
            const auto &b20 = b2[ 0 ];
            const auto &b21 = b2[ 1 ];
            const auto &b22 = b2[ 2 ];
            const auto &b23 = b2[ 3 ];
            const auto &b30 = b3[ 0 ];
            const auto &b31 = b3[ 1 ];
            const auto &b32 = b3[ 2 ];
            const auto &b33 = b3[ 3 ];

            return Matrix4Impl< decltype( T {} * U {} ) > {
                {
                    a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03,
                    a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03,
                    a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03,
                    a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03,
                },
                {
                    a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13,
                    a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13,
                    a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13,
                    a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13,
                },
                {
                    a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23,
                    a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23,
                    a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23,
                    a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23,
                },
                {
                    a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33,
                    a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33,
                    a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33,
                    a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33,
                }
            };
        }

        template< ArithmeticType U >
        [[nodiscard]] inline constexpr auto operator*( const Vector4Impl< U > &B ) const noexcept
        {
            const auto &a0 = ( *this )[ 0 ];
            const auto &a1 = ( *this )[ 1 ];
            const auto &a2 = ( *this )[ 2 ];
            const auto &a3 = ( *this )[ 3 ];
            const auto &a00 = a0[ 0 ];
            const auto &a01 = a0[ 1 ];
            const auto &a02 = a0[ 2 ];
            const auto &a03 = a0[ 3 ];
            const auto &a10 = a1[ 0 ];
            const auto &a11 = a1[ 1 ];
            const auto &a12 = a1[ 2 ];
            const auto &a13 = a1[ 3 ];
            const auto &a20 = a2[ 0 ];
            const auto &a21 = a2[ 1 ];
            const auto &a22 = a2[ 2 ];
            const auto &a23 = a2[ 3 ];
            const auto &a30 = a3[ 0 ];
            const auto &a31 = a3[ 1 ];
            const auto &a32 = a3[ 2 ];
            const auto &a33 = a3[ 3 ];

            const auto &b0 = B[ 0 ];
            const auto &b1 = B[ 1 ];
            const auto &b2 = B[ 2 ];
            const auto &b3 = B[ 3 ];

            return Vector4Impl< decltype( T {} * U {} ) > {
                a00 * b0 + a10 * b1 + a20 * b2 + a30 * b3,
                a01 * b0 + a11 * b1 + a21 * b2 + a31 * b3,
                a02 * b0 + a12 * b1 + a22 * b2 + a32 * b3,
                a03 * b0 + a13 * b1 + a23 * b2 + a33 * b3,
            };
        }

        [[nodiscard]] inline constexpr Ray3 operator*( const Ray3 &R ) const noexcept
        {
            return Ray3 {
                Point3( *this * Vector4( origin( R ) ) ),
                Vector3( *this * Vector4( direction( R ) ) ),
            };
        }
    };

    template< ArithmeticType T >
    struct Matrix4Impl< T >::Constants {
        static constexpr auto IDENTITY = Matrix4Impl {
            { 1, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 }
        };

        static constexpr auto ZERO = Matrix4Impl {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        };

        static constexpr auto ONE = Matrix4Impl {
            { 1, 1, 1, 1 },
            { 1, 1, 1, 1 },
            { 1, 1, 1, 1 },
            { 1, 1, 1, 1 }
        };
    };

    using Matrix4 = Matrix4Impl< real_t >;
    using Matrix4f = Matrix4Impl< float >;
    using Matrix4d = Matrix4Impl< double >;
    using Matrix4i = Matrix4Impl< int32_t >;
    using Matrix4ui = Matrix4Impl< int32_t >;

}

#endif
