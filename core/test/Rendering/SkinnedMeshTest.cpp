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

#include "Rendering/SkinnedMesh.hpp"
#include "Streaming/FileStream.hpp"
 
#include "gtest/gtest.h"

using namespace crimild;

TEST( SkinnedMesh, streamSkinnedMeshJoint )
{
	Transformation offset;
	offset.setTranslate( 1.0f, 2.0f, 3.0f );
	auto joint = crimild::alloc< SkinnedMeshJoint >( 5, offset, "aJoint" );

	{
		FileStream os( "skinnedMesh.crimild", FileStream::OpenMode::WRITE );
		os.addObject( joint );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "skinnedMesh.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		
		auto obj1 = is.getObjectAt< SkinnedMeshJoint >( 0 );
		EXPECT_TRUE( joint != nullptr );
		
		EXPECT_EQ( 5, joint->getId() );
		EXPECT_EQ( Vector3f( 1.0f, 2.0f, 3.0f ), joint->getOffset().getTranslate() );
		EXPECT_EQ( "aJoint", joint->getName() );
	}
}

TEST( SkinnedMesh, streamSkinnedMeshJointCatalog )
{
	{
		Transformation offset;
		auto catalog = crimild::alloc< SkinnedMeshJointCatalog >();
		EXPECT_EQ( 0, catalog->updateOrCreateJoint( "joint2", offset )->getId() );
		EXPECT_EQ( 1, catalog->updateOrCreateJoint( "joint5", offset )->getId() );
		EXPECT_EQ( 2, catalog->updateOrCreateJoint( "joint3", offset )->getId() );
		EXPECT_EQ( 3, catalog->updateOrCreateJoint( "joint4", offset )->getId() );
		EXPECT_EQ( 4, catalog->updateOrCreateJoint( "joint6", offset )->getId() );
		EXPECT_EQ( 5, catalog->updateOrCreateJoint( "joint7", offset )->getId() );
		EXPECT_EQ( 6, catalog->updateOrCreateJoint( "joint1", offset )->getId() );

		FileStream os( "skinnedMesh.crimild", FileStream::OpenMode::WRITE );
		os.addObject( catalog );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "skinnedMesh.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		
		auto catalog = is.getObjectAt< SkinnedMeshJointCatalog >( 0 );
		EXPECT_TRUE( catalog != nullptr );
		
		EXPECT_EQ( 7, catalog->getJointCount() );
		EXPECT_EQ( 6, catalog->find( "joint1" )->getId() );
		EXPECT_EQ( 0, catalog->find( "joint2" )->getId() );
		EXPECT_EQ( 2, catalog->find( "joint3" )->getId() );
		EXPECT_EQ( 3, catalog->find( "joint4" )->getId() );
		EXPECT_EQ( 1, catalog->find( "joint5" )->getId() );
		EXPECT_EQ( 4, catalog->find( "joint6" )->getId() );
		EXPECT_EQ( 5, catalog->find( "joint7" )->getId() );
	}
}

