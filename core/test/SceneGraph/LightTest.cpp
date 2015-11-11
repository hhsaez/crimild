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

#include "SceneGraph/Light.hpp"
#include "SceneGraph/Group.hpp"
#include "Visitors/FetchLights.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( LightTest, construction )
{
	auto light = crimild::alloc< Light >();

	EXPECT_EQ( Light::Type::POINT, light->getType() );
	EXPECT_EQ( Vector3f( 0.0f, 0.0f, 0.0f ), light->getPosition() );
	EXPECT_EQ( Vector3f( 1.0f, 0.0f, 0.0f ), light->getAttenuation() );
	EXPECT_EQ( Vector3f( 0.0f, 0.0f, 0.0f ), light->getDirection() );
	EXPECT_EQ( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ), light->getColor() );
	EXPECT_EQ( 0.0f, light->getOuterCutoff() );
	EXPECT_EQ( 0.0f, light->getInnerCutoff() );
	EXPECT_EQ( 0.0f, light->getExponent() );
}

TEST( LightTest, fetchLights )
{
	auto group = crimild::alloc< Group >();
	auto light1 = crimild::alloc< Light >();
	auto light2 = crimild::alloc< Light >();

	group->attachNode( light1 );
	group->attachNode( light2 );

	FetchLights fetchLights;
	group->perform( fetchLights );
	
	EXPECT_TRUE( fetchLights.hasLights() );

	int i = 0; 
	fetchLights.forEachLight( [&i, light1, light2]( Light *light ) {
		if ( i == 0 ) {
			EXPECT_EQ( crimild::get_ptr( light1 ), light );
		}
		else if ( i == 1 ) {
			EXPECT_EQ( crimild::get_ptr( light2 ), light );
		}
		i++;
	});
	EXPECT_EQ( 2, i );
}

