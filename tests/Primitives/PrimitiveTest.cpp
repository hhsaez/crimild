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

#include <Crimild.hpp>

#include "gtest/gtest.h"

using namespace Crimild;

TEST( PrimitiveTest, construction )
{
	PrimitivePtr p1( new Primitive() );
	EXPECT_EQ( p1->getType(), Primitive::Types::TRIANGLES );

	PrimitivePtr p2( new Primitive( Primitive::Types::LINES ) );
	EXPECT_EQ( p2->getType(), Primitive::Types::LINES );
}

TEST( PrimitiveTest, destruction )
{

}

TEST( PrimitiveTest, setVertexBuffer )
{
	PrimitivePtr p( new Primitive() );

	EXPECT_EQ( p->getVertexBuffer(), nullptr );

	VertexBufferObjectPtr vbo( new VertexBufferObject( VertexFormat::VF_P3, 0, nullptr ) );
	p->setVertexBuffer( vbo );

	EXPECT_EQ( p->getVertexBuffer(), vbo.get() );
}

TEST( PrimitiveTest, setIndexBuffer )
{
	PrimitivePtr p( new Primitive() );

	EXPECT_EQ( p->getIndexBuffer(), nullptr );

	IndexBufferObjectPtr ibo( new IndexBufferObject(0, nullptr ) );
	p->setIndexBuffer( ibo );

	EXPECT_EQ( p->getIndexBuffer(), ibo.get() );
}

