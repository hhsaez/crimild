/*
 * Copyright (c) 2013-2018, Hernan Saez
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

#include "Foundation/Containers/Digraph.hpp"

#include "gtest/gtest.h"

using namespace crimild;
using namespace crimild::containers;

Digraph< int > createTinyDigraph( void )
{
	Array< int > edges = {
		0, 1,
		0, 5,
		0, 6,
		2, 0,
		2, 3,
		3, 5,
		5, 4,
		6, 4,
		6, 9,
		7, 6,
		8, 7,
		9, 10,
		9, 11,
		9, 12,
		11, 12,
	};

	Digraph< int > d;
	for ( int i = 0; i < edges.size() / 2; i++ ) {
		d.addEdge( edges[ i * 2 ], edges[ i * 2 + 1 ] );
	}

	return d;
}

TEST( DigraphTest, construction )
{
	auto d = createTinyDigraph();

	EXPECT_EQ( 13, d.getVertexCount() );
	EXPECT_EQ( 15, d.getEdgeCount() );
}

TEST( DigraphTest, sort )
{
	auto d = createTinyDigraph();

	auto sorted = d.sort();

	Array< int > expected = {
		8, 2, 7, 3, 0, 5, 6, 1, 9, 4, 11, 10, 12,
	};

	EXPECT_EQ( expected.size(), sorted.size() );
	sorted.each( [ &expected ]( int e, crimild::Size index ) {
		EXPECT_EQ( e, expected[ index ] );	   
	});
}

