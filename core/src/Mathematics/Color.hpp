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

#ifndef CRIMILD_MATHEMATICS_COLOR_IMPL_
#define CRIMILD_MATHEMATICS_COLOR_IMPL_

#include "Foundation/Types.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Tuple.hpp"
#include "Mathematics/Utils.hpp"

#include <array>
#include <iomanip>
#include <sstream>

namespace crimild {

    namespace impl {

        template< typename T, Size N >
        class Color {
        public:
            struct Constants;

        public:
            constexpr Color( void ) noexcept = default;

            constexpr explicit Color( const std::array< T, N > &tuple ) noexcept
                : m_tuple( tuple )
            {
            }

            constexpr explicit Color( const T *tuple ) noexcept
                : m_tuple()
            {
                for ( auto i = 0l; i < N; ++i ) {
                    m_tuple[ i ] = tuple[ i ];
                }
            }

            constexpr explicit Color( const Tuple< T, N > &tuple ) noexcept
                : m_tuple()
            {
                for ( auto i = 0l; i < N; ++i ) {
                    m_tuple[ i ] = tuple[ i ];
                }
            }

            constexpr Color( T r ) noexcept
                : m_tuple()
            {
                for ( auto i = 0l; i < N; ++i ) {
                    m_tuple[ i ] = r;
                }
            }

            constexpr Color( T x, T y, T z ) noexcept
                : m_tuple()
            {
                static_assert( N == 3 );
                m_tuple[ 0 ] = x;
                m_tuple[ 1 ] = y;
                m_tuple[ 2 ] = z;
            }

            constexpr Color( T x, T y, T z, T w ) noexcept
                : m_tuple()
            {
                static_assert( N == 4 );
                m_tuple[ 0 ] = x;
                m_tuple[ 1 ] = y;
                m_tuple[ 2 ] = z;
                m_tuple[ 3 ] = w;
            }

            constexpr Color( const Color &other ) noexcept
                : m_tuple( other.m_tuple )
            {
            }

            constexpr Color( const Color &&other ) noexcept
                : m_tuple( std::move( other.m_tuple ) )
            {
            }

            ~Color( void ) noexcept = default;

            constexpr Color &operator=( const Color &other ) noexcept
            {
                m_tuple = other.m_tuple;
                return *this;
            }

            constexpr Color &operator=( const Color &&other ) noexcept
            {
                m_tuple = std::move( other.m_tuple );
                return *this;
            }

            inline constexpr T r( void ) const noexcept
            {
                static_assert( N >= 3 );
                return m_tuple[ 0 ];
            }

            inline constexpr T g( void ) const noexcept
            {
                static_assert( N >= 3 );
                return m_tuple[ 1 ];
            }

            inline constexpr T b( void ) const noexcept
            {
                static_assert( N >= 3 );
                return m_tuple[ 2 ];
            }

            inline constexpr T a( void ) const noexcept
            {
                if constexpr ( N == 4 ) {
                    return m_tuple[ 3 ];
                } else {
                    return T( 1 );
                }
            }

            inline constexpr Color< T, 3 > rgb( void ) const noexcept { return Color< T, 3 > { r(), g(), b() }; }
            inline constexpr Color< T, 4 > rgba( void ) const noexcept { return Color< T, 4 > { r(), g(), b(), a() }; }

            [[nodiscard]] inline constexpr T operator[]( Size index ) const noexcept
            {
                return m_tuple[ index ];
            }

            [[nodiscard]] inline constexpr Bool operator==( const Color &v ) const noexcept
            {
                auto ret = true;
                for ( auto i = 0l; i < N; ++i ) {
                    ret = ret && isEqual( ( *this )[ i ], v[ i ] );
                }
                return ret;
            }

            [[nodiscard]] inline constexpr Bool operator!=( const Color &v ) const noexcept
            {
                return !( *this == v );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Color operator+( const Color< U, N > &v ) const noexcept
            {
                T tuple[ N ];
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] + v[ i ];
                }
                return Color( tuple );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Color< T, N > operator-( const Color< U, N > &v ) const noexcept
            {
                T tuple[ N ];
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] - v[ i ];
                }
                return Color< T, N >( tuple );
            }

