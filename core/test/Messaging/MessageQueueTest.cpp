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

#include "Messaging/MessageQueue.hpp"
#include "Utils/MockMessageHandler.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( MessageQueueTest, broadcastMessage )
{
	MockMessenger m;

	EXPECT_EQ( 0, m.getCallCount() );

	m.broadcastMessage( MockMessage {} );
	EXPECT_EQ( 1, m.getCallCount() );

	m.broadcastMessage( MockMessage {} );
	EXPECT_EQ( 2, m.getCallCount() );
}

TEST( MessageQueueTest, unregisterMessageHandler )
{
	MockMessenger m;

	EXPECT_EQ( 0, m.getCallCount() );

	m.broadcastMessage( MockMessage {} );
	EXPECT_EQ( 1, m.getCallCount() );

	m.unregisterMessageHandler< MockMessage >();

	m.broadcastMessage( MockMessage {} );
	EXPECT_EQ( 1, m.getCallCount() );
}

TEST( MessageQueueTest, overrideMessageHandler )
{
	MockMessenger m;

	m.broadcastMessage( MockMessage {} );
	EXPECT_EQ( 1, m.getCallCount() );

	m.registerMessageHandler< MockMessage >( [&]( MockMessage const & ) {
		m.incCallCount();
		m.incCallCount();
		m.incCallCount();
	});

	m.broadcastMessage( MockMessage {} );
	EXPECT_EQ( 4, m.getCallCount() );
}

TEST( MessageQueueTest, destruction )
{
	int count = 0;
	Messenger m1;
	m1.registerMessageHandler< MockMessage >( [&]( MockMessage const & ) {
		count++;
	});

	m1.broadcastMessage( MockMessage {} );
	EXPECT_EQ( 1, count );

	{
		Messenger m2;
		m2.registerMessageHandler< MockMessage >( [&]( MockMessage const & ) {
			count++;
		});

		m1.broadcastMessage( MockMessage {} );
		EXPECT_EQ( 3, count );
	}

	m1.broadcastMessage( MockMessage {} );
	EXPECT_EQ( 4, count );
}

TEST( MessageQueueTest, pushMessage )
{
	MockMessenger m;

	EXPECT_EQ( 0, m.getCallCount() );

	MessageQueue::getInstance()->pushMessage( MockMessage { } );
	EXPECT_EQ( 0, m.getCallCount() );

	MessageQueue::getInstance()->pushMessage( MockMessage { } );
	EXPECT_EQ( 0, m.getCallCount() );

	MessageQueue::getInstance()->dispatchDeferredMessages();
	EXPECT_EQ( 2, m.getCallCount() );
}

