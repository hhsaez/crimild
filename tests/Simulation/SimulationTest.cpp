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

TEST( SimulationTest, construction )
{
	EXPECT_EQ( Simulation::getCurrent(), nullptr );

	SimulationPtr simulation( new Simulation( "a simulation" ) );

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
		SimulationPtr simulation( new Simulation( "a simulation" ) );		
		EXPECT_EQ( Simulation::getCurrent(), simulation.get() );
		simulation->getMainLoop()->startTask( task );
	}

	EXPECT_EQ( Simulation::getCurrent(), nullptr );
}

TEST( SimulationTest, update )
{
	SimulationPtr simulation( new Simulation() );

	MockTaskPtr task( new MockTask( 0 ) );
	EXPECT_CALL( *task, start() )
		.Times( ::testing::Exactly( 1 ) );
	EXPECT_CALL( *task, update() )
		.Times( ::testing::Exactly( 3 ) );
	EXPECT_CALL( *task, stop() )
		.Times( ::testing::Exactly( 1 ) );
	simulation->getMainLoop()->startTask( task );

	EXPECT_TRUE( simulation->update() );
	EXPECT_TRUE( simulation->update() );
	EXPECT_TRUE( simulation->update() );

	simulation->stop();

	EXPECT_FALSE( simulation->update() );
}

TEST( SimulationTest, run )
{
	int loopCount = 0;

	SimulationPtr simulation( new Simulation() );

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

