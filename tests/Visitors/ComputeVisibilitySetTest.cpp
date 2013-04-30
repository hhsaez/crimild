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

#include "Visitors/ComputeVisibilitySet.hpp"
#include "Rendering/VisibilitySet.hpp"
#include "Rendering/Camera.hpp"
#include "SceneGraph/GeometryNode.hpp"
#include "SceneGraph/GroupNode.hpp"

#include "Utils/MockVisitor.hpp"

#include "gtest/gtest.h"

using namespace Crimild;

TEST( ComputeVisibilitySetTest, traversal )
{
	GroupNodePtr group1( new GroupNode() );
	GroupNodePtr group2( new GroupNode() );
	GeometryNodePtr geometry1( new GeometryNode() );
	GeometryNodePtr geometry2( new GeometryNode() );
	GeometryNodePtr geometry3( new GeometryNode() );

	CameraPtr camera( new Camera );

	group1->attachNode( group2 );
	group1->attachNode( geometry1 );

	group2->attachNode( geometry2 );
	group2->attachNode( geometry3 );

	VisibilitySet result;
	group1->perform( ComputeVisibilitySet( &result, camera.get() ) );

	EXPECT_TRUE( result.hasGeometries() );

	int i = 0;
	result.foreachGeometry( [&]( GeometryNode * geo ) mutable {
		if ( i == 0 ) {
			EXPECT_EQ( geo, geometry2.get() );
		}
		else if ( i == 1 ) {
			EXPECT_EQ( geo, geometry3.get() );
		}
		else if ( i == 2 ) {
			EXPECT_EQ( geo, geometry1.get() );
		}
		i++;
	});
	EXPECT_EQ( 3, i );
}

