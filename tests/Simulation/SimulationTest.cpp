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

#include "Simulation/Simulation.hpp"
#include "SceneGraph/Node.hpp"
#include "SceneGraph/GroupNode.hpp"
#include "SceneGraph/Camera.hpp"

#include "Utils/MockTask.hpp"

#include "gtest/gtest.h"

using namespace Crimild;

TEST( SimulationTest, construction )
{
	EXPECT_EQ( Simulation::getCurrent(), nullptr );

	SimulationPtr simulation( new Simulation( "a simulation", 0, nullptr ) );

	EXPECT_EQ( simulation->getName(), "a simulation" );
	EXPECT_EQ( Simulation::getCurrent(), simulation.get() );
}

TEST( SimulationTest, destruction )
{
	EXPECT_EQ( Simulation::getCurrent(), nullptr );
	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, stop() )
		.Times( ::testing::Exactly( 1 ) );

	{
		SimulationPtr simulation( new Simulation( "a simulation", 0, nullptr ) );		
		EXPECT_EQ( Simulation::getCurrent(), simulation.get() );
		simulation->getMainLoop()->startTask( task );
	}

	EXPECT_EQ( Simulation::getCurrent(), nullptr );
}

TEST( SimulationTest, step )
{
	SimulationPtr simulation( new Simulation( "a simulation", 0, nullptr ) );		

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, start() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *task, update() )
		.Times( ::testing::Exactly( 3 ) );
	EXPECT_CALL( *task, stop() )
		.Times( ::testing::Exactly( 1 ) );
	simulation->getMainLoop()->startTask( task );

	EXPECT_TRUE( simulation->step() );
	EXPECT_TRUE( simulation->step() );
	EXPECT_TRUE( simulation->step() );

	simulation->stop();

	EXPECT_FALSE( simulation->step() );
}

TEST( SimulationTest, run )
{
	int loopCount = 0;

	SimulationPtr simulation( new Simulation( "a simulation", 0, nullptr ) );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, start() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *task, update() )
		.Times( ::testing::Exactly( 10 ) )
		.WillRepeatedly( ::testing::Invoke( [&]( void ) mutable {
			loopCount++;
			if ( loopCount >= 10 ) {
				Simulation::getCurrent()->stop();				
			}
		}));
	EXPECT_CALL( *task, stop() )
		.Times( ::testing::Exactly( 1 ) );

	simulation->getMainLoop()->startTask( task );
	simulation->run();

	EXPECT_EQ( 10, loopCount );
}

TEST( SimulationTest, attachSceneWithoutCamera )
{
	SimulationPtr simulation( new Simulation( "a simulation", 0, nullptr ) );

	EXPECT_FALSE( simulation->getMainLoop()->hasActiveTasks() );

	NodePtr simpleScene( new Node() );

	simulation->attachScene( simpleScene );

	EXPECT_FALSE( simulation->getMainLoop()->hasActiveTasks() );
}

TEST( SimulationTest, attachSceneWithCamera )
{
	SimulationPtr simulation( new Simulation( "a simulation", 0, nullptr ) );

	EXPECT_FALSE( simulation->getMainLoop()->hasActiveTasks() );

	GroupNodePtr scene( new GroupNode() );
	CameraPtr camera( new Camera() );
	scene->attachNode( camera );

	simulation->attachScene( scene );

	EXPECT_TRUE( simulation->getMainLoop()->hasActiveTasks() );
}

