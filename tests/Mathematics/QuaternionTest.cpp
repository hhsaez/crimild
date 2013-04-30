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

#include "Mathematics/Quaternion.hpp"
#include "Mathematics/Interpolation.hpp"

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

