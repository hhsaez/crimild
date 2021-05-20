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

#ifndef CRIMILD_MATHEMATICS_VECTOR_IMPL_
#define CRIMILD_MATHEMATICS_VECTOR_IMPL_

#include "Mathematics/Tuple.hpp"

namespace crimild {

    namespace impl {

        template< typename T, Size N >
        class Vector
            : public Tuple< T, N >,
              public TupleOps< Vector< T, N >, T, N >,
              public TupleSwizzle< Vector, T, N > {
        private:
            using Base = Tuple< T, N >;

        public:
            struct Constants;

        public:
            constexpr Vector( void ) noexcept = default;

            constexpr explicit Vector( const std::array< T, N > &tuple ) noexcept
                : Base( tuple )
            {
            }

            constexpr Vector( T x ) noexcept
                : Base( x )
            {
            }

            constexpr Vector( T x, T y ) noexcept
                : Base( x, y )
            {
            }

            constexpr Vector( T x, T y, T z ) noexcept
                : Base( x, y, z )
            {
            }

            constexpr Vector( T x, T y, T z, T w ) noexcept
                : Base( x, y, z, w )
            {
            }

            constexpr Vector( const Tuple< T, 2 > &t, T z )
                : Base( t.x(), t.y(), z )
            {
            }

            constexpr Vector( const Tuple< T, 2 > &t, T z, T w )
                : Base( t.x(), t.y(), z, w )
            {
            }

            constexpr Vector( const Tuple< T, 3 > &t, T w )
                : Base( t.x(), t.y(), t.z(), w )
            {
            }

            constexpr Vector( const Vector &other ) noexcept
                : Base( other )
            {
            }

            constexpr Vector( Vector &&other ) noexcept
                : Base( other )
            {
            }

            constexpr explicit Vector( const Base &other ) noexcept
                : Base( other )
            {
            }

            ~Vector( void ) noexcept = default;

            inline constexpr Vector &operator=( const Vector &other ) noexcept
            {
                Tuple< T, N >::operator=( other );
                return *this;
            }

            inline constexpr Vector &operator=( Vector &&other ) noexcept
            {
                Tuple< T, N >::operator=( other );
                return *this;
            }

            template< typename U >
            [[nodiscard]] inline constexpr Vector operator-( const Vector< U, N > &v ) const noexcept
            {
                std::array< T, N > tuple = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] - v[ i ];
                }
                return Vector( tuple );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Vector operator*( const Vector< U, N > &v ) const noexcept
            {
                std::array< T, N > tuple = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] * v[ i ];
                }
                return Vector( tuple );
            }

            [[nodiscard]] inline constexpr Vector operator/( Real scalar ) const noexcept
            {
                std::array< T, N > tuple = { 0 };
                const auto invScalar = Real( 1 ) / scalar;
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = ( *this )[ i ] * invScalar;
                }
                return Vector( tuple );
            }

            [[nodiscard]] inline constexpr Vector operator-( void ) const noexcept
            {
                std::array< T, N > tuple = { 0 };
                for ( auto i = 0l; i < N; ++i ) {
                    tuple[ i ] = -( *this )[ i ];
                }
                return Vector( tuple );
            }
        };

        template< typename T, Size N >
        struct Vector< T, N >::Constants {
            static constexpr auto ZERO = [] {
                return Vector< T, N >( 0 );
            }();

            static constexpr auto ONE = [] {
                return Vector< T, N >( 1 );
            }();

            static constexpr auto POSITIVE_INFINITY = [] {
                return Vector< T, N >( std::numeric_limits< T >::max() );
            }();

            static constexpr auto NEGATIVE_INFINITY = [] {
                return Vector< T, N >( std::numeric_limits< T >::min() );
            }();

            static constexpr auto UNIT_X = [] {
                if constexpr ( N == 2 ) {
                    return Vector< T, 2 >( 1, 0 );
                } else if constexpr ( N == 3 ) {
                    return Vector< T, 3 >( 1, 0, 0 );
                } else if constexpr ( N == 4 ) {
                    return Vector< T, 4 >( 1, 0, 0, 0 );
                } else {
                    return Vector< T, N > { 0 };
                }
            }();

            static constexpr auto UNIT_Y = [] {
                if constexpr ( N == 2 ) {
                    return Vector< T, 2 >( 0, 1 );
                } else if constexpr ( N == 3 ) {
                    return Vector< T, 3 >( 0, 1, 0 );
                } else if constexpr ( N == 4 ) {
                    return Vector< T, 4 >( 0, 1, 0, 0 );
                } else {
                    return Vector< T, N > { 0 };
                }
            }();

            static constexpr auto UNIT_Z = [] {
                if constexpr ( N == 3 ) {
                    return Vector< T, 3 >( 0, 0, 1 );
                } else if constexpr ( N == 4 ) {
                    return Vector< T, 4 >( 0, 0, 1, 0 );
                } else {
                    return Vector< T, N > { 0 };
                }
            }();

            static constexpr auto UNIT_W = [] {
                if constexpr ( N == 4 ) {
                    return Vector< T, 4 >( 0, 0, 0, 1 );
                } else {
                    return Vector< T, N > { 0 };
                }
            }();
        };

    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Bool isNaN( const impl::Vector< T, N > &u ) noexcept
    {
        auto ret = false;
        for ( auto i = 0l; i < N; ++i ) {
            ret = ret || isNaN( u[ i ] );
        }
        return ret;
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Vector< T, N > abs( const impl::Vector< T, N > &u ) noexcept
    {
        std::array< T, N > tuple = { 0 };
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = abs( u[ i ] );
        }
        return impl::Vector< T, N >( tuple );
    }

    template< typename T, typename U, Size N >
    [[nodiscard]] inline constexpr T dot( const impl::Vector< T, N > &u, const impl::Vector< U, N > &v ) noexcept
    {
        auto ret = T( 0 );
        for ( auto i = 0l; i < N; ++i ) {
            ret += u[ i ] * v[ i ];
        }
        return ret;
    }

    template< typename T, typename U, Size N >
    [[nodiscard]] inline constexpr T absDot( const impl::Vector< T, N > &u, const impl::Vector< U, N > &v ) noexcept
    {
        return abs( dot( u, v ) );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Real lengthSquared( const impl::Vector< T, N > &u ) noexcept
    {
        return dot( u, u );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Real length( const impl::Vector< T, N > &u ) noexcept
    {
        return sqrt( lengthSquared( u ) );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Vector< T, N > normalize( const impl::Vector< T, N > &u ) noexcept
    {
        return u / length( u );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr T min( const impl::Vector< T, N > &u ) noexcept
    {
        auto ret = u[ 0 ];
        for ( auto i = 1l; i < N; ++i ) {
            ret = min( ret, u[ i ] );
        }
        return ret;
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Vector< T, N > min( const impl::Vector< T, N > &u, const impl::Vector< T, N > &v ) noexcept
    {
        std::array< T, N > tuple = { 0 };
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = min( u[ i ], v[ i ] );
        }
        return impl::Vector< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Size minDimension( const impl::Vector< T, N > &u ) noexcept
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
    [[nodiscard]] inline constexpr T max( const impl::Vector< T, N > &u ) noexcept
    {
        auto ret = u[ 0 ];
        for ( auto i = 1l; i < N; ++i ) {
            ret = max( ret, u[ i ] );
        }
        return ret;
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr impl::Vector< T, N > max( const impl::Vector< T, N > &u, const impl::Vector< T, N > &v ) noexcept
    {
        std::array< T, N > tuple = { 0 };
        for ( auto i = 0l; i < N; ++i ) {
            tuple[ i ] = max( u[ i ], v[ i ] );
        }
        return impl::Vector< T, N >( tuple );
    }

    template< typename T, Size N >
    [[nodiscard]] inline constexpr Size maxDimension( const impl::Vector< T, N > &u ) noexcept
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