TEST( SkinnedMesh, streamSkinnedMeshAnimationChannel )
{
	{
		auto channel = crimild::alloc< SkinnedMeshAnimationChannel >();
		channel->setName( "L_Ankle" );

		channel->getPositionKeys().resize( 3 );
		channel->getPositionKeys()[ 0 ].time = 0;
		channel->getPositionKeys()[ 0 ].value = Vector3f( 1.0f, 0.0f, 0.0f );
		channel->getPositionKeys()[ 1 ].time = 5;
		channel->getPositionKeys()[ 1 ].value = Vector3f( 1.0f, 1.0f, 0.0f );
		channel->getPositionKeys()[ 2 ].time = 10;
		channel->getPositionKeys()[ 2 ].value = Vector3f( 1.0f, 0.0f, 1.0f );

		channel->getRotationKeys().resize( 3 );
		channel->getRotationKeys()[ 0 ].time = 0;
		channel->getRotationKeys()[ 0 ].value = Quaternion4f( 1.0f, 2.0f, 3.0f, 4.0f );
		channel->getRotationKeys()[ 1 ].time = 5;
		channel->getRotationKeys()[ 1 ].value = Quaternion4f( 1.0f, 4.0f, 3.0f, 6.0f );
		channel->getRotationKeys()[ 2 ].time = 10;
		channel->getRotationKeys()[ 2 ].value = Quaternion4f( 1.0f, 2.0f, 3.0f, 4.0f );

		channel->getScaleKeys().resize( 3 );
		channel->getScaleKeys()[ 0 ].time = 0;
		channel->getScaleKeys()[ 0 ].value = 1;
		channel->getScaleKeys()[ 1 ].time = 5;
		channel->getScaleKeys()[ 1 ].value = 2;
		channel->getScaleKeys()[ 2 ].time = 10;
		channel->getScaleKeys()[ 2 ].value = 3;

		FileStream os( "skinnedMesh.crimild", FileStream::OpenMode::WRITE );
		os.addObject( channel );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "skinnedMesh.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		
		auto channel = is.getObjectAt< SkinnedMeshAnimationChannel >( 0 );
		EXPECT_TRUE( channel != nullptr );

		EXPECT_EQ( "L_Ankle", channel->getName() );

		EXPECT_EQ( 3, channel->getPositionKeys().size() );
		EXPECT_EQ( 0, channel->getPositionKeys()[ 0 ].time );
		EXPECT_EQ( Vector3f( 1.0f, 0.0f, 0.0f ), channel->getPositionKeys()[ 0 ].value );
		EXPECT_EQ( 5, channel->getPositionKeys()[ 1 ].time );
		EXPECT_EQ( Vector3f( 1.0f, 1.0f, 0.0f ), channel->getPositionKeys()[ 1 ].value );
		EXPECT_EQ( 10, channel->getPositionKeys()[ 2 ].time );
		EXPECT_EQ( Vector3f( 1.0f, 0.0f, 1.0f ), channel->getPositionKeys()[ 2 ].value );

		EXPECT_EQ( 3, channel->getRotationKeys().size() );
		EXPECT_EQ( 0, channel->getRotationKeys()[ 0 ].time );
		EXPECT_EQ( Quaternion4f( 1.0f, 2.0f, 3.0f, 4.0f ), channel->getRotationKeys()[ 0 ].value );
		EXPECT_EQ( 5, channel->getRotationKeys()[ 1 ].time );
		EXPECT_EQ( Quaternion4f( 1.0f, 4.0f, 3.0f, 6.0f ), channel->getRotationKeys()[ 1 ].value );
		EXPECT_EQ( 10, channel->getRotationKeys()[ 2 ].time );
		EXPECT_EQ( Quaternion4f( 1.0f, 2.0f, 3.0f, 4.0f ), channel->getRotationKeys()[ 2 ].value );

		EXPECT_EQ( 3, channel->getScaleKeys().size() );
		EXPECT_EQ( 0, channel->getScaleKeys()[ 0 ].time );
		EXPECT_EQ( 1, channel->getScaleKeys()[ 0 ].value );
		EXPECT_EQ( 5, channel->getScaleKeys()[ 1 ].time );
		EXPECT_EQ( 2, channel->getScaleKeys()[ 1 ].value );
		EXPECT_EQ( 10, channel->getScaleKeys()[ 2 ].time );
		EXPECT_EQ( 3, channel->getScaleKeys()[ 2 ].value );
	}
}

TEST( SkinnedMesh, streamSkinnedMeshAnimationClip )
{
	{
		auto clip = crimild::alloc< SkinnedMeshAnimationClip >();
		clip->setDuration( 50.0f );
		clip->setFrameRate( 24.0f );

		auto ankleChannel = crimild::alloc< SkinnedMeshAnimationChannel >();
		ankleChannel->setName( "L_Ankle" );
		clip->getChannels().add( ankleChannel->getName(), ankleChannel );

		auto armChannel = crimild::alloc< SkinnedMeshAnimationChannel >();
		armChannel->setName( "R_Arm" );
		clip->getChannels().add( armChannel->getName(), armChannel );

		auto bodyChannel = crimild::alloc< SkinnedMeshAnimationChannel >();
		bodyChannel->setName( "Body" );
		clip->getChannels().add( bodyChannel->getName(), bodyChannel );

		FileStream os( "skinnedMesh.crimild", FileStream::OpenMode::WRITE );
		os.addObject( clip );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "skinnedMesh.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		
		auto clip = is.getObjectAt< SkinnedMeshAnimationClip >( 0 );
		EXPECT_TRUE( clip != nullptr );

		EXPECT_EQ( 50.0f, clip->getDuration() );
		EXPECT_EQ( 24.0f, clip->getFrameRate() );

		EXPECT_EQ( 3, clip->getChannels().size() );
		EXPECT_EQ( "L_Ankle", clip->getChannels()[ "L_Ankle" ]->getName() );
		EXPECT_EQ( "R_Arm", clip->getChannels()[ "R_Arm" ]->getName() );
		EXPECT_EQ( "Body", clip->getChannels()[ "Body" ]->getName() );
	}
}

