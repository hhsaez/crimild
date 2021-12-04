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

#include "Primitives/QuadPrimitive.hpp"

#include "Mathematics/Vector3_constants.hpp"
#include "Mathematics/Vector_equality.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( QuadPrimitive, construction )
{
    auto p = crimild::alloc< QuadPrimitive >();

    auto expected = Array< VertexP3N3TC2 > {
        {
            .position = Vector3f { -1.0f, 1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 0.0f, 0.0f },
        },
        {
            .position = Vector3f { -1.0f, -1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 0.0f, 1.0f },
        },
        {
            .position = Vector3f { 1.0f, -1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 1.0f, 1.0f },
        },
        {
            .position = Vector3f { 1.0f, 1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 1.0f, 0.0f },
        },
    };

    ASSERT_EQ( Primitive::Type::TRIANGLES, p->getType() );

    ASSERT_EQ( 1, p->getVertexData().size() );

    auto vertices = p->getVertexData()[ 0 ];
    ASSERT_NE( nullptr, vertices );
    ASSERT_EQ( VertexP3N3TC2::getLayout(), vertices->getVertexLayout() );
    ASSERT_EQ( 32 * sizeof( Real32 ), vertices->getBufferView()->getLength() );

    ASSERT_EQ( expected.size(), vertices->getVertexCount() );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    ASSERT_NE( nullptr, positions );
    ASSERT_TRUE( isEqual( expected[ 0 ].position, positions->get< Vector3f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].position, positions->get< Vector3f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].position, positions->get< Vector3f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].position, positions->get< Vector3f >( 3 ) ) );

    auto normals = vertices->get( VertexAttribute::Name::NORMAL );
    ASSERT_NE( nullptr, normals );
    ASSERT_TRUE( isEqual( expected[ 0 ].normal, normals->get< Vector3f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].normal, normals->get< Vector3f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].normal, normals->get< Vector3f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].normal, normals->get< Vector3f >( 3 ) ) );

    auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );
    ASSERT_NE( nullptr, texCoords );
    ASSERT_TRUE( isEqual( expected[ 0 ].texCoord, texCoords->get< Vector2f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].texCoord, texCoords->get< Vector2f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].texCoord, texCoords->get< Vector2f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].texCoord, texCoords->get< Vector2f >( 3 ) ) );

    auto indices = p->getIndices();
    ASSERT_NE( nullptr, indices );
    ASSERT_EQ( 6, indices->getIndexCount() );
    ASSERT_EQ( Format::INDEX_32_UINT, indices->getFormat() );
    ASSERT_EQ( 0, indices->getIndex( 0 ) );
    ASSERT_EQ( 1, indices->getIndex( 1 ) );
    ASSERT_EQ( 2, indices->getIndex( 2 ) );
    ASSERT_EQ( 0, indices->getIndex( 3 ) );
    ASSERT_EQ( 2, indices->getIndex( 4 ) );
    ASSERT_EQ( 3, indices->getIndex( 5 ) );
}

TEST( QuadPrimitive, withCustomLayout )
{
    auto p = crimild::alloc< QuadPrimitive >( QuadPrimitive::Params { .layout = VertexP3TC2::getLayout() } );

    auto expected = Array< VertexP3TC2 > {
        {
            .position = Vector3f { -1.0f, 1.0f, 0.0f },
            .texCoord = Vector2f { 0.0f, 0.0f },
        },
        {
            .position = Vector3f { -1.0f, -1.0f, 0.0f },
            .texCoord = Vector2f { 0.0f, 1.0f },
        },
        {
            .position = Vector3f { 1.0f, -1.0f, 0.0f },
            .texCoord = Vector2f { 1.0f, 1.0f },
        },
        {
            .position = Vector3f { 1.0f, 1.0f, 0.0f },
            .texCoord = Vector2f { 1.0f, 0.0f },
        },
    };

    ASSERT_EQ( Primitive::Type::TRIANGLES, p->getType() );

    ASSERT_EQ( 1, p->getVertexData().size() );

    auto vertices = p->getVertexData()[ 0 ];
    ASSERT_NE( nullptr, vertices );
    ASSERT_EQ( VertexP3TC2::getLayout(), vertices->getVertexLayout() );
    ASSERT_EQ( 4 * sizeof( VertexP3TC2 ), vertices->getBufferView()->getLength() );

    ASSERT_EQ( expected.size(), vertices->getVertexCount() );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    ASSERT_NE( nullptr, positions );
    ASSERT_TRUE( isEqual( expected[ 0 ].position, positions->get< Vector3f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].position, positions->get< Vector3f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].position, positions->get< Vector3f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].position, positions->get< Vector3f >( 3 ) ) );

    auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );
    ASSERT_NE( nullptr, texCoords );
    ASSERT_TRUE( isEqual( expected[ 0 ].texCoord, texCoords->get< Vector2f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].texCoord, texCoords->get< Vector2f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].texCoord, texCoords->get< Vector2f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].texCoord, texCoords->get< Vector2f >( 3 ) ) );

    auto indices = p->getIndices();
    ASSERT_NE( nullptr, indices );
    ASSERT_EQ( 6, indices->getIndexCount() );
    ASSERT_EQ( Format::INDEX_32_UINT, indices->getFormat() );
    ASSERT_EQ( 0, indices->getIndex( 0 ) );
    ASSERT_EQ( 1, indices->getIndex( 1 ) );
    ASSERT_EQ( 2, indices->getIndex( 2 ) );
    ASSERT_EQ( 0, indices->getIndex( 3 ) );
    ASSERT_EQ( 2, indices->getIndex( 4 ) );
    ASSERT_EQ( 3, indices->getIndex( 5 ) );
}