            [[nodiscard]] inline constexpr Color operator*( Real scalar ) const noexcept
            {
                T tuple[ N ];
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] * scalar;
                }
                return Color( tuple );
            }

            [[nodiscard]] friend inline constexpr Color operator*( Real64 scalar, const Color &u ) noexcept
            {
                T tuple[ N ];
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = u[ i ] * scalar;
                }
                return Color( tuple );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Color< T, N > operator*( const Color< U, N > &v ) const noexcept
            {
                T tuple[ N ];
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] * v[ i ];
                }
                return Color< T, N >( tuple );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Color operator/( U scalar ) const noexcept
            {
                T tuple[ N ];
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] / scalar;
                }
                return Color( tuple );
            }

            friend inline std::ostream &operator<<( std::ostream &out, const Color &u )
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

        private:
            std::array< T, 4 > m_tuple;
        };

        template< typename T, Size N >
        struct Color< T, N >::Constants {
            static constexpr auto BLACK = [] {
                return Color< T, N >( T( 0 ) );
            }();

            static constexpr auto WHITE = [] {
                return Color< T, N >( T( 1 ) );
            }();

            static constexpr auto RED = [] {
                static_assert( N >= 3 && N <= 4 );
                if constexpr ( N == 3 ) {
                    return Color< T, 3 >( 1, 0, 0 );
                } else {
                    return Color< T, 4 >( 1, 0, 0, 1 );
                }
            }();

            static constexpr auto GREEN = [] {
                static_assert( N >= 3 && N <= 4 );
                if constexpr ( N == 3 ) {
                    return Color< T, 3 >( 0, 1, 0 );
                } else {
                    return Color< T, 4 >( 0, 1, 0, 1 );
                }
            }();

            static constexpr auto BLUE = [] {
                static_assert( N >= 3 && N <= 4 );
                if constexpr ( N == 3 ) {
                    return Color< T, 3 >( 0, 0, 1 );
                } else {
                    return Color< T, 4 >( 0, 0, 1, 1 );
                }
            }();
        };
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Bool isNaN( const impl::Color< T, N > &u ) noexcept
    {
        auto ret = false;
        for ( auto i = 0l; i < N; ++i ) {
            ret = ret || isNaN( u[ i ] );
        }
        return ret;
    }

    template< typename T, typename U, Size N >
    [[nodiscard]] inline constexpr T dot( const impl::Color< T, N > &u, const impl::Color< U, N > &v ) noexcept
    {
        auto ret = T( 0 );
        for ( auto i = 0l; i < N; ++i ) {
            ret += u[ i ] * v[ i ];
        }
        return ret;
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Color< T, N > lerp( const impl::Color< T, N > &u, const impl::Color< T, N > &v, Real t ) noexcept
    {
        T tuple[ N ];
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = lerp( u[ i ], v[ i ], t );
        }
        return impl::Color< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr T min( const impl::Color< T, N > &u ) noexcept
    {
        auto ret = u[ 0 ];
        for ( auto i = 1l; i < N; ++i ) {
            ret = min( ret, u[ i ] );
        }
        return ret;
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Color< T, N > min( const impl::Color< T, N > &u, const impl::Color< T, N > &v ) noexcept
    {
        T tuple[ N ];
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = min( u[ i ], v[ i ] );
        }
        return impl::Color< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Size minDimension( const impl::Color< T, N > &u ) noexcept
    {
        auto ret = Size( 0 );
        for ( auto i = 1l; i < N; ++i ) {
            if ( u[ i ] < u[ ret ] ) {
                ret = i;
            }
        }
        return ret;
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr T max( const impl::Color< T, N > &u ) noexcept
    {
        auto ret = u[ 0 ];
        for ( auto i = 1l; i < N; ++i ) {
            ret = max( ret, u[ i ] );
        }
        return ret;
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Color< T, N > max( const impl::Color< T, N > &u, const impl::Color< T, N > &v ) noexcept
    {
        T tuple[ N ];
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = max( u[ i ], v[ i ] );
        }
        return impl::Color< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Size maxDimension( const impl::Color< T, N > &u ) noexcept
    {
        auto ret = Size( 0 );
        for ( auto i = 1l; i < N; ++i ) {
            if ( u[ i ] > u[ ret ] ) {
                ret = i;
            }
        }
        return ret;
    }

}

#endif
