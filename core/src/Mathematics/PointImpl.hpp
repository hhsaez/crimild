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

#ifndef CRIMILD_MATHEMATICS_POINT_IMPL_
#define CRIMILD_MATHEMATICS_POINT_IMPL_

#include "Foundation/Types.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Utils.hpp"
#include "Mathematics/VectorImpl.hpp"

#include <array>
#include <iomanip>
#include <sstream>

namespace crimild {

    namespace impl {

        template< typename T, Size N >
        class Point {
        public:
            constexpr Point( void ) noexcept = default;

            constexpr explicit Point( const std::array< T, N > &tuple ) noexcept
                : m_tuple( tuple )
            {
            }

            constexpr explicit Point( const T *tuple ) noexcept
                : m_tuple()
            {
                for ( auto i = 0l; i < N; ++i ) {
                    m_tuple[ i ] = tuple[ i ];
                }
            }

            constexpr Point( T x, T y ) noexcept
                : m_tuple()
            {
                static_assert( N == 2 );
                m_tuple[ 0 ] = x;
                m_tuple[ 1 ] = y;
            }

            constexpr Point( T x, T y, T z ) noexcept
                : m_tuple()
            {
                static_assert( N == 3 );
                m_tuple[ 0 ] = x;
                m_tuple[ 1 ] = y;
                m_tuple[ 2 ] = z;
            }

            constexpr Point( T x, T y, T z, T w ) noexcept
                : m_tuple()
            {
                static_assert( N == 4 );
                m_tuple[ 0 ] = x;
                m_tuple[ 1 ] = y;
                m_tuple[ 2 ] = z;
                m_tuple[ 3 ] = w;
            }

            constexpr Point( const Point &other ) noexcept
                : m_tuple( other.m_tuple )
            {
            }

            constexpr Point( const Point &&other ) noexcept
                : m_tuple( std::move( other.m_tuple ) )
            {
            }

            ~Point( void ) noexcept = default;

            constexpr Point &operator=( const Point &other ) noexcept
            {
                m_tuple = other.m_tuple;
                return *this;
            }

            constexpr Point &operator=( const Point &&other ) noexcept
            {
                m_tuple = std::move( other.m_tuple );
                return *this;
            }

            inline constexpr T x( void ) const noexcept
            {
                static_assert( N >= 1 );
                return m_tuple[ 0 ];
            }

            inline constexpr T y( void ) const noexcept
            {
                static_assert( N >= 2 );
                return m_tuple[ 1 ];
            }

            inline constexpr T z( void ) const noexcept
            {
                static_assert( N >= 3 );
                return m_tuple[ 2 ];
            }

            inline constexpr T w( void ) const noexcept
            {
                static_assert( N >= 4 );
                return m_tuple[ 3 ];
            }

            [[nodiscard]] inline constexpr T operator[]( Size index ) const noexcept
            {
                return m_tuple[ index ];
            }

            [[nodiscard]] inline constexpr Bool operator==( const Point &v ) const noexcept
            {
                auto ret = true;
                for ( auto i = 0l; i < N; ++i ) {
                    ret = ret && isEqual( ( *this )[ i ], v[ i ] );
                }
                return ret;
            }

            [[nodiscard]] inline constexpr Bool operator!=( const Point &v ) const noexcept
            {
                return !( *this == v );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Point operator+( const Vector< U, N > &v ) const noexcept
            {
                T tuple[ N ];
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] + v[ i ];
                }
                return Point( tuple );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Vector< T, N > operator-( const Point< U, N > &v ) const noexcept
            {
                T tuple[ N ];
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] - v[ i ];
                }
                return Vector< T, N >( tuple );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Point operator-( const Vector< U, N > &v ) const noexcept
            {
                T tuple[ N ];
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] - v[ i ];
                }
                return Point( tuple );
            }

            friend inline std::ostream &operator<<( std::ostream &out, const Point &u )
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

    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Bool isNaN( const impl::Point< T, N > &u ) noexcept
    {
        auto ret = false;
        for ( auto i = 0l; i < N; ++i ) {
            ret = ret || isNaN( u[ i ] );
        }
        return ret;
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Point< T, N > abs( const impl::Point< T, N > &u ) noexcept
    {
        T tuple[ N ];
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = abs( u[ i ] );
        }
        return impl::Point< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Point< T, N > floor( const impl::Point< T, N > &u ) noexcept
    {
        T tuple[ N ];
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = floor( u[ i ] );
        }
        return impl::Point< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Point< T, N > ceil( const impl::Point< T, N > &u ) noexcept
    {
        T tuple[ N ];
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = ceil( u[ i ] );
        }
        return impl::Point< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Point< T, N > lerp( const impl::Point< T, N > &u, const impl::Point< T, N > &v, Real t ) noexcept
    {
        T tuple[ N ];
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = lerp( u[ i ], v[ i ], t );
        }
        return impl::Point< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Real distanceSquared( const impl::Point< T, N > &u, const impl::Point< T, N > &v ) noexcept
    {
        return lengthSquared( v - u );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Real distance( const impl::Point< T, N > &u, const impl::Point< T, N > &v ) noexcept
    {
        return sqrt( lengthSquared( v - u ) );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr T min( const impl::Point< T, N > &u ) noexcept
    {
        auto ret = u[ 0 ];
        for ( auto i = 1l; i < N; ++i ) {
            ret = min( ret, u[ i ] );
        }
        return ret;
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Point< T, N > min( const impl::Point< T, N > &u, const impl::Point< T, N > &v ) noexcept
    {
        T tuple[ N ];
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = min( u[ i ], v[ i ] );
        }
        return impl::Point< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Size minDimension( const impl::Point< T, N > &u ) noexcept
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
    [[nodiscard]] inline constexpr T max( const impl::Point< T, N > &u ) noexcept
    {
        auto ret = u[ 0 ];
        for ( auto i = 1l; i < N; ++i ) {
            ret = max( ret, u[ i ] );
        }
        return ret;
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Point< T, N > max( const impl::Point< T, N > &u, const impl::Point< T, N > &v ) noexcept
    {
        T tuple[ N ];
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = max( u[ i ], v[ i ] );
        }
        return impl::Point< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Size maxDimension( const impl::Point< T, N > &u ) noexcept
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
