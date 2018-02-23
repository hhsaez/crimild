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

#include "Mathematics/Clock.hpp"

#include "gtest/gtest.h"

#include <thread>

using namespace crimild;

TEST( Clock, construction )
{
	Clock c;

	EXPECT_EQ( 0, c.getDeltaTime() );
	EXPECT_EQ( 0, c.getAccumTime() );
}

TEST( Clock, constructionWithDelta )
{
	Clock c( 0.016 );

	EXPECT_EQ( 0.016, c.getDeltaTime() );
	EXPECT_EQ( 0, c.getAccumTime() );
}

TEST( Clock, tick )
{
	Clock c;

	EXPECT_EQ( 0, c.getDeltaTime() );
	EXPECT_EQ( 0, c.getAccumTime() );

	std::this_thread::sleep_for( std::chrono::milliseconds( 16 ) );
	c.tick();

	EXPECT_GE( 0.020, c.getDeltaTime() );
	EXPECT_GE( 0.020, c.getAccumTime() );

	std::this_thread::sleep_for( std::chrono::milliseconds( 16 ) );
	c.tick();

	EXPECT_GE( 0.020, c.getDeltaTime() );
	EXPECT_GE( 0.040, c.getAccumTime() );

	std::this_thread::sleep_for( std::chrono::milliseconds( 16 ) );
	c.tick();

	EXPECT_GE( 0.025, c.getDeltaTime() );
	EXPECT_GE( 0.060, c.getAccumTime() );
}

TEST( Clock, addDelta )
{
	Clock c0;

	EXPECT_EQ( 0, c0.getDeltaTime() );
	EXPECT_EQ( 0, c0.getAccumTime() );

	c0 += 2.0;

	EXPECT_EQ( 2, c0.getDeltaTime() );
	EXPECT_EQ( 2, c0.getAccumTime() );
}

TEST( Clock, addClocks )
{
	Clock c0, c1( 1.0 );

	EXPECT_EQ( 0, c0.getDeltaTime() );
	EXPECT_EQ( 0, c0.getAccumTime() );

	EXPECT_EQ( 1, c1.getDeltaTime() );
	EXPECT_EQ( 0, c1.getAccumTime() );

	c0 += c1;

	EXPECT_EQ( 1, c1.getDeltaTime() );
	EXPECT_EQ( 1, c0.getAccumTime() );
}

TEST( Clock, timeoutWithoutRepeat )
{
	const Clock TIMER( 0.008 );
	
	bool invoked = false;
	Clock c;
	c.setTimeout( [&]( void ) {
		invoked = true;
	}, 0.016, false );

	c.tick();
	EXPECT_FALSE( invoked );

	c += TIMER;
	EXPECT_FALSE( invoked );

	c += TIMER;
	EXPECT_TRUE( invoked );

	invoked = false;
	c += TIMER;
	EXPECT_FALSE( invoked );
}

TEST( Clock, timeoutWithRepeat )
{
	const Clock TIMER( 0.008 );
	
	bool invoked = false;
	Clock c;
	c.setTimeout( [&]( void ) {
		invoked = true;
	}, 0.016, true );

	c.tick();
	EXPECT_FALSE( invoked );

	c += TIMER;
	EXPECT_FALSE( invoked );

	c += TIMER;
	EXPECT_TRUE( invoked );

	invoked = false;
	c += TIMER;
	EXPECT_TRUE( invoked );
}

