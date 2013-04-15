/**
 * Crimild Engine is an open source scene graph based engine which purpose
 * is to fulfill the high-performance requirements of typical multi-platform
 * two and tridimensional multimedia projects, like games, simulations and
 * virtual reality.
 *
 * Copyright (C) 2006-2013 Hernan Saez - hhsaez@gmail.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
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

