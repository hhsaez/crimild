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

#ifndef CRIMILD_MATHEMATICS_NORMAL_3_
#define CRIMILD_MATHEMATICS_NORMAL_3_

#include "Foundation/Types.hpp"
#include "Mathematics/Numbers.hpp"
#include "Mathematics/Utils.hpp"
#include "Mathematics/VectorImpl.hpp"

#include <array>
#include <iomanip>
#include <sstream>

namespace crimild {

    namespace impl {

        template< typename T >
        class Normal3 {
        public:
            constexpr Normal3( void ) noexcept = default;

            constexpr explicit Normal3( const T *tuple ) noexcept
                : m_tuple()
            {
                m_tuple[ 0 ] = tuple[ 0 ];
                m_tuple[ 1 ] = tuple[ 1 ];
                m_tuple[ 2 ] = tuple[ 2 ];
            }

            constexpr Normal3( T x, T y, T z ) noexcept
                : m_tuple()
            {
                m_tuple[ 0 ] = x;
                m_tuple[ 1 ] = y;
                m_tuple[ 2 ] = z;
            }

            constexpr Normal3( const Normal3 &other ) noexcept
                : m_tuple( other.m_tuple )
            {
            }

            constexpr Normal3( const Normal3 &&other ) noexcept
                : m_tuple( std::move( other.m_tuple ) )
            {
            }

            ~Normal3( void ) noexcept = default;

            constexpr Normal3 &operator=( const Normal3 &other ) noexcept
            {
                m_tuple = other.m_tuple;
                return *this;
            }

            constexpr Normal3 &operator=( const Normal3 &&other ) noexcept
            {
                m_tuple = std::move( other.m_tuple );
                return *this;
            }

            inline constexpr T x( void ) const noexcept
            {
                return m_tuple[ 0 ];
            }

            inline constexpr T y( void ) const noexcept
            {
                return m_tuple[ 1 ];
            }

            inline constexpr T z( void ) const noexcept
            {
                return m_tuple[ 2 ];
            }

            [[nodiscard]] inline constexpr T operator[]( Size index ) const noexcept
            {
                return m_tuple[ index ];
            }

            [[nodiscard]] inline constexpr Bool operator==( const Normal3 &v ) const noexcept
            {
                return isEqual( x(), v.x() ) && isEqual( y(), v.y() ) && isEqual( z(), v.z() );
            }

            [[nodiscard]] inline constexpr Bool operator!=( const Normal3 &v ) const noexcept
            {
                return !isEqual( x(), v.x() ) || !isEqual( y(), v.y() ) || !isEqual( z(), v.z() );
            }

            template< typename U >
            [[nodiscard]] inline constexpr Normal3 operator+( const Normal3< U > &v ) const noexcept
            {
                return Normal3 {
                    x() + v.x(),
                    y() + v.y(),
                    z() + v.z(),
                };
            }

            template< typename U >
            [[nodiscard]] inline constexpr Normal3 operator-( const Normal3< U > &v ) const noexcept
            {
                return Normal3 {
                    x() - v.x(),
                    y() - v.y(),
                    z() - v.z(),
                };
            }

            template< typename U >
            [[nodiscard]] inline constexpr Normal3 operator*( U scalar ) const noexcept
            {
                return Normal3 {
                    x() * scalar,
                    y() * scalar,
                    z() * scalar,
                };
            }

            [[nodiscard]] friend inline constexpr Normal3 operator*( Real scalar, const Normal3 &u ) noexcept
            {
                return Normal3 {
                    u.x() * scalar,
                    u.y() * scalar,
                    u.z() * scalar,
                };
            }

            template< typename U >
            [[nodiscard]] inline constexpr Normal3 operator/( U scalar ) const noexcept
            {
                return Normal3 {
                    x() / scalar,
                    y() / scalar,
                    z() / scalar,
                };
            }

            [[nodiscard]] inline constexpr Normal3 operator-( void ) const noexcept
            {
                return Normal3 {
                    -x(),
                    -y(),
                    -z(),
                };
            }

            friend inline std::ostream &operator<<( std::ostream &out, const Normal3 &u )
            {
                out << std::setiosflags( std::ios::fixed | std::ios::showpoint )
                    << std::setprecision( 6 );
                out << "("
                    << u.x() << ", "
                    << u.y() << ", "
                    << u.z()
                    << ")";
                return out;
            }

        private:
            std::array< T, 3 > m_tuple;
        };

    }

    template< typename T >
    [[nodiscard]] inline constexpr Bool isNaN( const impl::Normal3< T > &u ) noexcept
    {
        return isNaN( u.x() ) || isNaN( u.y() ) || isNaN( u.z() );
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Normal3< T > abs( const impl::Normal3< T > &u ) noexcept
    {
        return impl::Normal3< T > {
            abs( u.x() ),
            abs( u.y() ),
            abs( u.z() ),
        };
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr T dot( const impl::Normal3< T > &u, const impl::Normal3< U > &v ) noexcept
    {
        return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr T dot( const impl::Normal3< T > &u, const impl::Vector< U, 3 > &v ) noexcept
    {
        return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
    }

    template< typename T, typename U >
    [[nodiscard]] inline constexpr T absDot( const impl::Normal3< T > &u, const impl::Normal3< U > &v ) noexcept
    {
        return abs( dot( u, v ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Real lengthSquared( const impl::Normal3< T > &u ) noexcept
    {
        return dot( u, u );
    }

    template< typename T >
    [[nodiscard]] inline constexpr Real length( const impl::Normal3< T > &u ) noexcept
    {
        return sqrt( lengthSquared( u ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Normal3< T > normalize( const impl::Normal3< T > &u ) noexcept
    {
        return u / length( u );
    }

    template< typename T >
    [[nodiscard]] inline constexpr T min( const impl::Normal3< T > &u ) noexcept
    {
        return min( u.x(), min( u.y(), u.z() ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Normal3< T > min( const impl::Normal3< T > &u, const impl::Normal3< T > &v ) noexcept
    {
        return impl::Normal3< T > {
            min( u.x(), v.x() ),
            min( u.y(), v.y() ),
            min( u.z(), v.z() ),
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr T minDimension( const impl::Normal3< T > &u ) noexcept
    {
        return ( u.x() < u.y() ? ( u.x() < u.z() ? 0 : 2 ) : ( u.y() > u.z() ? 1 : 2 ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr T max( const impl::Normal3< T > &u ) noexcept
    {
        return max( u.x(), max( u.y(), u.z() ) );
    }

    template< typename T >
    [[nodiscard]] inline constexpr impl::Normal3< T > max( const impl::Normal3< T > &u, const impl::Normal3< T > &v ) noexcept
    {
        return impl::Normal3< T > {
            max( u.x(), v.x() ),
            max( u.y(), v.y() ),
            max( u.z(), v.z() ),
        };
    }

    template< typename T >
    [[nodiscard]] inline constexpr T maxDimension( const impl::Normal3< T > &u ) noexcept
    {
        return ( u.x() > u.y() ? ( u.x() > u.z() ? 0 : 2 ) : ( u.y() > u.z() ? 1 : 2 ) );
    }

    using Normal3 = impl::Normal3< Real >;
    using Normal3f = impl::Normal3< Real32 >;
    using Normal3d = impl::Normal3< Real64 >;

}

#endif
