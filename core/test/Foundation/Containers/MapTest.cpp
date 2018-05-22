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

#include "Foundation/Containers/Map.hpp"

#include "gtest/gtest.h"

using namespace crimild;
using namespace crimild::containers;

TEST( MapTest, basicConstruction )
{
	Map< int, int > m;

	EXPECT_EQ( 0, m.size() );
	EXPECT_TRUE( m.empty() );
}

TEST( MapTest, initializationList )
{
	Map< int, int > m = { { 1, 2 }, { 2, 4 }, { 3, 6 } };

	int keys[] = { 1, 2, 3 };
	crimild::Int32 count = 0;
	for ( auto k : keys ) {
		EXPECT_EQ( 2 * k, m[ k ] );
		count++;
	};

	EXPECT_EQ( count, m.size() );
}

TEST( MapTest, keys )
{
	Map< int, int > m = { { 1, 2 }, { 2, 4 }, { 3, 6 } };

	auto keys = m.keys();
	EXPECT_TRUE( keys.contains( 1 ) );
	EXPECT_TRUE( keys.contains( 2 ) );
	EXPECT_TRUE( keys.contains( 3 ) );
	EXPECT_EQ( 3, keys.size() );
}

