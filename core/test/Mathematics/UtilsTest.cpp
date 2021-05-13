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

#include "Mathematics/Utils.hpp"

#include "gtest/gtest.h"
#include <array>

using namespace crimild;

TEST( Utils, sqrt )
{
    EXPECT_EQ( Real( 5.3851648071 ), crimild::sqrt( Real( 29 ) ) );

    static_assert( Real( 5.3851648071 ) == crimild::sqrt( Real( 29 ) ), "sqrt" );
}

namespace crimild {

    template< typename T, Size N >
    class LookupTable {
    public:
        constexpr LookupTable( void ) = default;
        ~LookupTable( void ) = default;

        constexpr LookupTable( const LookupTable &other ) noexcept
            : m_table( other.m_table )
        {
        }

        constexpr LookupTable( LookupTable &&other ) noexcept
            : m_table( std::move( other.m_table ) )
        {
        }

        template< typename Fn >
        constexpr explicit LookupTable( Fn fn ) noexcept
            : m_table()
        {
            for ( auto i = 0l; i < N; ++i ) {
                m_table[ i ] = fn( i );
            }
        }

        constexpr LookupTable &operator=( const LookupTable &other ) noexcept
        {
            return *this;
        }

        constexpr LookupTable &operator=( LookupTable &other ) noexcept
        {
            return *this;
        }

    private:
        std::array< T, N > m_table;
    };

    [[nodiscard]] constexpr UInt64 factorial( UInt64 N ) noexcept
    {
        if ( N == 0 ) {
            return 1;
        }

        auto ret = UInt64( 1 );
        for ( auto i = UInt64( 1 ); i <= N; ++i ) {
            ret *= i;
        }
        return ret;
    }

    [[nodiscard]] constexpr Real pow( Real base, Int exp ) noexcept
    {
        auto ret = Real( 1 );
        for ( auto i = Int( 0 ); i < exp; ++i ) {
            ret *= base;
        }
        return ret;
    }

    [[nodiscard]] Real pow( Real base, Real exp ) noexcept
    {
        return std::pow( base, exp );
    }
}

TEST( Utils, factorial )
{
    static_assert( 1 == crimild::factorial( 1 ) );
    static_assert( 2 == crimild::factorial( 2 ) );
    static_assert( 6 == crimild::factorial( 3 ) );
    static_assert( 24 == crimild::factorial( 4 ) );
    static_assert( 120 == crimild::factorial( 5 ) );
    static_assert( 720 == crimild::factorial( 6 ) );
    static_assert( 5040 == crimild::factorial( 7 ) );
    static_assert( 40320 == crimild::factorial( 8 ) );
    static_assert( 362880 == crimild::factorial( 9 ) );
    static_assert( 3628800 == crimild::factorial( 10 ) );

    EXPECT_TRUE( true );
}

TEST( Utils, pow )
{
    static_assert( 1 == crimild::pow( 5, 0 ) );
    static_assert( 2 == crimild::pow( 2, 1 ) );
    static_assert( 4 == crimild::pow( 2, 2 ) );
    static_assert( 8 == crimild::pow( 2, 3 ) );
    static_assert( 16 == crimild::pow( 2, 4 ) );

    EXPECT_TRUE( true );
}

TEST( Utils, cos )
{
#if 0
    constexpr auto cosLUT = crimild::LookupTable< Real, 36000 >(
        []( auto x ) {
            /*
            // use a Taylor series to aproximate cos values
            auto ret = Real( 0 );
            for ( auto n = Int( 0 ); n < 2; ++n ) {
                ret += crimild::pow( -1, n ) * crimild::pow( Int64( x ), Int64( 2 * n ) ) / crimild::factorial( 2 * n );
            }
            return ret;
            */
            return 1;
        } );

    const auto cosTable = []() {
        std::array< Real, 36000 > values;
        for ( auto i = 0l; i < 36000; ++i ) {
            values[ i ] = std::cos( radians( Real( 0.01 * i ) ) );
        }
        return values;
    }();

    auto fastCos = [cosTable]( auto rad ) constexpr
    {
        //const auto deg = Real( 100 ) * crimild::degrees( rad );
        //   const auto deg0 = crimild::floor( Real( 100 ) * crimild::degrees( rad ) );
        //  const auto deg1 = crimild::ceil( Real( 100 ) * crimild::degrees( rad ) );
        //  const auto c = crimild::lerp( cosTable[ UInt32( deg0 ) ], cosTable[ UInt32( deg1 ) ], crimild::min( Real( 1 ), deg - deg0 ) );
        //  return c;
        Real ret = 0;
        for ( auto n = Int( 0 ); n < 20; ++n ) {
            ret += crimild::pow( -1, n ) * crimild::pow( rad, 2 * n ) / crimild::factorial( 2 * n );
        }
        return ret;
    };

    {
        constexpr auto i = Real( 0.1 );
        const auto expected = std::cos( i );
        constexpr auto c = fastCos( i );
        EXPECT_TRUE( crimild::isEqual( c, expected ) );
    }

    {
        constexpr auto i = Real( 0.01 );
        const auto expected = std::cos( i );
        constexpr auto c = fastCos( i );
        EXPECT_TRUE( crimild::isEqual( c, expected ) );
    }

    {
        constexpr auto i = Real( 0.001 );
        const auto expected = std::cos( i );
        constexpr auto c = fastCos( i );
        EXPECT_TRUE( crimild::isEqual( c, expected ) );
    }

    for ( auto i = Real( 0 ); i < numbers::TWO_PI; i += Real( 0.1 ) ) {
        const auto expected = std::cos( i );
        const auto c = fastCos( i );
        EXPECT_TRUE( crimild::isEqual( c, expected ) );
        std::cout << c << " " << expected << "\n";
        //EXPECT_EQ( expected, c );
    }

#endif

    EXPECT_TRUE( true );
}
