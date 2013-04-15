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

