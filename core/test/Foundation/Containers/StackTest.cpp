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

#include "Foundation/Containers/Stack.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( StackTest, basicConstruction )
{
	Stack< int > s;

	EXPECT_EQ( 0, s.size() );
	EXPECT_TRUE( s.empty() );
}

TEST( StackTest, pushPop )
{
	Stack< int > s;

	EXPECT_EQ( 0, s.size() );

	s.push( 2 );	
	EXPECT_EQ( 1, s.size() );
	EXPECT_EQ( 2, s.top() );

	s.push( 5 );	
	EXPECT_EQ( 2, s.size() );
	EXPECT_EQ( 5, s.top() );

	s.pop();
	EXPECT_EQ( 1, s.size() );
	EXPECT_EQ( 2, s.top() );
}

TEST( StackTest, contains )
{
	Stack< int > s;
	s.push( 1 );
	s.push( 2 );
	s.push( 3 );

	EXPECT_TRUE( s.contains( 2 ) );
	EXPECT_FALSE( s.contains( 5 ) );
}

TEST( StackTest, remove )
{
	Stack< int > s;
	s.push( 1 );
	s.push( 2 );
	s.push( 3 );

	EXPECT_EQ( 3, s.size() );
	EXPECT_EQ( 3, s.top() );

	s.remove( 2 );

	EXPECT_EQ( 2, s.size() );
	EXPECT_EQ( 3, s.top() );
}

