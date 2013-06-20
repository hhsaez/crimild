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

TEST( MessageQueueTest, pushMessage )
{
	MockMessagePtr message( new MockMessage() );
	message->value = 0;

	MockMessageHandler handler1;

	MessageQueue::getInstance().pushMessage( message );
	EXPECT_EQ( 1, message->value );

	MockMessageHandler handler2;

	MessageQueue::getInstance().pushMessage( message );
	EXPECT_EQ( 3, message->value );
}

TEST( MessageQueueTest, pushDeferredMessage )
{
	MockDeferredMessagePtr message( new MockDeferredMessage() );
	message->value = 0;

	MockDeferredMessageHandler handler1;

	MessageQueue::getInstance().pushMessage( message );
	EXPECT_EQ( 0, message->value );

	MessageQueue::getInstance().dispatchMessages();
	EXPECT_EQ( 1, message->value );

	MockDeferredMessageHandler handler2;

	MessageQueue::getInstance().pushMessage( message );
	EXPECT_EQ( 1, message->value );

	MessageQueue::getInstance().dispatchMessages();
	EXPECT_EQ( 3, message->value );

	// do it again to make sure the message is not sent twice
	MessageQueue::getInstance().dispatchMessages();
	EXPECT_EQ( 3, message->value );
}

TEST( MessageQueueTest, delayMessages )
{
	// this test must always pass. the only way for it to
	// fail is if the message instance is lost, which won't
	// happen since the queue is keeping a reference to it
	{
		MockDeferredMessagePtr message( new MockDeferredMessage() );
		message->value = 0;
		MessageQueue::getInstance().pushMessage( message );
	}

	MockDeferredMessageHandler handler1;
	MockDeferredMessageHandler handler2;

	MessageQueue::getInstance().dispatchMessages();
}

TEST( MessageQueueTest, staticAllocatedMessages ) 
{
	MockMessage message;
	message.value = 0;

	MockMessageHandler handler1;

	MessageQueue::getInstance().pushMessage( message );
	EXPECT_EQ( 1, message.value );

	MockMessageHandler handler2;

	MessageQueue::getInstance().pushMessage( message );
	EXPECT_EQ( 3, message.value );
}

