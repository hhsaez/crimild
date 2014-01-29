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

#include "Simulation/RunLoop.hpp"

#include "Utils/MockTask.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( RunLoopTest, construction )
{
	Pointer< RunLoop > loop( new RunLoop() );
}

TEST( RunLoopTest, destruction )
{
	Pointer< MockTask > task0( new MockTask( 0 ) );
	EXPECT_CALL( *task0, stop() )
		.Times( ::testing::Exactly( 1 ) );
	Pointer< MockTask > task1( new MockTask( 1000 ) );
	EXPECT_CALL( *task1, stop() )
		.Times( ::testing::Exactly( 1 ) );
	Pointer< MockTask > task2( new MockTask( 2000 ) );
	EXPECT_CALL( *task2, stop() )
		.Times( ::testing::Exactly( 1 ) );
	Pointer< MockTask > task3( new MockTask( -5 ) );
	EXPECT_CALL( *task3, stop() )
		.Times( ::testing::Exactly( 1 ) );

	{
		Pointer< RunLoop > loop( new RunLoop() );

		loop->startTask( task0 );
		loop->startTask( task1 );
		loop->startTask( task2 );
		loop->startTask( task3 );
	}
}

TEST( RunLoopTest, startTasks )
{
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task0( new MockTask( 0 ) );
	Pointer< MockTask > task1( new MockTask( 1000 ) );
	Pointer< MockTask > task2( new MockTask( 2000 ) );
	Pointer< MockTask > task3( new MockTask( -5 ) );

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
	loop->foreachActiveTask( [&]( Task *task ) {
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
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
	EXPECT_CALL( *task, start() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->startTask( task );

	int i = 0;
	loop->foreachActiveTask( [&]( Task *task ) {
		i++;
	});
	EXPECT_EQ( i, 1 );
}

TEST( RunLoopTest, startSuspendedTask )
{
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
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
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
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
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
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
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
	EXPECT_CALL( *task, stop() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->stopTask( task );
	loop->stopTask( task );

	EXPECT_TRUE( loop->isTaskKilled( task ) );
}

TEST( RunLoopTest, stopSuspendedTask )
{
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
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
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
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
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
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
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
	EXPECT_CALL( *task, suspend() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->suspendTask( task );
	loop->suspendTask( task );

	EXPECT_TRUE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, resumeTasks )
{
	Pointer< RunLoop > loop( new RunLoop() );
	Pointer< MockTask > task0( new MockTask( 0 ) );
	Pointer< MockTask > task1( new MockTask( 1000 ) );

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
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
	EXPECT_CALL( *task, resume() )
		.Times( 0 );

	loop->startTask( task );
	loop->resumeTask( task );

	EXPECT_TRUE( loop->isTaskActive( task ) );
}

TEST( RunLoopTest, resumeAlreadyKilledTask )
{
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
	EXPECT_CALL( *task, resume() )
		.Times( 0 );

	loop->startTask( task );
	loop->stopTask( task );
	loop->resumeTask( task );

	EXPECT_TRUE( loop->isTaskKilled( task ) );
}

TEST( RunLoopTest, updateTasks )
{
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task0( new MockTask( 0 ) );
	EXPECT_CALL( *task0, update() )
		.Times( ::testing::Exactly( 1 ) );

	Pointer< MockTask > task1( new MockTask( 1000 ) );
	EXPECT_CALL( *task1, update() )
		.Times( ::testing::Exactly( 1 ) );

	Pointer< MockTask > task2( new MockTask( 2000 ) );
	EXPECT_CALL( *task2, update() )
		.Times( ::testing::Exactly( 1 ) );

	Pointer< MockTask > task3( new MockTask( -5 ) );
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
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
	loop->startTask( task );

	EXPECT_TRUE( loop->update() );
	EXPECT_TRUE( loop->update() );

	loop->stopTask( task );

	EXPECT_FALSE( loop->update() );
}

TEST( RunLoopTest, stop )
{
	Pointer< RunLoop > loop( new RunLoop() );

	Pointer< MockTask > task( new MockTask( 0 ) );
	EXPECT_CALL( *task, stop() )
		.Times( ::testing::Exactly( 1 ) );
	loop->startTask( task );

	EXPECT_TRUE( loop->update() );
	EXPECT_TRUE( loop->update() );

	loop->stop();

	EXPECT_FALSE( loop->update() );
}

