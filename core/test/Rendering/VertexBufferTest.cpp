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

#include "Rendering/VertexBuffer.hpp"

#include "Coding/MemoryDecoder.hpp"
#include "Coding/MemoryEncoder.hpp"
#include "Rendering/Buffer.hpp"
#include "Rendering/BufferAccessor.hpp"
#include "Rendering/BufferView.hpp"
#include "Rendering/Format.hpp"
#include "Rendering/Vertex.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( VertexBuffer, construction )
{
    auto layout = VertexLayout {
        { VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
    };

    auto vertices = crimild::alloc< VertexBuffer >(
        layout,
        Array< crimild::Real32 > {
            -0.5f,
            -0.5f,
            0.0f,
            0.5f,
            -0.5f,
            0.0f,
            0.0f,
            0.5f,
            0.0f,
        }
    );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );

    ASSERT_EQ( 3, vertices->getVertexCount() );
    ASSERT_EQ( layout, vertices->getVertexLayout() );
    ASSERT_NE( nullptr, positions );
    ASSERT_EQ( ( Vector3f { -0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 0 ) );
}

TEST( VertexBuffer, setSingleValue )
{
    auto vertices = crimild::alloc< VertexBuffer >( VertexLayout::P3, 3 );

    ASSERT_EQ( 3, vertices->getVertexCount() );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );

    positions->set( 0, Vector3f { -0.5f, -0.5f, 0.0f } );
    ASSERT_EQ( ( Vector3f { -0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 0 ) );
}

TEST( VertexBuffer, setMultipleValues )
{
    auto vertices = crimild::alloc< VertexBuffer >( VertexLayout::P3, 3 );

    ASSERT_EQ( 3, vertices->getVertexCount() );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );

    positions->set(
        Array< crimild::Real32 > {
            -0.5f,
            -0.5f,
            0.0f,
            0.5f,
            -0.5f,
            0.0f,
            0.0f,
            0.5f,
            0.0f,
        }
    );

    ASSERT_EQ( ( Vector3f { -0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 0 ) );
    ASSERT_EQ( ( Vector3f { 0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 1 ) );
    ASSERT_EQ( ( Vector3f { 0.0f, 0.5f, 0.0f } ), positions->get< Vector3f >( 2 ) );
}

TEST( VertexBuffer, setPositionsInterleaved )
{
    auto vertices = crimild::alloc< VertexBuffer >( VertexLayout::P3_TC2, 3 );

    ASSERT_EQ( 3, vertices->getVertexCount() );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );

    positions->set(
        Array< crimild::Real32 > {
            -0.5f,
            -0.5f,
            0.0f,
            0.5f,
            -0.5f,
            0.0f,
            0.0f,
            0.5f,
            0.0f,
        }
    );

    ASSERT_EQ( ( Vector3f { -0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 0 ) );
    ASSERT_EQ( ( Vector3f { 0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 1 ) );
    ASSERT_EQ( ( Vector3f { 0.0f, 0.5f, 0.0f } ), positions->get< Vector3f >( 2 ) );
}

TEST( VertexBuffer, setTexCoordsInterleaved )
{
    auto vertices = crimild::alloc< VertexBuffer >( VertexLayout::P3_TC2, 3 );

    ASSERT_EQ( 3, vertices->getVertexCount() );

    auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );

    texCoords->set(
        Array< crimild::Real32 > {
            0.0,
            0.0,
            0.0,
            1.0,
            1.0,
            1.0,
        }
    );

    ASSERT_EQ( ( Vector2f { 0.0f, 0.0f } ), texCoords->get< Vector2f >( 0 ) );
    ASSERT_EQ( ( Vector2f { 0.f, 1.0f } ), texCoords->get< Vector2f >( 1 ) );
    ASSERT_EQ( ( Vector2f { 1.0f, 1.0f } ), texCoords->get< Vector2f >( 2 ) );
}

TEST( VertexBuffer, setInterleaved )
{
    auto vertices = crimild::alloc< VertexBuffer >( VertexLayout::P3_TC2, 3 );

    ASSERT_EQ( 3, vertices->getVertexCount() );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );

    positions->set(
        Array< crimild::Real32 > {
            -0.5f,
            -0.5f,
            0.0f,
            0.5f,
            -0.5f,
            0.0f,
            0.0f,
            0.5f,
            0.0f,
        }
    );

    auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );

    texCoords->set(
        Array< crimild::Real32 > {
            0.0,
            0.0,
            0.0,
            1.0,
            1.0,
            1.0,
        }
    );

    ASSERT_EQ( ( Vector3f { -0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 0 ) );
    ASSERT_EQ( ( Vector3f { 0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 1 ) );
    ASSERT_EQ( ( Vector3f { 0.0f, 0.5f, 0.0f } ), positions->get< Vector3f >( 2 ) );

    ASSERT_EQ( ( Vector2f { 0.0f, 0.0f } ), texCoords->get< Vector2f >( 0 ) );
    ASSERT_EQ( ( Vector2f { 0.f, 1.0f } ), texCoords->get< Vector2f >( 1 ) );
    ASSERT_EQ( ( Vector2f { 1.0f, 1.0f } ), texCoords->get< Vector2f >( 2 ) );
}

TEST( VertexBuffer, setInterleavedVertex )
{
    auto vertices = crimild::alloc< VertexBuffer >( VertexP3N3TC2::getLayout(), 3 );

    ASSERT_EQ( 3, vertices->getVertexCount() );

    auto positions = vertices->get( VertexAttribute::Name::POSITION );

    positions->set(
        Array< crimild::Real32 > {
            -0.5f,
            -0.5f,
            0.0f,
            0.5f,
            -0.5f,
            0.0f,
            0.0f,
            0.5f,
            0.0f,
        }
    );

    auto normals = vertices->get( VertexAttribute::Name::NORMAL );
    normals->set(
        Array< Vector3f > {
            Vector3f::Constants::UNIT_Z,
            Vector3f::Constants::UNIT_Z,
            Vector3f::Constants::UNIT_Z,
        }
    );

    auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );

    texCoords->set(
        Array< crimild::Real32 > {
            0.0,
            0.0,
            0.0,
            1.0,
            1.0,
            1.0,
        }
    );

    ASSERT_EQ( ( Vector3f { -0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 0 ) );
    ASSERT_EQ( ( Vector3f { 0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 1 ) );
    ASSERT_EQ( ( Vector3f { 0.0f, 0.5f, 0.0f } ), positions->get< Vector3f >( 2 ) );

    ASSERT_EQ( Vector3f::Constants::UNIT_Z, normals->get< Vector3f >( 0 ) );
    ASSERT_EQ( Vector3f::Constants::UNIT_Z, normals->get< Vector3f >( 1 ) );
    ASSERT_EQ( Vector3f::Constants::UNIT_Z, normals->get< Vector3f >( 2 ) );

    ASSERT_EQ( ( Vector2f { 0.0f, 0.0f } ), texCoords->get< Vector2f >( 0 ) );
    ASSERT_EQ( ( Vector2f { 0.f, 1.0f } ), texCoords->get< Vector2f >( 1 ) );
    ASSERT_EQ( ( Vector2f { 1.0f, 1.0f } ), texCoords->get< Vector2f >( 2 ) );
}

TEST( VertexBuffer, eachPosition )
{
    auto vertices = crimild::alloc< VertexBuffer >( VertexLayout::P3_TC2, 3 );

    ASSERT_EQ( 3, vertices->getVertexCount() );

    auto data = Array< float > {
        -0.5f,
        -0.5f,
        0.0f,
        0.5f,
        -0.5f,
        0.0f,
        0.0f,
        0.5f,
        0.0f,
    };

    auto positions = vertices->get( VertexAttribute::Name::POSITION );

    positions->set( data );

    positions->each< Vector3f >(
        [ & ]( auto &v, auto i ) {
            ASSERT_EQ( data[ i * 3 + 0 ], v[ 0 ] );
            ASSERT_EQ( data[ i * 3 + 1 ], v[ 1 ] );
            ASSERT_EQ( data[ i * 3 + 2 ], v[ 2 ] );
        }
    );
}

TEST( VertexBuffer, eachTexCoord )
{
    auto vertices = crimild::alloc< VertexBuffer >( VertexLayout::P3_TC2, 3 );

    ASSERT_EQ( 3, vertices->getVertexCount() );

    auto data = Array< float > {
        0.0,
        0.0,
        0.0,
        1.0,
        1.0,
        1.0,
    };

    auto texCoords = vertices->get( VertexAttribute::Name::TEX_COORD );

    texCoords->set( data );

    texCoords->each< Vector2f >(
        [ & ]( auto &v, auto i ) {
            ASSERT_EQ( data[ i * 2 + 0 ], v[ 0 ] );
            ASSERT_EQ( data[ i * 2 + 1 ], v[ 1 ] );
        }
    );
}

TEST( VertexBuffer, sparseData )
{
    /*
        struct VertexLayoutType {
                Vector3f position;
                Vector3f normal;
                Vector2i weights;
                Size index;
        };

        auto layout = VertexLayout {
                { VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
                { VertexAttribute::Name::NORMAL, utils::getFormat< Vector3f >() },
                { VertexAttribute::Name::BLEND_INDICES, utils::getFormat< Vector2i >() },
                { VertexAttribute::Name::INDEX, utils::getFormat< crimild::Size >() },
        };

        auto vertices = crimild::alloc< VertexBuffer >(
                layout,
                Array< VertexType > {

                }
        );
        */
}
