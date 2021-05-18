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
        class Point
            : public Tuple< T, N >,
              public TupleOps< Point< T, N >, T, N > {
        private:
            using Base = Tuple< T, N >;

        public:
            constexpr Point( void ) noexcept = default;

            constexpr explicit Point( const std::array< T, N > &tuple ) noexcept
                : Base( tuple )
            {
            }

            constexpr Point( T x ) noexcept
                : Base( x )
            {
            }

            constexpr Point( T x, T y ) noexcept
                : Base( x, y )
            {
            }

            constexpr Point( T x, T y, T z ) noexcept
                : Base( x, y, z )
            {
            }

            constexpr Point( const Point &other ) noexcept
                : Base( other )
            {
            }

            constexpr Point( Point &&other ) noexcept
                : Base( other )
            {
            }

            constexpr explicit Point( const Base &other ) noexcept
                : Base( other )
            {
            }

            ~Point( void ) noexcept = default;

            inline constexpr Point &operator=( const Point &other ) noexcept
            {
                Tuple< T, N >::operator=( other );
                return *this;
            }

            inline constexpr Point &operator=( Point &&other ) noexcept
            {
                Tuple< T, N >::operator=( other );
                return *this;
            }

            [[nodiscard]] inline constexpr Vector< T, 4 > xyzw( void ) const noexcept
            {
                static_assert( N == 3 );
                return Vector< T, 4 >( Point::x(), Point::y(), Point::z(), 1 );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Point operator+( const Vector< U, N > &v ) const noexcept
            {
                std::array< T, N > tuple = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] + v[ i ];
                }
                return Point( tuple );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Vector< T, N > operator-( const Point< U, N > &v ) const noexcept
            {
                std::array< T, N > tuple = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] - v[ i ];
                }
                return Vector< T, N >( tuple );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Point operator-( const Vector< U, N > &v ) const noexcept
            {
                std::array< T, N > tuple = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] - v[ i ];
                }
                return Point( tuple );
            }
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
        std::array< T, N > tuple = { 0 };
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = abs( u[ i ] );
        }
        return impl::Point< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Point< T, N > floor( const impl::Point< T, N > &u ) noexcept
    {
        std::array< T, N > tuple = { 0 };
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = floor( u[ i ] );
        }
        return impl::Point< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Point< T, N > ceil( const impl::Point< T, N > &u ) noexcept
    {
        std::array< T, N > tuple = { 0 };
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = ceil( u[ i ] );
        }
        return impl::Point< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Point< T, N > lerp( const impl::Point< T, N > &u, const impl::Point< T, N > &v, Real t ) noexcept
    {
        return ( Real( 1 ) - t ) * u + t * v;
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
        std::array< T, N > tuple = { 0 };
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
        std::array< T, N > tuple = { 0 };
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