TEST( QuadPrimitive, customSize )
{
    auto p = crimild::alloc< QuadPrimitive >( QuadPrimitive::Params { .size = Vector2f { 4.0f, 2.0f } } );

    auto expected = Array< VertexP3N3TC2 > {
        {
            .position = Vector3f { -4.0f, 2.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 0.0f, 0.0f },
        },
        {
            .position = Vector3f { -4.0f, -2.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 0.0f, 1.0f },
        },
        {
            .position = Vector3f { 4.0f, -2.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 1.0f, 1.0f },
        },
        {
            .position = Vector3f { 4.0f, 2.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 1.0f, 0.0f },
        },
    };

    ASSERT_EQ( Primitive::Type::TRIANGLES, p->getType() );
    ASSERT_EQ( 1, p->getVertexData().size() );

    auto vertices = p->getVertexData()[ 0 ];
    ASSERT_NE( nullptr, vertices );
    ASSERT_EQ( VertexP3N3TC2::getLayout(), vertices->getVertexLayout() );
    ASSERT_EQ( 32 * sizeof( Real32 ), vertices->getBufferView()->getLength() );

    ASSERT_EQ( expected.size(), vertices->getVertexCount() );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    ASSERT_NE( nullptr, positions );
    ASSERT_TRUE( isEqual( expected[ 0 ].position, positions->get< Vector3f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].position, positions->get< Vector3f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].position, positions->get< Vector3f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].position, positions->get< Vector3f >( 3 ) ) );

    auto normals = vertices->get( VertexAttribute::Name::NORMAL );
    ASSERT_NE( nullptr, normals );
    ASSERT_TRUE( isEqual( expected[ 0 ].normal, normals->get< Vector3f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].normal, normals->get< Vector3f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].normal, normals->get< Vector3f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].normal, normals->get< Vector3f >( 3 ) ) );

    auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );
    ASSERT_NE( nullptr, texCoords );
    ASSERT_TRUE( isEqual( expected[ 0 ].texCoord, texCoords->get< Vector2f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].texCoord, texCoords->get< Vector2f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].texCoord, texCoords->get< Vector2f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].texCoord, texCoords->get< Vector2f >( 3 ) ) );

    auto indices = p->getIndices();
    ASSERT_NE( nullptr, indices );
    ASSERT_EQ( 6, indices->getIndexCount() );
    ASSERT_EQ( Format::INDEX_32_UINT, indices->getFormat() );
    ASSERT_EQ( 0, indices->getIndex( 0 ) );
    ASSERT_EQ( 1, indices->getIndex( 1 ) );
    ASSERT_EQ( 2, indices->getIndex( 2 ) );
    ASSERT_EQ( 0, indices->getIndex( 3 ) );
    ASSERT_EQ( 2, indices->getIndex( 4 ) );
    ASSERT_EQ( 3, indices->getIndex( 5 ) );
}

