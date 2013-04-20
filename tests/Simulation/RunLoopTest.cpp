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

#include "Utils/MockTask.hpp"

#include "gtest/gtest.h"

using namespace Crimild;

TEST( RunLoopTest, construction )
{
	RunLoopPtr loop( new RunLoop() );
}

TEST( RunLoopTest, destruction )
{
	MockTaskPtr task0( new MockTask( 0 ) );
	EXPECT_CALL( *task0, stop() )
		.Times( ::testing::Exactly( 1 ) );
	MockTaskPtr task1( new MockTask( 1000 ) );
	EXPECT_CALL( *task1, stop() )
		.Times( ::testing::Exactly( 1 ) );
	MockTaskPtr task2( new MockTask( 2000 ) );
	EXPECT_CALL( *task2, stop() )
		.Times( ::testing::Exactly( 1 ) );
	MockTaskPtr task3( new MockTask( -5 ) );
	EXPECT_CALL( *task3, stop() )
		.Times( ::testing::Exactly( 1 ) );

	{
		RunLoopPtr loop( new RunLoop() );

		loop->startTask( task0 );
		loop->startTask( task1 );
		loop->startTask( task2 );
		loop->startTask( task3 );
	}
}

TEST( RunLoopTest, startTasks )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task0( new MockTask( 0 ) );
	MockTaskPtr task1( new MockTask( 1000 ) );
	MockTaskPtr task2( new MockTask( 2000 ) );
	MockTaskPtr task3( new MockTask( -5 ) );

	EXPECT_FALSE( loop->hasActiveTasks() );

	EXPECT_CALL( *task0, start() )
		.Times( ::testing::Exactly( 1 ) );
	loop->startTask( task0 );

	EXPECT_CALL( *task1, start() )
		.Times( ::testing::Exactly( 1 ) );
	loop->startTask( task1 );

	EXPECT_CALL( *task2, start() )
		.Times( ::testing::Exactly( 1 ) );
	loop->startTask( task2 );

	EXPECT_CALL( *task3, start() )
		.Times( ::testing::Exactly( 1 ) );
	loop->startTask( task3 );

	EXPECT_TRUE( loop->hasActiveTasks() );
	EXPECT_TRUE( loop->isTaskActive( task0 ) );
	EXPECT_TRUE( loop->isTaskActive( task1 ) );
	EXPECT_TRUE( loop->isTaskActive( task2 ) );
	EXPECT_TRUE( loop->isTaskActive( task3 ) );

	int i = 0;
	loop->foreachActiveTask( [&]( TaskPtr &task ) {
		switch (i) {
			case 0:
				EXPECT_EQ( task, task3 );
				break;
			case 1: 
				EXPECT_EQ( task, task0 );
				break;
			case 2: 
				EXPECT_EQ( task, task1 );
				break;
			case 3:
				EXPECT_EQ( task, task2 );
				break;
		}
		i++;
	});
	EXPECT_EQ( i, 4 );
}

TEST( RunLoopTest, startTaskAlreadyStarted )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, start() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->startTask( task );

	int i = 0;
	loop->foreachActiveTask( [&]( TaskPtr &task ) {
		i++;
	});
	EXPECT_EQ( i, 1 );
}

TEST( RunLoopTest, startSuspendedTask )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, start() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->suspendTask( task );
	loop->startTask( task );

	EXPECT_FALSE( loop->isTaskActive( task ) );
	EXPECT_TRUE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, startStoppedTask )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, start() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->stopTask( task );
	loop->startTask( task );

	EXPECT_FALSE( loop->isTaskActive( task ) );
	EXPECT_TRUE( loop->isTaskKilled( task ) );
}

TEST( RunLoopTest, stopTasks )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, stop() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );

	loop->stopTask( task );

	EXPECT_FALSE( loop->hasActiveTasks() );
	EXPECT_TRUE( loop->hasKilledTasks() );
	EXPECT_TRUE( loop->isTaskKilled( task ) );
}

TEST( RunLoopTest, stopKilledTask )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, stop() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->stopTask( task );
	loop->stopTask( task );

	EXPECT_TRUE( loop->isTaskKilled( task ) );
}

TEST( RunLoopTest, stopSuspendedTask )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, stop() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->suspendTask( task );
	loop->stopTask( task );

	EXPECT_TRUE( loop->isTaskKilled( task ) );
	EXPECT_FALSE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, suspendTask )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, suspend() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );

	loop->suspendTask( task );

	EXPECT_FALSE( loop->hasActiveTasks() );
	EXPECT_TRUE( loop->hasSuspendedTasks() );
	EXPECT_TRUE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, suspendKilledTask )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, suspend() )
		.Times( 0 );

	loop->startTask( task );
	loop->stopTask( task );
	loop->suspendTask( task );

	EXPECT_TRUE( loop->isTaskKilled( task ) );
	EXPECT_FALSE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, suspendAlreadySuspendedTask )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, suspend() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->suspendTask( task );
	loop->suspendTask( task );

	EXPECT_TRUE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, resumeTasks )
{
	RunLoopPtr loop( new RunLoop() );
	MockTaskPtr task0( new MockTask( 0 ) );
	MockTaskPtr task1( new MockTask( 1000 ) );

	EXPECT_CALL( *task0, suspend() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *task0, resume() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task0 );
	loop->startTask( task1 );

	loop->suspendTask( task0 );
	
	loop->resumeTask( task0 );

	EXPECT_TRUE( loop->hasActiveTasks() );
	EXPECT_FALSE( loop->hasSuspendedTasks() );
	EXPECT_FALSE( loop->isTaskSuspended( task0 ) );
	EXPECT_TRUE( loop->isTaskActive( task0 ) );
}

TEST( RunLoopTest, resumeAlreadyStartedTask )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, resume() )
		.Times( 0 );

	loop->startTask( task );
	loop->resumeTask( task );

	EXPECT_TRUE( loop->isTaskActive( task ) );
}

TEST( RunLoopTest, resumeAlreadyKilledTask )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, resume() )
		.Times( 0 );

	loop->startTask( task );
	loop->stopTask( task );
	loop->resumeTask( task );

	EXPECT_TRUE( loop->isTaskKilled( task ) );
}

TEST( RunLoopTest, updateTasks )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task0( new MockTask( 0 ) );
	EXPECT_CALL( *task0, update() )
		.Times( ::testing::Exactly( 1 ) );

	MockTaskPtr task1( new MockTask( 1000 ) );
	EXPECT_CALL( *task1, update() )
		.Times( ::testing::Exactly( 1 ) );

	MockTaskPtr task2( new MockTask( 2000 ) );
	EXPECT_CALL( *task2, update() )
		.Times( ::testing::Exactly( 1 ) );

	MockTaskPtr task3( new MockTask( -5 ) );
	EXPECT_CALL( *task3, update() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task0 );
	loop->startTask( task1 );
	loop->startTask( task2 );
	loop->startTask( task3 );

	EXPECT_TRUE( loop->update() );
}

TEST( RunLoopTest, breakLoop )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	loop->startTask( task );

	EXPECT_TRUE( loop->update() );
	EXPECT_TRUE( loop->update() );

	loop->stopTask( task );

	EXPECT_FALSE( loop->update() );
}

TEST( RunLoopTest, stop )
{
	RunLoopPtr loop( new RunLoop() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, stop() )
		.Times( ::testing::Exactly( 1 ) );
	loop->startTask( task );

	EXPECT_TRUE( loop->update() );
	EXPECT_TRUE( loop->update() );

	loop->stop();

	EXPECT_FALSE( loop->update() );
}

