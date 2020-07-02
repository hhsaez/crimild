/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Foundation/Types.hpp"
#include "Foundation/Containers/Array.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( ArrayTest, construction )
{
	Array< int > a( 10 );

	EXPECT_EQ( 10, a.size() );
	EXPECT_FALSE( a.empty() );

	a[ 0 ] = 1;

	EXPECT_EQ( 1, a[ 0 ] );
}

TEST( ArrayTest, initializationList )
{
	Array< int > a = { 1, 2, 3, 4, 5 };

	EXPECT_EQ( 1, a[ 0 ] );
	EXPECT_EQ( 2, a[ 1 ] );
	EXPECT_EQ( 3, a[ 2 ] );
	EXPECT_EQ( 4, a[ 3 ] );
	EXPECT_EQ( 5, a[ 4 ] );
}

TEST( ArrayTest, foreach )
{
	Array< int > a( 10 );
	for ( int i = 0; i < 10; i++ ) {
		a[ i ] = i + 1;
	}

	auto sum = 10 * 11 / 2;
	a.each( [&sum]( int &e ) {
		sum -= e;
	});

	EXPECT_EQ( 0, sum );
}

TEST( ArrayTest, add )
{
	Array< int > a;

	a.add( 1 );
	EXPECT_EQ( a[ 0 ], 1 );
	EXPECT_EQ( 1, a.size() );

	a.add( 2 );
	EXPECT_EQ( a[ 1 ], 2 );
	EXPECT_EQ( 2, a.size() );

	a.add( 3 );
	EXPECT_EQ( a[ 2 ], 3 );
	EXPECT_EQ( 3, a.size() );
}

TEST( ArrayTest, remove )
{
	Array< int > a;
	a.add( 1 );
	a.add( 2 );
	a.add( 3 );

	EXPECT_EQ( a[ 0 ], 1 );
	EXPECT_EQ( a[ 1 ], 2 );
	EXPECT_EQ( a[ 2 ], 3 );

	a.remove( 1 );

	EXPECT_EQ( a[ 0 ], 2 );
	EXPECT_EQ( a[ 1 ], 3 );
}

TEST( ArrayTest, removeAt )
{
	auto a = Array< int >{ 0, 1, 2, 3, 4, 5 };
	auto b = Array< int >{ 0, 1, 2, 3, 4, 5 };
	auto c = Array< int >{ 0, 1, 2, 4, 5 };

	EXPECT_EQ( b, a );

	a.removeAt( 3 );

	EXPECT_EQ( c, a );
}

TEST( ArrayTest, sort )
{
    auto a = Array< int >{ 0, 3, 1, 2, 5, 4 };
    auto sorted = Array< int >{ 0, 1, 2, 3, 4, 5 };
    auto reversed = Array< int >{ 5, 4, 3, 2, 1, 0 };

    a.sort( []( int a, int b ) -> bool {
        return a < b;
    });

    EXPECT_EQ( sorted, a );

    a.sort( []( int a, int b ) -> bool {
        return a > b;
    });

    EXPECT_EQ( reversed, a );
}

TEST( ArrayTest, sortAdvanced )
{
    using Element = std::pair< double, int >;

    auto es = Array< Element > {
        { 0.2, 0 },
        { 0.1, 1 },
        { 0.9, 2 },
        { 0.0, 3 },
    };

    auto sorted = Array< Element > {
        { 0.0, 3 },
        { 0.1, 1 },
        { 0.2, 0 },
        { 0.9, 2 }
    };

    es.sort( []( const Element &a, const Element &b ) -> bool {
        return a.first < b.first;
    });

    EXPECT_TRUE( sorted == es );
}

TEST( ArrayTest, mapSimple )
{
    using Element = crimild::UInt32;

    auto es = Array< Element > { 0, 1, 2, 3, 4 };
    auto doubles = Array< Element > { 0, 2, 4, 6, 8 };
    auto mapped = es.map( []( auto e ) { return 2 * e; } );

    EXPECT_TRUE( doubles == mapped );
}

TEST( ArrayTest, mapString )
{
    using Element = crimild::UInt32;

    auto es = Array< Element > { 0, 1, 2, 3, 4 };
    auto expected = Array< std::string > { "0", "1", "2", "3", "4" };
    auto ret = es.map(
        []( auto e ) {
        	std::stringstream ss;
            ss << e;
            return ss.str();
    	}
    );

    EXPECT_EQ( expected, ret );
}

TEST( ArrayTest, fill )
{
    auto es = Array< Int32 >( 10 ).fill( []( auto i ) { return i; } );
    auto expected = Array< Int32 > { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    ASSERT_EQ( expected, es );
}

TEST( ArrayTest, reversed )
{
    auto es = Array< Int32 >( 10 ).fill( []( auto i ) { return i; } ).reversed();
    auto expected = Array< Int32 > { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    ASSERT_EQ( expected, es );
}
