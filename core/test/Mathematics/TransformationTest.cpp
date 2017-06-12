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

#include "Mathematics/Transformation.hpp"

#include "gtest/gtest.h"

using namespace crimild;

bool orthonormalize( const Vector3f &u, const Vector3f &v, Vector3f &i, Vector3f &j, Vector3f &k )
{
	Vector3f temp = u.getNormalized() ^ v.getNormalized();

	i = u;
	k = temp;
	j = temp ^ u;

	return true;
}

TEST ( Transformation, testTranslate )
{
	Transformation t;
	t.setTranslate( Vector3f( 1.0f, 2.0f, 3.0f ) );

	EXPECT_EQ( Vector3f( 1.0f, 2.0f, 3.0f ), t.getTranslate() );
}

TEST( Transformation, testLookAt )
{
	Transformation t;
	t.setTranslate( Vector3f( 1.0f, 2.0f, 3.0f ) );

	EXPECT_EQ( Vector3f( 1.0f, 2.0f, 3.0f ), t.getTranslate() );

	t.lookAt( Vector3f( 0.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );

	EXPECT_EQ( Vector3f( -1.0f, -2.0f, -3.0f ).getNormalized(), t.computeDirection() );
}

TEST( Transformation, constructFromMatrix )
{
	Matrix4f m( 1.0f, 0.0f, 0.0f, 0.0f, 
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				3.0f, 4.0f, 5.0f, 1.0f );
	
	Transformation t( m );

	EXPECT_EQ( Vector3f( 3.0f, 4.0f, 5.0f ), t.getTranslate() );
	EXPECT_EQ( 1.0f, t.getScale() );
	EXPECT_EQ( Quaternion4f( 0.0f, 0.0f, 0.0f, 1.0f ), t.getRotate() );
}

TEST( Transformation, getInverse )
{
	Transformation t0;
	t0.setTranslate( 1.0f, 2.0f, 3.0f );
	t0.rotate().fromEulerAngles( 0.0f, 90.0f, 0.0f );

	Transformation t1 = t0.getInverse();

    Transformation t2;
    t2.computeFrom( t0, t1 );

    EXPECT_TRUE( Vector3f::ZERO == t2.getTranslate() );
    EXPECT_TRUE( Quaternion4f( 0.0f, 0.0f, 0.0f, 1.0f ) == t2.getRotate() );
    EXPECT_TRUE( Numericf::equals( 1.0f, t2.getScale() ) );
}

