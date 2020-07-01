/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/Vertex.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( VertexP3TC2, layout )
{
    auto layout = VertexP3TC2::getLayout();

	ASSERT_EQ( sizeof( Real32 ) * 5, layout.getSize() );

	ASSERT_EQ( 0, layout.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, layout.getAttributeFormat( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( sizeof( Real32 ) * 3, layout.getAttributeSize( VertexAttribute::Name::POSITION ) );

	ASSERT_EQ( sizeof( Real32 ) * 3, layout.getAttributeOffset( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( Format::R32G32_SFLOAT, layout.getAttributeFormat( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( sizeof( Real32 ) * 2, layout.getAttributeSize( VertexAttribute::Name::TEX_COORD ) );
}

TEST( VertexP3N3TC2, layout )
{
    auto layout = VertexP3N3TC2::getLayout();

	ASSERT_EQ( sizeof( Real32 ) * 8, layout.getSize() );

	ASSERT_EQ( 0, layout.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, layout.getAttributeFormat( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( sizeof( Real32 ) * 3, layout.getAttributeSize( VertexAttribute::Name::POSITION ) );

	ASSERT_EQ( sizeof( Real32 ) * 3, layout.getAttributeOffset( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, layout.getAttributeFormat( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( sizeof( Real32 ) * 3, layout.getAttributeSize( VertexAttribute::Name::NORMAL ) );

	ASSERT_EQ( sizeof( Real32 ) * 6, layout.getAttributeOffset( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( Format::R32G32_SFLOAT, layout.getAttributeFormat( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( sizeof( Real32 ) * 2, layout.getAttributeSize( VertexAttribute::Name::TEX_COORD ) );
}