TEST( SkinnedMesh, streamSkinnedMeshSkeleton )
{
	{
		auto skeleton = crimild::alloc< SkinnedMeshSkeleton >();

		skeleton->getClips().add( crimild::alloc< SkinnedMeshAnimationClip >() );
		skeleton->getClips().add( crimild::alloc< SkinnedMeshAnimationClip >() );
		skeleton->getClips().add( crimild::alloc< SkinnedMeshAnimationClip >() );

		Transformation offset;
		skeleton->getJoints()->updateOrCreateJoint( "joint2", offset );
		skeleton->getJoints()->updateOrCreateJoint( "joint3", offset );

		Transformation t;
		t.setTranslate( 0.0f, 1.0f, 3.0f );
		skeleton->setGlobalInverseTransform( t );

		FileStream os( "skinnedMesh.crimild", FileStream::OpenMode::WRITE );
		os.addObject( skeleton );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "skinnedMesh.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		
		auto skeleton = is.getObjectAt< SkinnedMeshSkeleton >( 0 );
		EXPECT_TRUE( skeleton != nullptr );

		EXPECT_EQ( 3, skeleton->getClips().size() );

		EXPECT_NE( nullptr, skeleton->getJoints() );
		EXPECT_EQ( 2, skeleton->getJoints()->getJointCount() );
		EXPECT_EQ( 0, skeleton->getJoints()->find( "joint2" )->getId() );
		EXPECT_EQ( 1, skeleton->getJoints()->find( "joint3" )->getId() );
	}
}

TEST( SkinnedMesh, streamSkinnedMeshAnimationState )
{
	{
		auto state = crimild::alloc< SkinnedMeshAnimationState >();
		state->getJointPoses().add( Matrix4f() );
		state->getJointPoses().add( Matrix4f() );
		state->getJointPoses().add( Matrix4f() );

		FileStream os( "skinnedMesh.crimild", FileStream::OpenMode::WRITE );
		os.addObject( state );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "skinnedMesh.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		
		auto state = is.getObjectAt< SkinnedMeshAnimationState >( 0 );
		EXPECT_TRUE( state != nullptr );

		EXPECT_EQ( 3, state->getJointPoses().size() );
	}
}

TEST( SkinnedMesh, streamSkinnedMesh )
{
	{
		auto skinnedMesh = crimild::alloc< SkinnedMesh >();
		skinnedMesh->setSkeleton( crimild::alloc< SkinnedMeshSkeleton >() );

		auto state = skinnedMesh->getAnimationState();
		state->getJointPoses().add( Matrix4f() );
		state->getJointPoses().add( Matrix4f() );
		state->getJointPoses().add( Matrix4f() );

		FileStream os( "skinnedMesh.crimild", FileStream::OpenMode::WRITE );
		os.addObject( skinnedMesh );
		EXPECT_TRUE( os.flush() );
	}

	{
		FileStream is( "skinnedMesh.crimild", FileStream::OpenMode::READ );
		EXPECT_TRUE( is.load() );
		EXPECT_EQ( 1, is.getObjectCount() );
		
		auto skinnedMesh = is.getObjectAt< SkinnedMesh >( 0 );
		EXPECT_TRUE( skinnedMesh != nullptr );

		EXPECT_NE( nullptr, skinnedMesh->getSkeleton() );
		EXPECT_EQ( 3, skinnedMesh->getAnimationState()->getJointPoses().size() );
	}
}

