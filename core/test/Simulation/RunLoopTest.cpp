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
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );
}

TEST( RunLoopTest, destruction )
{
	auto task0 = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task0.get() ), stop() )
		.Times( ::testing::Exactly( 1 ) );

	auto task1 = crimild::alloc< MockTask >( 1000 );
	EXPECT_CALL( *( task1.get() ), stop() )
		.Times( ::testing::Exactly( 1 ) );

	auto task2 = crimild::alloc< MockTask >( 2000 );
	EXPECT_CALL( *( task2.get() ), stop() )
		.Times( ::testing::Exactly( 1 ) );

	auto task3 = crimild::alloc< MockTask >( -5 );
	EXPECT_CALL( *( task3.get() ), stop() )
		.Times( ::testing::Exactly( 1 ) );

	{
		auto loop = crimild::alloc< RunLoop >( "Test Loop" );

		loop->startTask( task0 );
		loop->startTask( task1 );
		loop->startTask( task2 );
		loop->startTask( task3 );
	}
}

TEST( RunLoopTest, startTasks )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task0 = crimild::alloc< MockTask >( 0 );
	auto task1 = crimild::alloc< MockTask >( 100 );
	auto task2 = crimild::alloc< MockTask >( 200 );
	auto task3 = crimild::alloc< MockTask >( -5 );

	EXPECT_FALSE( loop->hasActiveTasks() );

	EXPECT_CALL( *( task0.get() ), start() )
		.Times( ::testing::Exactly( 1 ) );
	loop->startTask( task0 );

	EXPECT_CALL( *( task1.get() ), start() )
		.Times( ::testing::Exactly( 1 ) );
	loop->startTask( task1 );

	EXPECT_CALL( *( task2.get() ), start() )
		.Times( ::testing::Exactly( 1 ) );
	loop->startTask( task2 );

	EXPECT_CALL( *( task3.get() ), start() )
		.Times( ::testing::Exactly( 1 ) );
	loop->startTask( task3 );

	EXPECT_TRUE( loop->hasActiveTasks() );
	EXPECT_TRUE( loop->isTaskActive( task0 ) );
	EXPECT_TRUE( loop->isTaskActive( task1 ) );
	EXPECT_TRUE( loop->isTaskActive( task2 ) );
	EXPECT_TRUE( loop->isTaskActive( task3 ) );

	int i = 0;
	loop->foreachActiveTask( [&]( TaskPtr const &task ) {
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
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), start() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->startTask( task );

	int i = 0;
	loop->foreachActiveTask( [&]( TaskPtr const &task ) {
		i++;
	});
	EXPECT_EQ( i, 1 );
}

TEST( RunLoopTest, startSuspendedTask )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), start() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->suspendTask( task );
	loop->startTask( task );

	EXPECT_FALSE( loop->isTaskActive( task ) );
	EXPECT_TRUE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, startStoppedTask )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), start() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->stopTask( task );
	loop->startTask( task );

	EXPECT_FALSE( loop->isTaskActive( task ) );
	EXPECT_TRUE( loop->isTaskKilled( task ) );
}

TEST( RunLoopTest, stopTasks )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), stop() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );

	loop->stopTask( task );

	EXPECT_FALSE( loop->hasActiveTasks() );
	EXPECT_TRUE( loop->hasKilledTasks() );
	EXPECT_TRUE( loop->isTaskKilled( task ) );
}

TEST( RunLoopTest, stopKilledTask )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), stop() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->stopTask( task );
	loop->stopTask( task );

	EXPECT_TRUE( loop->isTaskKilled( task ) );
}

TEST( RunLoopTest, stopSuspendedTask )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), stop() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->suspendTask( task );
	loop->stopTask( task );

	EXPECT_TRUE( loop->isTaskKilled( task ) );
	EXPECT_FALSE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, suspendTask )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), suspend() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );

	loop->suspendTask( task );

	EXPECT_FALSE( loop->hasActiveTasks() );
	EXPECT_TRUE( loop->hasSuspendedTasks() );
	EXPECT_TRUE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, suspendKilledTask )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), suspend() )
		.Times( 0 );

	loop->startTask( task );
	loop->stopTask( task );
	loop->suspendTask( task );

	EXPECT_TRUE( loop->isTaskKilled( task ) );
	EXPECT_FALSE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, suspendAlreadySuspendedTask )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), suspend() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task );
	loop->suspendTask( task );
	loop->suspendTask( task );

	EXPECT_TRUE( loop->isTaskSuspended( task ) );
}

TEST( RunLoopTest, resumeTasks )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );
	auto task0 = crimild::alloc< MockTask >( 0 );
	auto task1 = crimild::alloc< MockTask >( 1000 );

	EXPECT_CALL( *( task0 ), suspend() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *( task0 ), resume() )
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
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), resume() )
		.Times( 0 );

	loop->startTask( task );
	loop->resumeTask( task );

	EXPECT_TRUE( loop->isTaskActive( task ) );
}

TEST( RunLoopTest, resumeAlreadyKilledTask )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), resume() )
		.Times( 0 );

	loop->startTask( task );
	loop->stopTask( task );
	loop->resumeTask( task );

	EXPECT_TRUE( loop->isTaskKilled( task ) );
}

TEST( RunLoopTest, updateTasks )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task0 = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task0.get() ), update() )
		.Times( ::testing::Exactly( 1 ) );

	auto task1 = crimild::alloc< MockTask >( 1000 );
	EXPECT_CALL( *( task1.get() ), update() )
		.Times( ::testing::Exactly( 1 ) );

	auto task2 = crimild::alloc< MockTask >( 2000 );
	EXPECT_CALL( *( task2.get() ), update() )
		.Times( ::testing::Exactly( 1 ) );

	auto task3 = crimild::alloc< MockTask >( -5 );
	EXPECT_CALL( *( task3.get() ), update() )
		.Times( ::testing::Exactly( 1 ) );

	loop->startTask( task0 );
	loop->startTask( task1 );
	loop->startTask( task2 );
	loop->startTask( task3 );

	EXPECT_TRUE( loop->update() );
}

TEST( RunLoopTest, breakLoop )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	
	loop->startTask( task );

	EXPECT_TRUE( loop->update() );
	EXPECT_TRUE( loop->update() );

	loop->stopTask( task );

	EXPECT_FALSE( loop->update() );
}

TEST( RunLoopTest, stop )
{
	auto loop = crimild::alloc< RunLoop >( "Test Loop" );

	auto task = crimild::alloc< MockTask >( 0 );
	EXPECT_CALL( *( task.get() ), stop() )
		.Times( ::testing::Exactly( 1 ) );
	
	loop->startTask( task );

	EXPECT_TRUE( loop->update() );
	EXPECT_TRUE( loop->update() );

	loop->stop();

	EXPECT_FALSE( loop->update() );
}

