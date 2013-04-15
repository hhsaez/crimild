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

TEST( QuaternionTest, testBasicOperations )
{
	Quaternion4i q( 1, 2, 3, 4 );

	EXPECT_TRUE( q.getReal() == 1 );
	EXPECT_TRUE( q.getImaginary()[ 0 ] == 2 );
	EXPECT_TRUE( q.getImaginary()[ 1 ] == 3 );
	EXPECT_TRUE( q.getImaginary()[ 2 ] == 4 );
}

TEST( QuaternionTest, testProduct )
{
}

TEST( QuaternionTest, testAddition )
{
	Quaternion4f q( 1, 2, 3, 4 );
	Quaternion4f p( 5, 6, 7, 8 );
	Quaternion4f result = q + p;
	Quaternion4f expected( 6, 8, 10, 12 );

	EXPECT_TRUE( result == expected );
}

TEST( QuaternionTest, testConjugate )
{
	Quaternion4f q( 1, 2, 3, 4 );
	Quaternion4f p( 5, 6, 7, 8 );
	Quaternion4f r = q.getConjugate();

	EXPECT_TRUE( r == Quaternion4f( 1, -2, -3, -4 ) );
	EXPECT_TRUE( r.getConjugate() == q );
	EXPECT_TRUE( ( q + p ).getConjugate() == ( q.getConjugate() + p.getConjugate() ) );

	// TODO: fix this test
	EXPECT_EQ( ( q * p ).getConjugate(), ( p.getConjugate() * q.getConjugate() ) );
}

TEST( QuaternionTest, testNorm )
{
	Quaternion4f q( 1, 2, 3, 4 );
	Quaternion4f p( 5, 6, 7, 8 );

	EXPECT_TRUE( Numericf::equals( q.getSquaredNorm(), 30.0f ) );
	EXPECT_TRUE( Numericf::equals( q.getNorm(), std::sqrt( 30.0f ) ) );
	EXPECT_TRUE( Numericf::equals( q.getNorm(), q.getConjugate().getNorm() ) );

	q.normalize();
	EXPECT_TRUE( Numericf::equals( 1, q.getNorm() ) );
}

TEST( QuaternionTest, testIdentity )
{
	Quaternion4f q( 1, 2, 3, 4 );
	q.makeIdentity();
	EXPECT_TRUE( q == Quaternion4f( 1.0f, 0, 0, 0 ) );
}

TEST( QuaternionTest, testInverse )
{
	Quaternion4f q( 1, 2, 3, 4 );
	Quaternion4f r = q.getInverse();

	EXPECT_TRUE( ( q * r ) == Quaternion4f( 1.0f, 0, 0, 0 ) );
}

TEST( QuaternionTest, testRotationMatrix )
{
	Vector3f axis( 0.0f, 1.0f, 0.0f );
	float angle = Numericf::PI;

	Matrix3f rotMatrix( axis, angle );

	Quaternion4f rot( std::cos( angle / 2.0f ), std::sin( angle / 2.0f ) * axis );
	Matrix3f rotMatrixFromQ;
	rot.getRotationMatrix( rotMatrixFromQ );

	for ( unsigned int i = 0; i < 9; i++ ) {
		EXPECT_TRUE( Numericf::equals( rotMatrix[ i ], rotMatrixFromQ[ i ] ) );
	}
}

TEST( QuaternionTest, testSlerp )
{
	Quaternion4f q0;
	Quaternion4f q1;
	Quaternion4f result = Interpolation::slerp( q0, q1, 0.5 );
}

