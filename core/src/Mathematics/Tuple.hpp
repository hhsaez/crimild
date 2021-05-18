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

#ifndef CRIMILD_MATHEMATICS_TUPLE_
#define CRIMILD_MATHEMATICS_TUPLE_

#include "Foundation/Types.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Utils.hpp"

#include <array>
#include <iomanip>
#include <iostream>

namespace crimild {

    namespace impl {

        template< typename T, Size N >
        class Tuple {
        public:
            constexpr Tuple( void ) noexcept = default;

            constexpr explicit Tuple( const std::array< T, N > &data ) noexcept
                : m_tuple( data )
            {
            }

            constexpr Tuple( T x ) noexcept
                : m_tuple()
            {
                for ( auto i = 0l; i < N; ++i ) {
                    m_tuple[ i ] = x;
                }
            }

            constexpr Tuple( T x, T y ) noexcept
                : m_tuple()
            {
                static_assert( N == 2 );
                m_tuple[ 0 ] = x;
                m_tuple[ 1 ] = y;
            }

            constexpr Tuple( T x, T y, T z ) noexcept
                : m_tuple()
            {
                static_assert( N == 3 );
                m_tuple[ 0 ] = x;
                m_tuple[ 1 ] = y;
                m_tuple[ 2 ] = z;
            }

            constexpr Tuple( T x, T y, T z, T w ) noexcept
                : m_tuple()
            {
                static_assert( N == 4 );
                m_tuple[ 0 ] = x;
                m_tuple[ 1 ] = y;
                m_tuple[ 2 ] = z;
                m_tuple[ 3 ] = w;
            }

            constexpr Tuple( const Tuple &other ) noexcept
                : m_tuple( other.m_tuple )
            {
            }

            constexpr Tuple( Tuple &&other ) noexcept
                : m_tuple( std::move( other.m_tuple ) )
            {
            }

            ~Tuple( void ) noexcept = default;

            constexpr Tuple &operator=( const Tuple &other ) noexcept
            {
                m_tuple = other.m_tuple;
                return *this;
            }

            constexpr Tuple &operator=( Tuple &&other ) noexcept
            {
                m_tuple = std::move( other.m_tuple );
                return *this;
            }

            [[nodiscard]] inline constexpr T operator[]( size_t i ) const noexcept { return m_tuple[ i ]; }

            [[nodiscard]] inline constexpr T x( void ) const noexcept { return m_tuple[ 0 ]; }
            [[nodiscard]] inline constexpr T y( void ) const noexcept { return m_tuple[ 1 ]; }
            [[nodiscard]] inline constexpr T z( void ) const noexcept { return m_tuple[ 2 ]; }
            [[nodiscard]] inline constexpr T w( void ) const noexcept { return m_tuple[ 3 ]; }

        private:
            std::array< T, N > m_tuple = { 0 };
        };

        template< class TupleImpl, typename T, Size N >
        struct TupleOps {
            [[nodiscard]] inline constexpr Bool operator==( const TupleImpl &v ) const noexcept
            {
                const auto &tuple = *( const TupleImpl * ) ( this );

                auto ret = true;
                for ( auto i = 0l; i < N; ++i ) {
                    ret = ret && isEqual( tuple[ i ], v[ i ] );
                }
                return ret;
            }

            [[nodiscard]] inline constexpr Bool operator!=( const TupleImpl &v ) const noexcept
            {
                return !( *this == v );
            }

            [[nodiscard]] inline friend constexpr TupleImpl operator+( const TupleImpl &a, const TupleImpl &b ) noexcept
            {
                std::array< T, N > tuple = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = a[ i ] + b[ i ];
                }
                return TupleImpl( tuple );
            }

            [[nodiscard]] inline friend constexpr TupleImpl operator*( const TupleImpl &u, Real scalar ) noexcept
            {
                std::array< T, N > tuple = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = u[ i ] * scalar;
                }
                return TupleImpl( tuple );
            }

            [[nodiscard]] friend inline constexpr TupleImpl operator*( Real64 scalar, const TupleImpl &u ) noexcept
            {
                std::array< T, N > tuple = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = u[ i ] * scalar;
                }
                return TupleImpl( tuple );
            }

            friend inline std::ostream &operator<<( std::ostream &out, const TupleImpl &u )
            {
                out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
                    << std::setprecision( 6 );
                out << "(";
                for ( auto i = 0l; i < N; ++i ) {
                    if ( i > 0 ) {
                        out << ", ";
                    }
                    out << u[ i ];
                }
                out << ")";
                return out;
            }
        };

        template< template< typename, Size > class TupleImpl, typename T, Size N >
        struct TupleSwizzle {
            [[nodiscard]] inline constexpr TupleImpl< T, 2 > xy( void ) const noexcept
            {
                static_assert( N >= 2 );
                const auto &tuple = *( const TupleImpl< T, N > * ) ( this );
                return TupleImpl< T, 2 > { tuple.x(), tuple.y() };
            }

            [[nodiscard]] inline constexpr TupleImpl< T, 3 > xyz( void ) const noexcept
            {
                static_assert( N >= 3 );
                const auto &tuple = *( const TupleImpl< T, N > * ) ( this );
                return TupleImpl< T, 3 > { tuple.x(), tuple.y(), tuple.z() };
            }

            [[nodiscard]] inline constexpr TupleImpl< T, 3 > xxx( void ) const noexcept
            {
                static_assert( N >= 3 );
                const auto &tuple = *( const TupleImpl< T, N > * ) ( this );
                return TupleImpl< T, 3 > { tuple.x(), tuple.x(), tuple.x() };
            }

            [[nodiscard]] inline constexpr TupleImpl< T, 3 > yyy( void ) const noexcept
            {
                static_assert( N >= 3 );
                const auto &tuple = *( const TupleImpl< T, N > * ) ( this );
                return TupleImpl< T, 3 > { tuple.y(), tuple.y(), tuple.y() };
            }

            [[nodiscard]] inline constexpr TupleImpl< T, 3 > zzz( void ) const noexcept
            {
                static_assert( N >= 3 );
                const auto &tuple = *( const TupleImpl< T, N > * ) ( this );
                return TupleImpl< T, 3 > { tuple.z(), tuple.z(), tuple.z() };
            }

            [[nodiscard]] inline constexpr TupleImpl< T, 4 > xyzw( void ) const noexcept
            {
                static_assert( N >= 4 );
                const auto &tuple = *( const TupleImpl< T, N > * ) ( this );
                return TupleImpl< T, 4 > { tuple.x(), tuple.y(), tuple.z(), tuple.w() };
            }

            [[nodiscard]] inline constexpr TupleImpl< T, 4 > xyzz( void ) const noexcept
            {
                static_assert( N >= 3 );
                const auto &tuple = *( const TupleImpl< T, N > * ) ( this );
                return TupleImpl< T, 4 > { tuple.x(), tuple.y(), tuple.z(), tuple.z() };
            }
        };

    }

}

#endif
