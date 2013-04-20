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

TEST( GeometryNodeTest, construction )
{
	GeometryNodePtr geometry( new GeometryNode( "a geometry" ) );

	EXPECT_EQ( geometry->getName(), "a geometry" );
}

TEST( GeometryNodeTest, attachPrimitive )
{
	GeometryNodePtr geometry( new GeometryNode( "a geometry" ) );
	PrimitivePtr primitive( new Primitive() );

	EXPECT_FALSE( geometry->hasPrimitives() );	

	geometry->attachPrimitive( primitive );

	EXPECT_TRUE( geometry->hasPrimitives() );	
	
	bool found = false;
	int count = 0;
	geometry->foreachPrimitive( [&]( PrimitivePtr p ) mutable {
		++count;
		if ( p == primitive ) {
			found = true;
		}
	});

	EXPECT_EQ( count, 1 );
	EXPECT_TRUE( found );
}

TEST( GeometryNodeTest, detachPrimitive )
{
	GeometryNodePtr geometry( new GeometryNode( "a geometry" ) );
	PrimitivePtr primitive( new Primitive() );

	EXPECT_FALSE( geometry->hasPrimitives() );	

	geometry->attachPrimitive( primitive );

	EXPECT_TRUE( geometry->hasPrimitives() );	

	geometry->detachPrimitive( primitive );

	EXPECT_FALSE( geometry->hasPrimitives() );	
	
	int count = 0;
	geometry->foreachPrimitive( [&]( PrimitivePtr p ) mutable {
		++count;
	});

	EXPECT_EQ( count, 0 );
}

TEST( GeometryNodeTest, detachAllPrimitives )
{
	GeometryNodePtr geometry( new GeometryNode( "a geometry" ) );
	PrimitivePtr primitive1( new Primitive() );
	PrimitivePtr primitive2( new Primitive() );

	geometry->attachPrimitive( primitive1 );
	geometry->attachPrimitive( primitive2 );

	int count = 0;
	geometry->foreachPrimitive( [&]( PrimitivePtr p ) mutable {
		++count;
	});
	EXPECT_EQ( count, 2 );

	geometry->detachAllPrimitives();

	count = 0;
	geometry->foreachPrimitive( [&]( PrimitivePtr p ) mutable {
		++count;
	});
	EXPECT_EQ( count, 0 );

}

