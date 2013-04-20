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

#include <Crimild.hpp>

#include "gtest/gtest.h"

using namespace Crimild;

TEST( MatrixTest, testTranspose )
{
	Matrix3i m;
	m[ 0 ] = 0;
	m[ 1 ] = 1;
	m[ 2 ] = 2;
	m[ 3 ] = 3;
	m[ 4 ] = 4;
	m[ 5 ] = 5;
	m[ 6 ] = 6;
	m[ 7 ] = 7;
	m[ 8 ] = 8;

	Matrix3i n = m.getTranspose();

	EXPECT_TRUE( n[ 0 ] == m[ 0 ] );
	EXPECT_TRUE( n[ 1 ] == m[ 3 ] );
	EXPECT_TRUE( n[ 2 ] == m[ 6 ] );
	EXPECT_TRUE( n[ 3 ] == m[ 1 ] );
	EXPECT_TRUE( n[ 4 ] == m[ 4 ] );
	EXPECT_TRUE( n[ 5 ] == m[ 7 ] );
	EXPECT_TRUE( n[ 6 ] == m[ 2 ] );
	EXPECT_TRUE( n[ 7 ] == m[ 5 ] );
	EXPECT_TRUE( n[ 8 ] == m[ 8 ] );
}

TEST( MatrixTest, testAddition )
{
	Matrix3i m;
	m[ 0 ] = 0;
	m[ 1 ] = 1;
	m[ 2 ] = 2;
	m[ 3 ] = 3;
	m[ 4 ] = 4;
	m[ 5 ] = 5;
	m[ 6 ] = 6;
	m[ 7 ] = 7;
	m[ 8 ] = 8;

	Matrix3i result = m + m;

	EXPECT_TRUE( result == 2 * m );
	EXPECT_TRUE( ( result - m ) == m );
}

TEST( MatrixTest, testMatrixProduct )
{
	Matrix3i m;
	m[ 0 ] = 0;
	m[ 1 ] = 1;
	m[ 2 ] = 2;
	m[ 3 ] = 3;
	m[ 4 ] = 4;
	m[ 5 ] = 5;
	m[ 6 ] = 6;
	m[ 7 ] = 7;
	m[ 8 ] = 8;

	Matrix3i result = m * m;

	EXPECT_TRUE( result[ 0 ] == 15 );
	EXPECT_TRUE( result[ 1 ] == 18 );
	EXPECT_TRUE( result[ 2 ] == 21 );
	EXPECT_TRUE( result[ 3 ] == 42 );
	EXPECT_TRUE( result[ 4 ] == 54 );
	EXPECT_TRUE( result[ 5 ] == 66 );
	EXPECT_TRUE( result[ 6 ] == 69 );
	EXPECT_TRUE( result[ 7 ] == 90 );
	EXPECT_TRUE( result[ 8 ] == 111 );
}

TEST( MatrixTest, testMakeIdentity )
{
	Matrix3i m, n;
	m.makeIdentity();

	EXPECT_TRUE( ( n * m ) == n );
}

TEST( MatrixTest, testScalarProduct )
{
	Matrix3i m;
	m[ 0 ] = 0;
	m[ 1 ] = 1;
	m[ 2 ] = 2;
	m[ 3 ] = 3;
	m[ 4 ] = 4;
	m[ 5 ] = 5;
	m[ 6 ] = 6;
	m[ 7 ] = 7;
	m[ 8 ] = 8;

	int scalar = 2;

	Matrix3i result1 = m * scalar;

	EXPECT_TRUE( result1[ 0 ] == 0 );
	EXPECT_TRUE( result1[ 1 ] == 2 );
	EXPECT_TRUE( result1[ 2 ] == 4 );
	EXPECT_TRUE( result1[ 3 ] == 6 );
	EXPECT_TRUE( result1[ 4 ] == 8 );
	EXPECT_TRUE( result1[ 5 ] == 10 );
	EXPECT_TRUE( result1[ 6 ] == 12 );
	EXPECT_TRUE( result1[ 7 ] == 14 );
	EXPECT_TRUE( result1[ 8 ] == 16 );

	Matrix3i result2( m );
	result2 *= scalar;

	EXPECT_TRUE( result2 == result1 );
}

TEST( MatrixTest, testProperties )
{
	Matrix3i F, G, H;
	int a, b;

	EXPECT_TRUE( ( F + G ) == ( G + F ) );
	EXPECT_TRUE( ( F + G ) + H == F + ( G + H ) );
	EXPECT_TRUE( a * ( b * F ) == ( a * b ) * F );
	EXPECT_TRUE( a * ( F + G ) == a * F + a * G );
	EXPECT_TRUE( ( a + b ) * F == a * F + b * F );
	EXPECT_TRUE( ( a * F ) * G == a * ( F * G ) );
	EXPECT_TRUE( ( F * G ) * H == F * ( G * H ) );
	EXPECT_TRUE( ( F * G ).getTranspose() == G.getTranspose() * F.getTranspose() );
}

TEST( MatrixTest, testDeterminant )
{
	Matrix3i m;
	m[ 0 ] = 1;
	m[ 1 ] = 1;
	m[ 2 ] = 2;
	m[ 3 ] = 3;
	m[ 4 ] = 4;
	m[ 5 ] = 5;
	m[ 6 ] = 6;
	m[ 7 ] = 7;
	m[ 8 ] = 8;

	int d = m.getDeterminant();

	EXPECT_TRUE( d == -3 );
}

TEST( MatrixTest, testInverse )
{
	Matrix3f M;
	M[ 0 ] = 1;
	M[ 1 ] = 1;
	M[ 2 ] = 2;
	M[ 3 ] = 3;
	M[ 4 ] = 4;
	M[ 5 ] = 5;
	M[ 6 ] = 6;
	M[ 7 ] = 7;
	M[ 8 ] = 8;

	Matrix3f N, I;
	I.makeIdentity();
	N = M.getInverse();

	EXPECT_TRUE( N[ 0 ] == 1 );
	EXPECT_TRUE( N[ 1 ] == -2 );
	EXPECT_TRUE( N[ 2 ] == 1 );
	EXPECT_TRUE( N[ 3 ] == -2 );
	EXPECT_TRUE( N[ 4 ] == 4.0f / 3.0f );
	EXPECT_TRUE( N[ 5 ] == -1.0f / 3.0f );
	EXPECT_TRUE( N[ 6 ] == 1 );
	EXPECT_TRUE( N[ 7 ] == 1.0f / 3.0f );
	EXPECT_TRUE( N[ 8 ] == -1.0f / 3.0f );

	EXPECT_TRUE( M * N == N * M );
	EXPECT_TRUE( M * N == I );
	EXPECT_TRUE( N * M == I );
}

