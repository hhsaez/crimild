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

#include "Mathematics/Vector.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( VectorTest, construction )
{
	Vector< 3, float > u( 5 );

	EXPECT_EQ( 5, u.x() );
	EXPECT_EQ( 5, u.y() );
	EXPECT_EQ( 5, u.z() );
}

TEST( VectorTest, testBasicOperations )
{
	float data[ 3 ] = { 1.0f, 2.0f, 3.0f };
	Vector< 3, float > u( data );
	EXPECT_EQ( u[ 0 ], 1.0f );
	EXPECT_EQ( u[ 1 ], 2.0f );
	EXPECT_EQ( u[ 2 ], 3.0f );

	Vector< 3, float > v( u );
	EXPECT_EQ( v[ 0 ], 1.0f );
	EXPECT_EQ( v[ 1 ], 2.0f );
	EXPECT_EQ( v[ 2 ], 3.0f );
	EXPECT_TRUE( v == u );
	EXPECT_TRUE( u == v );

	v[ 2 ] = 5.0f;
	EXPECT_EQ( v[ 2 ], 5.0f );
	EXPECT_NE( v, u );
	EXPECT_NE( u, v );

	Vector< 3, float > w;
	w[ 0 ] = 0.0f;
	w[ 1 ] = 0.0f;
	w[ 2 ] = 0.0f;
	EXPECT_NE( w, u );
	EXPECT_NE( w, v );
}

TEST( VectorTest, testSpecialAccessors )
{
	Vector< 2, int > u( 1, 2 );
	Vector< 3, int > v( 2, 4, 6 );
	Vector< 4, int > w( 3, 6, 9, 12 );

	EXPECT_TRUE( u.xy() + v.xy() == ( Vector< 2, int >( 3, 6 ) ) );
	EXPECT_TRUE( u.xyz() + v.xyz() == ( Vector< 3, int >( 3, 6, 6 ) ) ); 
}

TEST( VectorTest, testAddition )
{
	Vector< 3, float > u, v, w;

	u[ 0 ] = 2.0f;
	u[ 1 ] = 3.0f;
	u[ 2 ] = 5.0f;

	v[ 0 ] = 8.0f;
	v[ 1 ] = 10.0f;
	v[ 2 ] = -2.0f;

	w = u + v;

	EXPECT_TRUE( w == ( u + v ) );
	EXPECT_TRUE( w[ 0 ] == 10.0f );
	EXPECT_TRUE( w[ 1 ] == 13.0f );
	EXPECT_TRUE( w[ 2 ] == 3.0f );

	w -= v;

	EXPECT_TRUE( w == u );

	w = u - v;
	EXPECT_TRUE( w[ 0 ] == -6.0f );
	EXPECT_TRUE( w[ 1 ] == -7.0f );
	EXPECT_TRUE( w[ 2 ] == 7.0f );

	w = -w;
	EXPECT_TRUE( w[ 0 ] == 6.0f );
	EXPECT_TRUE( w[ 1 ] == 7.0f );
	EXPECT_TRUE( w[ 2 ] == -7.0f );
}

TEST( VectorTest, testProduct )
{
	Vector< 3, float > u, v, w;
	float a = 2.0f;

	u[ 0 ] = 2.0f;
	u[ 1 ] = 3.0f;
	u[ 2 ] = 5.0f;

	v[ 0 ] = 8.0f;
	v[ 1 ] = 10.0f;
	v[ 2 ] = -2.0f;

	w = u * a; 

	EXPECT_TRUE( w == ( a * u ) );
	EXPECT_TRUE( w == ( u * a ) );
	EXPECT_TRUE( w[ 0 ] == 4.0f );
	EXPECT_TRUE( w[ 1 ] == 6.0f );
	EXPECT_TRUE( w[ 2 ] == 10.0f );

	w = u / a;

	EXPECT_TRUE( w == ( u / a ) );
	EXPECT_TRUE( w[ 0 ] == 2.0f / a );
	EXPECT_TRUE( w[ 1 ] == 3.0f / a );
	EXPECT_TRUE( w[ 2 ] == 5.0f / a );

	float p = u * v;
	EXPECT_TRUE( p == ( u[ 0 ] * v[ 0 ] + u[ 1 ] * v[ 1 ] + u[ 2 ] * v[ 2 ] ) );
	EXPECT_TRUE( p == 36.0f );
}

TEST( VectorTest, testCrossProduct )
{
	Vector< 3, double > i, j, k, w;

	i[ 0 ] = 1.0;
	i[ 1 ] = 0.0;
	i[ 2 ] = 0.0;

	j[ 0 ] = 0.0;
	j[ 1 ] = 1.0;
	j[ 2 ] = 0.0;

	k[ 0 ] = 0.0;
	k[ 1 ] = 0.0;
	k[ 2 ] = 1.0;

	w = i ^ j;

	EXPECT_TRUE( w[ 0 ] == 0.0 );
	EXPECT_TRUE( w[ 1 ] == 0.0 );
	EXPECT_TRUE( w[ 2 ] == 1.0 );
	EXPECT_TRUE( w == k );

	w = j ^ k;
	EXPECT_TRUE( w == i );

	w = k ^ i;
	EXPECT_TRUE( w == j );
}

TEST( VectorTest, testMagnitude )
{
	Vector< 3, float > u;
	u[ 0 ] = 2.0f;
	u[ 1 ] = 3.0f;
	u[ 2 ] = 5.0f;

	float sm = u.getSquaredMagnitude();
	EXPECT_TRUE( sm == ( u[ 0 ] * u[ 0 ] + u[ 1 ] * u[ 1 ] + u[ 2 ] * u[ 2 ] ) );
	EXPECT_TRUE( sm == 38 );

	float m = u.getMagnitude();
	EXPECT_TRUE( m == static_cast< float >( std::sqrt( sm ) ) );
}

TEST( VectorTest, testNormalize )
{
	Vector< 3, float > u, v;
	u[ 0 ] = 2.0f;
	u[ 1 ] = 3.0f;
	u[ 2 ] = 5.0f;

	v = u;
	v.normalize();

	EXPECT_TRUE( v == u / u.getMagnitude() );

	EXPECT_TRUE( Numeric< float >::equals( v.getMagnitude(), 1 ) );
	EXPECT_TRUE( Numeric< float >::equals( v[ 0 ], u[ 0 ] / u.getMagnitude() ) );
	EXPECT_TRUE( Numeric< float >::equals( v[ 1 ], u[ 1 ] / u.getMagnitude() ) );
	EXPECT_TRUE( Numeric< float >::equals( v[ 2 ], u[ 2 ] / u.getMagnitude() ) );
}

TEST( VectorTest, times )
{
	Vector< 3, float > u( 2, 0, 5 );
	Vector< 3, float > v( 3, 4, 10 );
	u.times( v );

	EXPECT_EQ( 6, u.x() );
	EXPECT_EQ( 0, u.y() );
	EXPECT_EQ( 50, u.z() );
}

