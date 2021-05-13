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

#ifndef CRIMILD_MATHEMATICS_MATRIX_IMPL_
#define CRIMILD_MATHEMATICS_MATRIX_IMPL_

#include "Mathematics/VectorImpl.hpp"

#include <array>
#include <iomanip>
#include <sstream>

namespace crimild {

    namespace impl {

        template< typename T, Size N >
        class Matrix {
        private:
            using Vector = impl::Vector< T, N >;

        public:
            struct Constants;

            constexpr Matrix( void ) noexcept = default;

            constexpr Matrix( std::initializer_list< T > ns ) noexcept
            {
                //static_assert( ns.size() == N * N );
                auto i = 0l;
                for ( auto n : ns ) {
                    m_table[ i++ ] = n;
                }
            }

            constexpr Matrix( const T *data ) noexcept
            {
                for ( auto i = 0l; i < N * N; ++i ) {
                    m_table[ i ] = data[ i ];
                }
            }

            [[nodiscard]] inline constexpr Vector operator[]( Size index ) const noexcept
            {
                if constexpr ( N == 3 ) {
                    return Vector {
                        m_table[ index + 0 ],
                        m_table[ index + 3 ],
                        m_table[ index + 6 ],
                    };
                } else {
                    return Vector {
                        m_table[ index + 0 ],
                        m_table[ index + 4 ],
                        m_table[ index + 8 ],
                        m_table[ index + 12 ],
                    };
                }
            }

            [[nodiscard]] inline constexpr const Bool operator==( const Matrix &other ) const noexcept
            {
                auto ret = true;
                for ( auto i = 0l; i < N * N; ++i ) {
                    ret = ret && isEqual( m_table[ i ], other.m_table[ i ] );
                }
                return ret;
            }

            [[nodiscard]] inline constexpr const Bool operator!=( const Matrix &other ) const noexcept
            {
                return !( *this == other );
            }

            [[nodiscard]] inline constexpr Matrix operator*( const Matrix &m ) const noexcept
            {
                T ret[ N * N ] = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    for ( auto j = 0l; j < N; ++j ) {
                        for ( auto k = 0l; k < N; ++k ) {
                            ret[ i * N + j ] += m_table[ i * N + k ] * m.m_table[ k * N + j ];
                        }
                    }
                }
                return Matrix( ret );
            }

            [[nodiscard]] inline constexpr Vector operator*( const Vector &v ) const noexcept
            {
                T ret[ N ] = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    for ( auto j = 0l; j < N; ++j ) {
                        ret[ i ] += m_table[ i * N + j ] * v[ j ];
                    }
                }
                return Vector( ret );
            }

            [[nodiscard]] explicit inline constexpr operator const T *( void ) const noexcept
            {
                // TODO: I'm not 100% convinced of providing this conversion tool, but it makes thing easier
                // (and probably faster too) for some operations that require to access the raw data of
                // a matrix, like computing a transpose or an inverse.
                return &m_table[ 0 ];
            }

