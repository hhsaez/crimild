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

#include "Visitors/UpdateRenderState.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Light.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Rendering/Material.hpp"

#include "Utils/MockVisitor.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( UpdateRenderStateTest, lights )
{
	Pointer< Group > scene( new Group() );
	Pointer< Geometry > geometry( new Geometry() );

	Pointer< Light > light( new Light() );

	scene->attachNode( geometry.get() );
	scene->attachNode( light.get() );

	RenderStateComponent *rs = geometry->getComponent< RenderStateComponent >();
	ASSERT_NE( nullptr, rs );

	EXPECT_FALSE( rs->hasMaterials() );
	EXPECT_FALSE( rs->hasLights() );

	scene->perform( UpdateRenderState() );

	EXPECT_TRUE( rs->hasLights() );
	EXPECT_TRUE( rs->hasMaterials() );

	int i = 0;
	rs->foreachLight( [&]( Light *l ) {
		EXPECT_EQ( light.get(), l );
		i++;
	});
	EXPECT_EQ( 1, i );
}

TEST( UpdateRenderStateTest, materials )
{
	Pointer< Group > scene( new Group() );

	Pointer< Geometry > geometry( new Geometry() );
	Pointer< Material > material( new Material() );
	geometry->getComponent< MaterialComponent >()->attachMaterial( material.get() );
	scene->attachNode( geometry.get() );

	RenderStateComponent *rs = geometry->getComponent< RenderStateComponent >();
	ASSERT_NE( nullptr, rs );

	EXPECT_FALSE( rs->hasMaterials() );
	EXPECT_FALSE( rs->hasLights() );

	scene->perform( UpdateRenderState() );

	EXPECT_FALSE( rs->hasLights() );
	EXPECT_TRUE( rs->hasMaterials() );
}

