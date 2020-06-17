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

#include "Rendering/VertexLayout.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( VertexLayout, P3 )
{
	auto v = VertexLayout::P3;

	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getSize() );
	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
}

TEST( VertexLayout, positionsOnly )
{
	auto v = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
	};

	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getSize() );
	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
}

TEST( VertexLayout, positionsAndNormals )
{
	auto v = VertexLayout()
	    .withAttribute< Vector3f >( VertexAttribute::Name::POSITION )
	    .withAttribute< Vector3f >( VertexAttribute::Name::NORMAL );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 6, v.getSize() );

	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeOffset( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::NORMAL ) );
}

TEST( VertexLayout, multipleAttributes )
{
	auto v = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::COLOR, utils::getFormat< RGBColorf >() },
		{ VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() },
	};

	ASSERT_EQ( sizeof( crimild::Real32 ) * 11, v.getSize() );

	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::POSITION ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeOffset( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::NORMAL ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 6, v.getAttributeOffset( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::COLOR ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 9, v.getAttributeOffset( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( Format::R32G32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 2, v.getAttributeSize( VertexAttribute::Name::TEX_COORD ) );
}

TEST( VertexLayout, multipleAttributesDifferentOrder )
{
	auto v = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() },
		{ VertexAttribute::Name::COLOR, utils::getFormat< RGBColorf >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
	};

	ASSERT_EQ( sizeof( crimild::Real32 ) * 11, v.getSize() );

	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::POSITION ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeOffset( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( Format::R32G32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 2, v.getAttributeSize( VertexAttribute::Name::TEX_COORD ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 5, v.getAttributeOffset( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::COLOR ) );

 	ASSERT_EQ( sizeof( crimild::Real32 ) * 8, v.getAttributeOffset( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::NORMAL ) );
}

TEST( VertexLayout, fromArray )
{
    auto attribs = containers::Array< VertexAttribute > {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() },
		{ VertexAttribute::Name::COLOR, utils::getFormat< RGBColorf >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
    };

    auto v = VertexLayout( attribs );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 11, v.getSize() );

	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::POSITION ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeOffset( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( Format::R32G32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 2, v.getAttributeSize( VertexAttribute::Name::TEX_COORD ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 5, v.getAttributeOffset( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::COLOR ) );

 	ASSERT_EQ( sizeof( crimild::Real32 ) * 8, v.getAttributeOffset( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::NORMAL ) );
}

TEST( VertexLayout, eachAttribute )
{
    auto attribs = containers::Array< VertexAttribute > {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::TEX_COORD, utils::getFormat< Vector2f >() },
		{ VertexAttribute::Name::COLOR, utils::getFormat< RGBColorf >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
    };

    auto v = VertexLayout( attribs );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 11, v.getSize() );

    auto callCount = 0;
    v.eachAttribute(
        [&]( auto &attrib ) {
            ASSERT_EQ( attribs[ callCount++ ].name, attrib.name );
        }
    );
    ASSERT_EQ( 4, callCount );
}

TEST( VertexLayout, withAttribute )
{
	auto v = VertexLayout()
	    .withAttribute< Vector3f >( VertexAttribute::Name::POSITION )
	    .withAttribute< Vector2f >( VertexAttribute::Name::TEX_COORD )
	    .withAttribute< RGBColorf >( VertexAttribute::Name::COLOR )
	    .withAttribute< Vector3f >( VertexAttribute::Name::NORMAL );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 11, v.getSize() );

	ASSERT_EQ( 0, v.getAttributeOffset( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::POSITION ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::POSITION ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeOffset( VertexAttribute::Name::TEX_COORD ) );
    ASSERT_EQ( Format::R32G32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::TEX_COORD ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 2, v.getAttributeSize( VertexAttribute::Name::TEX_COORD ) );

	ASSERT_EQ( sizeof( crimild::Real32 ) * 5, v.getAttributeOffset( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::COLOR ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::COLOR ) );

 	ASSERT_EQ( sizeof( crimild::Real32 ) * 8, v.getAttributeOffset( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( Format::R32G32B32_SFLOAT, v.getAttributeFormat( VertexAttribute::Name::NORMAL ) );
	ASSERT_EQ( sizeof( crimild::Real32 ) * 3, v.getAttributeSize( VertexAttribute::Name::NORMAL ) );
}

TEST( VertexLayout, equality )
{
	auto v1 = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::COLOR, utils::getFormat< RGBColorf >() },
	};

	auto v2 = VertexLayout {
		{ VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
		{ VertexAttribute::Name::COLOR, utils::getFormat< RGBColorf >() },
	};

	ASSERT_EQ( v1, v2 );
}