        private:
            T m_table[ N * N ];
        };

        template< typename T, Size N >
        struct Matrix< T, N >::Constants {
            static constexpr auto ZERO = Matrix();

            // clang-format off
            static constexpr auto IDENTITY = [] {
                if constexpr ( N == 3 ) {
                    return Matrix {
                        1, 0, 0,
                        0, 1, 0,
                        0, 0, 1
                    };
                } else {
                    return Matrix {
                        1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1
                    };
                }
            }();
            // clang-format on
        };

    }

    template< typename T, Size N >
    [[nodiscard]] constexpr impl::Matrix< T, N > transpose( const impl::Matrix< T, N > &M ) noexcept
    {
        const auto data = static_cast< const T * >( M );
        T ret[ N * N ] = { 0 };
        for ( auto i = 0l; i < N; ++i ) {
            for ( auto j = 0l; j < N; ++j ) {
                ret[ j * N + i ] = data[ i * N + j ];
            }
        }
        return impl::Matrix< T, N >( ret );
    }

    template< typename T, Size N >
    [[nodiscard]] constexpr Real determinant( const impl::Matrix< T, N > &M ) noexcept
    {
        Real result = Real( 1 );
        const auto data = static_cast< const T * >( M );

        if constexpr ( N == 2 ) {
            result = data[ 0 ] * data[ 3 ] - data[ 1 ] * data[ 2 ];
        } else if constexpr ( N == 3 ) {
            result = data[ 0 ] * ( data[ 4 ] * data[ 8 ] - data[ 5 ] * data[ 7 ] )
                     - data[ 1 ] * ( data[ 3 ] * data[ 8 ] - data[ 5 ] * data[ 6 ] )
                     + data[ 2 ] * ( data[ 3 ] * data[ 7 ] - data[ 4 ] * data[ 6 ] );
        } else if constexpr ( N == 4 ) {
            const auto m11 = data[ 0 ];
            const auto m12 = data[ 1 ];
            const auto m13 = data[ 2 ];
            const auto m14 = data[ 3 ];
            const auto m21 = data[ 4 ];
            const auto m22 = data[ 5 ];
            const auto m23 = data[ 6 ];
            const auto m24 = data[ 7 ];
            const auto m31 = data[ 8 ];
            const auto m32 = data[ 9 ];
            const auto m33 = data[ 10 ];
            const auto m34 = data[ 11 ];
            const auto m41 = data[ 12 ];
            const auto m42 = data[ 13 ];
            const auto m43 = data[ 14 ];
            const auto m44 = data[ 15 ];

            result = m11 * ( m22 * ( m33 * m44 - m34 * m43 ) + m23 * ( m34 * m42 - m32 * m44 ) + m24 * ( m32 * m43 - m33 * m42 ) )
                     - m12 * ( m21 * ( m33 * m44 - m34 * m43 ) + m23 * ( m34 * m41 - m31 * m44 ) + m24 * ( m31 * m43 - m33 * m41 ) )
                     + m13 * ( m21 * ( m32 * m44 - m34 * m42 ) + m22 * ( m34 * m41 - m31 * m44 ) + m24 * ( m31 * m42 - m32 * m41 ) )
                     - m14 * ( m21 * ( m32 * m43 - m33 * m42 ) + m22 * ( m33 * m41 - m31 * m43 ) + m23 * ( m31 * m42 - m32 * m41 ) );
        }

        return result;
    }

    template< typename T, Size N >
    [[nodiscard]] constexpr impl::Matrix< T, N > inverse( const impl::Matrix< T, N > &M ) noexcept
    {
        static_assert( N == 3 || N == 4 );

        const auto data = static_cast< const T * >( M );

        if constexpr ( N == 3 ) {
            constexpr auto det = determinant( M );
            assert( det != 0 );
            constexpr auto invDet = Real( 1.0 ) / det;
            return impl::Matrix< T, N > {
                ( data[ 4 ] * data[ 8 ] - data[ 5 ] * data[ 7 ] ) * invDet,
                ( data[ 2 ] * data[ 7 ] - data[ 1 ] * data[ 8 ] ) * invDet,
                ( data[ 1 ] * data[ 5 ] - data[ 2 ] * data[ 4 ] ) * invDet,

                ( data[ 5 ] * data[ 6 ] - data[ 3 ] * data[ 8 ] ) * invDet,
                ( data[ 0 ] * data[ 8 ] - data[ 2 ] * data[ 6 ] ) * invDet,
                ( data[ 2 ] * data[ 3 ] - data[ 0 ] * data[ 5 ] ) * invDet,

                ( data[ 3 ] * data[ 7 ] - data[ 4 ] * data[ 6 ] ) * invDet,
                ( data[ 1 ] * data[ 6 ] - data[ 0 ] * data[ 7 ] ) * invDet,
                ( data[ 0 ] * data[ 4 ] - data[ 1 ] * data[ 3 ] ) * invDet,
            };
        } else if ( N == 4 ) {
            const T a00 = data[ 0 ], a01 = data[ 1 ], a02 = data[ 2 ], a03 = data[ 3 ],
                    a10 = data[ 4 ], a11 = data[ 5 ], a12 = data[ 6 ], a13 = data[ 7 ],
                    a20 = data[ 8 ], a21 = data[ 9 ], a22 = data[ 10 ], a23 = data[ 11 ],
                    a30 = data[ 12 ], a31 = data[ 13 ], a32 = data[ 14 ], a33 = data[ 15 ];

            const T b00 = a00 * a11 - a01 * a10;
            const T b01 = a00 * a12 - a02 * a10;
            const T b02 = a00 * a13 - a03 * a10;
            const T b03 = a01 * a12 - a02 * a11;
            const T b04 = a01 * a13 - a03 * a11;
            const T b05 = a02 * a13 - a03 * a12;
            const T b06 = a20 * a31 - a21 * a30;
            const T b07 = a20 * a32 - a22 * a30;
            const T b08 = a20 * a33 - a23 * a30;
            const T b09 = a21 * a32 - a22 * a31;
            const T b10 = a21 * a33 - a23 * a31;
            const T b11 = a22 * a33 - a23 * a32;

            const Real det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
            assert( det != 0 );
            const Real invDet = Real( 1 ) / det;

            return impl::Matrix< T, N > {
                ( a11 * b11 - a12 * b10 + a13 * b09 ) * invDet,
                ( a02 * b10 - a01 * b11 - a03 * b09 ) * invDet,
                ( a31 * b05 - a32 * b04 + a33 * b03 ) * invDet,
                ( a22 * b04 - a21 * b05 - a23 * b03 ) * invDet,
                ( a12 * b08 - a10 * b11 - a13 * b07 ) * invDet,
                ( a00 * b11 - a02 * b08 + a03 * b07 ) * invDet,
                ( a32 * b02 - a30 * b05 - a33 * b01 ) * invDet,
                ( a20 * b05 - a22 * b02 + a23 * b01 ) * invDet,
                ( a10 * b10 - a11 * b08 + a13 * b06 ) * invDet,
                ( a01 * b08 - a00 * b10 - a03 * b06 ) * invDet,
                ( a30 * b04 - a31 * b02 + a33 * b00 ) * invDet,
                ( a21 * b02 - a20 * b04 - a23 * b00 ) * invDet,
                ( a11 * b07 - a10 * b09 - a12 * b06 ) * invDet,
                ( a00 * b09 - a01 * b07 + a02 * b06 ) * invDet,
                ( a31 * b01 - a30 * b03 - a32 * b00 ) * invDet,
                ( a20 * b03 - a21 * b01 + a22 * b00 ) * invDet,
            };
        }

        return M;
    }

}

#endif
