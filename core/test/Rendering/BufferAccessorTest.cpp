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

#include "Rendering/BufferAccessor.hpp"

#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Matrix3.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/Vector_equality.hpp"

#include "gtest/gtest.h"

using namespace crimild;

TEST( BufferAccessor, vertexData )
{
    auto data = Array< crimild::Real32 > {
        -0.5f,
        0.5f,
        0.0f,
        0.0f,
        0.0f,
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        1.0f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        1.0f,
        0.5f,
        0.5f,
        0.0f,
        1.0f,
        0.0f,
    };

    auto buffer = crimild::alloc< Buffer >( data );
    auto view = crimild::alloc< BufferView >( BufferView::Target::VERTEX, buffer, 0, 5 * sizeof( crimild::Real32 ) );

    auto positions = crimild::alloc< BufferAccessor >( view, 0, 3 * sizeof( crimild::Real32 ) );
    ASSERT_EQ( 0, positions->getOffset() );
    ASSERT_EQ( 3 * sizeof( crimild::Real32 ), positions->getSize() );
    ASSERT_EQ( ( Vector3f { -0.5f, 0.5f, 0.0f } ), positions->get< Vector3f >( 0 ) );
    ASSERT_EQ( ( Vector3f { -0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 1 ) );
    ASSERT_EQ( ( Vector3f { 0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 2 ) );
    ASSERT_EQ( ( Vector3f { 0.5f, 0.5f, 0.0f } ), positions->get< Vector3f >( 3 ) );

    auto texCoords = crimild::alloc< BufferAccessor >( view, 3 * sizeof( crimild::Real32 ), 2 * sizeof( crimild::Real32 ) );
    ASSERT_EQ( 3 * sizeof( crimild::Real32 ), texCoords->getOffset() );
    ASSERT_EQ( 2 * sizeof( crimild::Real32 ), texCoords->getSize() );
    ASSERT_EQ( ( Vector2f { 0.0f, 0.0f } ), texCoords->get< Vector2f >( 0 ) );
    ASSERT_EQ( ( Vector2f { 0.0f, 1.0f } ), texCoords->get< Vector2f >( 1 ) );
    ASSERT_EQ( ( Vector2f { 1.0f, 1.0f } ), texCoords->get< Vector2f >( 2 ) );
    ASSERT_EQ( ( Vector2f { 1.0f, 0.0f } ), texCoords->get< Vector2f >( 3 ) );
}

TEST( BufferAccessor, vertexFromStruct )
{
    struct Vertex {
        Vector3f position;
        Vector3f normal;
        Vector2f texCoord;
        Vector2i indices;

        crimild::Bool operator==( const Vertex &other ) const noexcept
        {
            return position == other.position
                   && normal == other.normal
                   && texCoord == other.texCoord
                   && indices == other.indices;
        }
    };

    auto data = Array< Vertex > {
        {
            .position = Vector3f { -0.5f, -0.5f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 0.0f, 1.0f },
            .indices = Vector2i { 0, 1 },
        },
        {
            .position = Vector3f { 0.5f, -0.5f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 1.0f, 1.0f },
            .indices = Vector2i { 1, 1 },
        },
        {
            .position = Vector3f { 0.0f, 0.5f, 0.0f },
            .normal = Vector3f::Constants::UNIT_Z,
            .texCoord = Vector2f { 0.5f, 0.0f },
            .indices = Vector2i { 2, 1 },
        },
    };

    auto buffer = crimild::alloc< Buffer >( data );
    auto view = crimild::alloc< BufferView >( BufferView::Target::VERTEX, buffer, 0, sizeof( Vertex ) );
    auto vertices = crimild::alloc< BufferAccessor >( view, 0 );

    ASSERT_EQ( view->getStride(), vertices->getSize() );
    ASSERT_EQ( data[ 0 ], vertices->get< Vertex >( 0 ) );
    ASSERT_EQ( data[ 1 ], vertices->get< Vertex >( 1 ) );
    ASSERT_EQ( data[ 2 ], vertices->get< Vertex >( 2 ) );
}

TEST( BufferAccessor, uniformData )
{
    struct Uniform {
        Matrix4f proj;
        Matrix3f view;
        Matrix4f normal;
        ColorRGBA color;

        crimild::Bool operator==( const Uniform &other ) const noexcept
        {
            return proj == other.proj
                   && view == other.view
                   && normal == other.normal
                   && color == other.color;
        }
    };

    auto data = Uniform {
        .proj = Matrix4f::Constants::IDENTITY,
        .view = Matrix3f::Constants::IDENTITY,
        .normal = Matrix4f::Constants::ZERO,
        .color = ColorRGBA::Constants::WHITE,
    };

    auto buffer = crimild::alloc< Buffer >( data );
    auto view = crimild::alloc< BufferView >( BufferView::Target::UNIFORM, buffer, 0, sizeof( Uniform ) );
    auto accessor = crimild::alloc< BufferAccessor >( view );

    ASSERT_EQ( buffer->getSize(), accessor->getSize() );
    ASSERT_EQ( view->getStride(), accessor->getSize() );
    ASSERT_EQ( data.proj, accessor->get< Uniform >().proj );
    ASSERT_EQ( data.view, accessor->get< Uniform >().view );
    ASSERT_EQ( data.normal, accessor->get< Uniform >().normal );
    ASSERT_EQ( data.color, accessor->get< Uniform >().color );
}

TEST( BufferAccessor, setSingleValue )
{
    auto buffer = crimild::alloc< Buffer >( Array< Vector3f >( 3 ) );
    auto view = crimild::alloc< BufferView >( BufferView::Target::VERTEX, buffer, 0, sizeof( Vector3f ) );
    auto positions = crimild::alloc< BufferAccessor >( view );

    ASSERT_EQ( 3, view->getCount() );

    positions->set( 0, Vector3f { -0.5f, -0.5f, 0.0f } );
    ASSERT_EQ( ( Vector3f { -0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 0 ) );
}

TEST( BufferAccessor, setMultipleValues )
{
    auto buffer = crimild::alloc< Buffer >( Array< Vector3f >( 3 ) );
    auto view = crimild::alloc< BufferView >( BufferView::Target::VERTEX, buffer, 0, sizeof( Vector3f ) );
    auto positions = crimild::alloc< BufferAccessor >( view );

    ASSERT_EQ( 3, view->getCount() );

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
        } );

    ASSERT_EQ( ( Vector3f { -0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 0 ) );
    ASSERT_EQ( ( Vector3f { 0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 1 ) );
    ASSERT_EQ( ( Vector3f { 0.0f, 0.5f, 0.0f } ), positions->get< Vector3f >( 2 ) );
}

TEST( BufferAccessor, setInterleavedMultipleValues )
{
    struct Vertex {
        Vector3f position;
        Vector2f texCoord;
    };

    auto buffer = crimild::alloc< Buffer >( Array< Vertex >( 3 ) );
    auto view = crimild::alloc< BufferView >( BufferView::Target::VERTEX, buffer, 0, sizeof( Vertex ) );
    auto positions = crimild::alloc< BufferAccessor >( view, offsetof( Vertex, position ), sizeof( Vector3f ) );
    auto texCoords = crimild::alloc< BufferAccessor >( view, offsetof( Vertex, texCoord ), sizeof( Vector2f ) );

    ASSERT_EQ( 3, view->getCount() );

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
        } );

    texCoords->set(
        Array< crimild::Real32 > {
            0.0,
            0.0,
            0.0,
            1.0,
            1.0,
            1.0,
        } );

    ASSERT_EQ( ( Vector3f { -0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 0 ) );
    ASSERT_EQ( ( Vector3f { 0.5f, -0.5f, 0.0f } ), positions->get< Vector3f >( 1 ) );
    ASSERT_EQ( ( Vector3f { 0.0f, 0.5f, 0.0f } ), positions->get< Vector3f >( 2 ) );

    ASSERT_EQ( ( Vector2f { 0.0f, 0.0f } ), texCoords->get< Vector2f >( 0 ) );
    ASSERT_EQ( ( Vector2f { 0.f, 1.0f } ), texCoords->get< Vector2f >( 1 ) );
    ASSERT_EQ( ( Vector2f { 1.0f, 1.0f } ), texCoords->get< Vector2f >( 2 ) );
}

TEST( BufferAccessor, eachPosition )
{
    struct Vertex {
        Vector3f position;
        Vector2f texCoord;
    };

    auto buffer = crimild::alloc< Buffer >( Array< Vertex >( 3 ) );
    auto view = crimild::alloc< BufferView >( BufferView::Target::VERTEX, buffer, 0, sizeof( Vertex ) );
    auto positions = crimild::alloc< BufferAccessor >( view, offsetof( Vertex, position ), sizeof( Vector3f ) );
    auto texCoords = crimild::alloc< BufferAccessor >( view, offsetof( Vertex, texCoord ), sizeof( Vector2f ) );

    ASSERT_EQ( 3, view->getCount() );

    auto positionData = Array< Vector3f > {
        Vector3f { -0.5f, -0.5f, 0.0f },
        Vector3f { 0.5f, -0.5f, 0.0f },
        Vector3f { 0.0f, 0.5f, 0.0f },
    };

    positions->set( positionData );

    auto callCount = 0;
    positions->each< Vector3f >(
        [ & ]( const Vector3f &p, crimild::Size index ) {
            ASSERT_EQ( positionData[ index ], p );
            callCount++;
        } );
    ASSERT_EQ( 3, callCount );

    auto texCoordData = Array< Vector2f > {
        Vector2f { 0.0, 0.0 },
        Vector2f { 0.0, 1.0 },
        Vector2f { 1.0, 1.0 },
    };

    texCoords->set( texCoordData );

    callCount = 0;
    texCoords->each< Vector2f >(
        [ & ]( const Vector2f &uv, crimild::Size index ) {
            ASSERT_EQ( texCoordData[ index ], uv );
            callCount++;
        } );
    ASSERT_EQ( 3, callCount );
}
