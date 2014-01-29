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

#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Group.hpp"
#include "Visitors/FetchCameras.hpp"
#include "Visitors/SelectNodes.hpp"
#include "Visitors/UpdateWorldState.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( CameraTest, construction )
{
	Pointer< Camera > camera( new Camera() );
}

TEST( CameraTest, viewMatrix )
{
	Pointer< Camera > camera( new Camera() );
	camera->local().setTranslate( 0.0f, 1.0f, 5.0f );
	camera->perform( UpdateWorldState() );

	Matrix4f view = camera->getViewMatrix();

	EXPECT_EQ( 0.0f, view[ 12 ] );
	EXPECT_EQ( -1.0f, view[ 13 ] );
	EXPECT_EQ( -5.0f, view[ 14 ] );
}

TEST( CameraTest, fetchCameras )
{
	Pointer< Group > scene( new Group() );
	Pointer< Camera > camera( new Camera() );
	scene->attachNode( camera );

	FetchCameras fetchCameras;
	scene->perform( fetchCameras );
	int i = 0;
	fetchCameras.foreachCamera( [&]( Camera *c ) mutable {
		EXPECT_EQ( camera.get(), c );
		i++;
	});
	EXPECT_EQ( 1, i );
}

