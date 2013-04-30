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

#include "Mathematics/Intersection.hpp"

#include "gtest/gtest.h"

using namespace Crimild;

TEST( IntersectionTest, testRaySphere )
{
	Sphere3f s0( Vector3f( 0.0f, 0.0f, 0.0f ), 2.0f );
	Ray3f r0( Vector3f( 0.0f, 0.0f, -5.0f ), Vector3f( 0.0f, 0.0f, -1.0f ) );
	Ray3f r1( Vector3f( 0.0f, 0.0f, -5.0f ), Vector3f( 0.0f, 0.0f, 1.0f ) );

	EXPECT_TRUE( Intersection::test( s0, r0 ) == false );
	EXPECT_TRUE( Intersection::test( s0, r1 ) == true );

	Sphere3f sphere1( Vector3f( 0.0f, 0.0f, -10.0f ), 2.0f );
	Sphere3f sphere2( Vector3f( 0.0f, 0.0f, 10.0f ), 2.0f );
	Ray3f ray( Vector3f( 0.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 0.0f, -1.0 ) );

	EXPECT_TRUE( Intersection::test( sphere1, ray ) == true );
	EXPECT_TRUE( Intersection::test( sphere2, ray ) == false );
}

TEST( IntersectionTest, testRayPlane )
{
	Vector< 3, double > rayOrigin;
	rayOrigin[ 0 ] = 0.0;
	rayOrigin[ 1 ] = 0.0;
	rayOrigin[ 2 ] = 5.0;

	Vector< 3, double > rayDirection;
	rayDirection[ 0 ] = 0.0;
	rayDirection[ 1 ] = 0.0;
	rayDirection[ 2 ] = -1.0;

	Ray< 3, double > ray( rayOrigin, rayDirection );

	Vector< 3, double > planeNormal;
	planeNormal[ 0 ] = 0.0;
	planeNormal[ 1 ] = 0.0;
	planeNormal[ 2 ] = 1.0;

	Plane< 3, double > plane( planeNormal, 0.0 );

	EXPECT_TRUE( Intersection::test( plane, ray ) == true );
	//EXPECT_TRUE( Numeric< double >::equal( Intersection::find( plane, ray ), 5.0 ) );

	rayDirection[ 0 ] = 0.0;
	rayDirection[ 1 ] = 1.0;
	rayDirection[ 2 ] = 0.0;

	ray.setDirection( rayDirection );

	EXPECT_TRUE( Intersection::test( plane, ray ) == false );
	EXPECT_TRUE( Intersection::find( plane, ray ) < 0 );
}

TEST( IntersectionTest, testRayTriangle )
{
	// TODO
	Vector< 3, double > p0, p1, p2;
	Vector< 3, double > rOrigin, rDirection;

	p0[ 0 ] = -1.0;
	p0[ 1 ] = -1.0;
	p0[ 2 ] = 0.0;

	p1[ 0 ] = 1.0;
	p1[ 1 ] = -1.0;
	p1[ 2 ] = 0.0;

	p2[ 0 ] = 0.0;
	p2[ 1 ] = 2.0;
	p2[ 2 ] = 0.0;

	rOrigin[ 0 ] = 0.0;
	rOrigin[ 1 ] = 1.0;
	rOrigin[ 2 ] = 5.0;
	rDirection[ 0 ] = 0.0;
	rDirection[ 1 ] = 0.0;
	rDirection[ 2 ] = -1.0;
	Ray< 3, double > ray( rOrigin, rDirection );

	//EXPECT_TRUE( Intersection::test( ray, p0, p1, p2 ) == true );

	ray.getOrigin()[ 0 ] = 5.0;
	ray.getOrigin()[ 1 ] = 1.0;
	ray.getOrigin()[ 2 ] = 5.0;

	//EXPECT_TRUE( Intersection::test( ray, p0, p1, p2 ) == false );
}

