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

#include "QuadPrimitive.hpp"

using namespace crimild;

QuadPrimitive::QuadPrimitive(
    float width,
    float height,
    const VertexLayout &layout,
    const Vector2f &textureOffset,
    const Vector2f &textureScale,
    bool wireframe
) noexcept
    : Primitive( wireframe ? Primitive::Type::LINE_LOOP : Primitive::Type::TRIANGLE_STRIP )
{
    assert( false );
    /*
    float halfWidth = 0.5f * width;
    float halfHeigh = 0.5f * height;

    auto vbo = crimild::alloc< VertexBufferObject >( format, 4, nullptr );
    float *vertex = vbo->data();

    // bottom left vertex
    vertex[ format.getPositionsOffset() + 0 ] = -halfWidth;
    vertex[ format.getPositionsOffset() + 1 ] = -halfHeigh;
    vertex[ format.getPositionsOffset() + 2 ] = 0.0f;

    if ( format.hasNormals() ) {
        vertex[ format.getNormalsOffset() + 0 ] = 0.0f;
        vertex[ format.getNormalsOffset() + 1 ] = 0.0f;
        vertex[ format.getNormalsOffset() + 2 ] = 1.0f;
    }

    if ( format.hasTextureCoords() ) {
        vertex[ format.getTextureCoordsOffset() + 0 ] = textureOffset[ 0 ] + textureScale[ 0 ] * 0.0f;
        vertex[ format.getTextureCoordsOffset() + 1 ] = textureOffset[ 1 ] + textureScale[ 1 ] * 0.0f;
    }

    vertex += format.getVertexSize();

    // bottom right vertex
    vertex[ format.getPositionsOffset() + 0 ] = +halfWidth;
    vertex[ format.getPositionsOffset() + 1 ] = -halfHeigh;
    vertex[ format.getPositionsOffset() + 2 ] = 0.0f;

    if ( format.hasNormals() ) {
        vertex[ format.getNormalsOffset() + 0 ] = 0.0f;
        vertex[ format.getNormalsOffset() + 1 ] = 0.0f;
        vertex[ format.getNormalsOffset() + 2 ] = 1.0f;
    }

    if ( format.hasTextureCoords() ) {
        vertex[ format.getTextureCoordsOffset() + 0 ] = textureOffset[ 0 ] + textureScale[ 0 ] * 1.0f;
        vertex[ format.getTextureCoordsOffset() + 1 ] = textureOffset[ 1 ] + textureScale[ 1 ] * 0.0f;
    }

    vertex += format.getVertexSize();

    // top left vertex
    vertex[ format.getPositionsOffset() + 0 ] = -halfWidth;
    vertex[ format.getPositionsOffset() + 1 ] = +halfHeigh;
    vertex[ format.getPositionsOffset() + 2 ] = 0.0f;

    if ( format.hasNormals() ) {
        vertex[ format.getNormalsOffset() + 0 ] = 0.0f;
        vertex[ format.getNormalsOffset() + 1 ] = 0.0f;
        vertex[ format.getNormalsOffset() + 2 ] = 1.0f;
    }

    if ( format.hasTextureCoords() ) {
        vertex[ format.getTextureCoordsOffset() + 0 ] = textureOffset[ 0 ] + textureScale[ 0 ] * 0.0f;
        vertex[ format.getTextureCoordsOffset() + 1 ] = textureOffset[ 1 ] + textureScale[ 1 ] * 1.0f;
    }

    vertex += format.getVertexSize();

    // top right vertex
    vertex[ format.getPositionsOffset() + 0 ] = +halfWidth;
    vertex[ format.getPositionsOffset() + 1 ] = +halfHeigh;
    vertex[ format.getPositionsOffset() + 2 ] = 0.0f;

    if ( format.hasNormals() ) {
        vertex[ format.getNormalsOffset() + 0 ] = 0.0f;
        vertex[ format.getNormalsOffset() + 1 ] = 0.0f;
        vertex[ format.getNormalsOffset() + 2 ] = 1.0f;
    }

    if ( format.hasTextureCoords() ) {
        vertex[ format.getTextureCoordsOffset() + 0 ] = textureOffset[ 0 ] + textureScale[ 0 ] * 1.0f;
        vertex[ format.getTextureCoordsOffset() + 1 ] = textureOffset[ 1 ] + textureScale[ 1 ] * 1.0f;
    }

    vertex += format.getVertexSize();

    setVertexBuffer( vbo );

    unsigned short indices[] = { 0, 1, 2, 3 };
    auto ibo = crimild::alloc< IndexBufferObject >( 4, indices );
    setIndexBuffer( ibo );
    */
}
