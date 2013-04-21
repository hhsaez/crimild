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

TEST( GeometryNodeTest, construction )
{
	GeometryNodePtr geometry( new GeometryNode( "a geometry" ) );

	EXPECT_EQ( geometry->getName(), "a geometry" );

	MaterialComponent *materials = geometry->getComponent< MaterialComponent >();
	ASSERT_NE( nullptr, materials );
	EXPECT_FALSE( materials->hasMaterials() );
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

