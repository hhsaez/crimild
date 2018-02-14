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

#include "Foundation/Containers/Array.hpp"

#include "gtest/gtest.h"

using namespace crimild;
using namespace crimild::containers;

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
	a.each( [&sum]( int &e, crimild::Size ) {
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