TEST( QuadPrimitive, withTriangleStrip )
{
    auto p = crimild::alloc< QuadPrimitive >( QuadPrimitive::Params { .type = Primitive::Type::TRIANGLE_STRIP } );

    auto expected = Array< VertexP3N3TC2 > {
        {
            .position = Vector3f { -1.0f, 1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 0.0f, 0.0f },
        },
        {
            .position = Vector3f { -1.0f, -1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 0.0f, 1.0f },
        },
        {
            .position = Vector3f { 1.0f, -1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 1.0f, 1.0f },
        },
        {
            .position = Vector3f { 1.0f, 1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 1.0f, 0.0f },
        },
    };

    ASSERT_EQ( Primitive::Type::TRIANGLE_STRIP, p->getType() );
    ASSERT_EQ( 1, p->getVertexData().size() );

    auto vertices = p->getVertexData()[ 0 ];
    ASSERT_NE( nullptr, vertices );
    ASSERT_EQ( VertexP3N3TC2::getLayout(), vertices->getVertexLayout() );
    ASSERT_EQ( 32 * sizeof( Real32 ), vertices->getBufferView()->getLength() );

    ASSERT_EQ( expected.size(), vertices->getVertexCount() );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    ASSERT_NE( nullptr, positions );
    ASSERT_TRUE( isEqual( expected[ 0 ].position, positions->get< Vector3f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].position, positions->get< Vector3f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].position, positions->get< Vector3f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].position, positions->get< Vector3f >( 3 ) ) );

    auto normals = vertices->get( VertexAttribute::Name::NORMAL );
    ASSERT_NE( nullptr, normals );
    ASSERT_TRUE( isEqual( expected[ 0 ].normal, normals->get< Vector3f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].normal, normals->get< Vector3f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].normal, normals->get< Vector3f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].normal, normals->get< Vector3f >( 3 ) ) );

    auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );
    ASSERT_NE( nullptr, texCoords );
    ASSERT_TRUE( isEqual( expected[ 0 ].texCoord, texCoords->get< Vector2f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].texCoord, texCoords->get< Vector2f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].texCoord, texCoords->get< Vector2f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].texCoord, texCoords->get< Vector2f >( 3 ) ) );

    auto indices = p->getIndices();
    ASSERT_NE( nullptr, indices );
    ASSERT_EQ( 4, indices->getIndexCount() );
    ASSERT_EQ( Format::INDEX_32_UINT, indices->getFormat() );
    ASSERT_EQ( 1, indices->getIndex( 0 ) );
    ASSERT_EQ( 2, indices->getIndex( 1 ) );
    ASSERT_EQ( 0, indices->getIndex( 2 ) );
    ASSERT_EQ( 3, indices->getIndex( 3 ) );
}

TEST( QuadPrimitive, withLines )
{
    auto p = crimild::alloc< QuadPrimitive >( QuadPrimitive::Params { .type = Primitive::Type::LINES } );

    auto expected = Array< VertexP3N3TC2 > {
        {
            .position = Vector3f { -1.0f, 1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 0.0f, 0.0f },
        },
        {
            .position = Vector3f { -1.0f, -1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 0.0f, 1.0f },
        },
        {
            .position = Vector3f { 1.0f, -1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 1.0f, 1.0f },
        },
        {
            .position = Vector3f { 1.0f, 1.0f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 1.0f, 0.0f },
        },
    };

    ASSERT_EQ( Primitive::Type::LINES, p->getType() );
    ASSERT_EQ( 1, p->getVertexData().size() );

    auto vertices = p->getVertexData()[ 0 ];
    ASSERT_NE( nullptr, vertices );
    ASSERT_EQ( VertexP3N3TC2::getLayout(), vertices->getVertexLayout() );
    ASSERT_EQ( 32 * sizeof( Real32 ), vertices->getBufferView()->getLength() );

    ASSERT_EQ( expected.size(), vertices->getVertexCount() );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );
    ASSERT_NE( nullptr, positions );
    ASSERT_TRUE( isEqual( expected[ 0 ].position, positions->get< Vector3f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].position, positions->get< Vector3f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].position, positions->get< Vector3f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].position, positions->get< Vector3f >( 3 ) ) );

    auto normals = vertices->get( VertexAttribute::Name::NORMAL );
    ASSERT_NE( nullptr, normals );
    ASSERT_TRUE( isEqual( expected[ 0 ].normal, normals->get< Vector3f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].normal, normals->get< Vector3f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].normal, normals->get< Vector3f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].normal, normals->get< Vector3f >( 3 ) ) );

    auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );
    ASSERT_NE( nullptr, texCoords );
    ASSERT_TRUE( isEqual( expected[ 0 ].texCoord, texCoords->get< Vector2f >( 0 ) ) );
    ASSERT_TRUE( isEqual( expected[ 1 ].texCoord, texCoords->get< Vector2f >( 1 ) ) );
    ASSERT_TRUE( isEqual( expected[ 2 ].texCoord, texCoords->get< Vector2f >( 2 ) ) );
    ASSERT_TRUE( isEqual( expected[ 3 ].texCoord, texCoords->get< Vector2f >( 3 ) ) );

    auto indices = p->getIndices();
    ASSERT_NE( nullptr, indices );
    ASSERT_EQ( 8, indices->getIndexCount() );
    ASSERT_EQ( Format::INDEX_32_UINT, indices->getFormat() );
    ASSERT_EQ( 0, indices->getIndex( 0 ) );
    ASSERT_EQ( 1, indices->getIndex( 1 ) );
    ASSERT_EQ( 1, indices->getIndex( 2 ) );
    ASSERT_EQ( 2, indices->getIndex( 3 ) );
    ASSERT_EQ( 2, indices->getIndex( 4 ) );
    ASSERT_EQ( 3, indices->getIndex( 5 ) );
    ASSERT_EQ( 3, indices->getIndex( 6 ) );
    ASSERT_EQ( 0, indices->getIndex( 7 ) );
}
