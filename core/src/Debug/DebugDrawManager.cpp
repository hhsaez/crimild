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

#include "Debug/DebugDrawManager.hpp"

#include "Crimild_Mathematics.hpp"
#include "Primitives/Primitive.hpp"

using namespace crimild;

std::vector< DebugDrawManager::Renderable > DebugDrawManager::s_renderables;
std::vector< DebugDrawManager::Renderable > DebugDrawManager::s_persistentRenderables;

void DebugDrawManager::reset( bool clearAll ) noexcept
{
    s_renderables.clear();
    if ( clearAll ) {
        s_persistentRenderables.clear();
    }
}

void DebugDrawManager::addLine(
    const Point3f &from,
    const Point3f &to,
    const ColorRGB &color,
    float width,
    float duration,
    bool depthEnabled
) noexcept
{
    if ( isEqual( to, from ) ) {
        return;
    }

    // Use a single primitive for all lines
    // This might lead to some performance issues when rendering many lines, but I'll deal with that later.
    static auto primitive = [] {
        auto primitive = crimild::alloc< Primitive >( Primitive::Type::LINES );
        const auto layout = VertexLayout::P3_N3_TC2;

        auto vertices = crimild::alloc< VertexBuffer >( layout, 2 );
        auto positions = vertices->get( VertexAttribute::Name::POSITION );
        positions->set( 0, Point3f { 0, 0, 0 } );
        positions->set( 1, Point3f { 0, 0, -1 } );
        primitive->setVertexData( { vertices } );

        auto indices = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, Array< UInt32 > { 0, 1 } );
        primitive->setIndices( indices );
        return primitive;
    }();

    const auto up = [ & ] {
        auto ret = cross( Vector3f( to ), Vector3f( from ) );
        const auto retLengthSquared = length2( ret );
        if ( !isNaN( ret ) && !isNaN( retLengthSquared ) && retLengthSquared > 0 ) {
            return ret;
        } else {
            Vector3f right, up;
            orthonormalBasis( normalize( Vector3f( to ) ), right, up );
            if ( isNaN( up ) || isZero( length2( up ) ) ) {
                return Vector3f { 0, 1, 0 };
            }
            return up;
        }
    }();

    s_renderables.push_back(
        Renderable {
            primitive,
            lookAt( from, to, up )( scale( distance( from, to ) ) ),
            color,
            duration,
            depthEnabled,
        }
    );
}

void DebugDrawManager::addCross(
    const Point3f &position,
    const ColorRGB &color,
    float size,
    float duration,
    bool depthEnabled
) noexcept
{
}

void DebugDrawManager::addSphere(
    const Point3f &center,
    float radius,
    const ColorRGB &color,
    float duration,
    bool depthEnabled
) noexcept
{
}

void DebugDrawManager::addCircle( const Point3f &center, const Vector3f &normal, float radius, const ColorRGB &color, float duration, bool depthEnabled ) noexcept
{
    // Shared primitive
    static auto primitive = [] {
        auto primitive = crimild::alloc< Primitive >( Primitive::Type::LINES );
        const auto layout = VertexLayout::P3_N3_TC2;

        const auto N = 16;
        auto vertices = crimild::alloc< VertexBuffer >( layout, N );
        auto positions = vertices->get( VertexAttribute::Name::POSITION );
        float phi = 0;
        for ( size_t i = 0; i < N; ++i ) {
            positions->set( i, Point3f { cos( phi ), sin( phi ), 0 } );
            phi += numbers::TWO_PI / N;
        }
        primitive->setVertexData( { vertices } );

        auto indices = crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, 2 * N );
        for ( auto i = 0; i < N; ++i ) {
            indices->setIndex( 2 * i, i );
            indices->setIndex( 2 * i + 1, ( i + 1 ) % N );
        }
        primitive->setIndices( indices );
        return primitive;
    }();

    s_renderables.push_back(
        Renderable {
            primitive,
            lookAt( center, center + normal, Vector3 { 0, 1, 0 } )( scale( radius ) ),
            color,
            duration,
            depthEnabled,
        }
    );
}

void DebugDrawManager::addAxes(
    const Transformation &transformation,
    float duration,
    bool depthEnabled
) noexcept
{
}

void DebugDrawManager::addText(
    const Point3f &position,
    std::string_view text,
    const ColorRGB &color,
    float duration,
    bool depthEnabled
) noexcept
{
}
