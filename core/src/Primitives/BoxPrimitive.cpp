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

#include "BoxPrimitive.hpp"

#include <vector>

using namespace crimild;

BoxPrimitive::BoxPrimitive( float width, float height, float depth, const VertexFormat &format )
    : Primitive( Primitive::Type::TRIANGLES )
{
    float halfWidth = 0.5f * width;
    float halfHeight = 0.5f * height;
    float halfDepth = 0.5f * depth;

    float vertices[] = {
        // top
        -halfWidth, +halfHeight, -halfDepth, 0.0f, 1.0f, 0.0f,
        -halfWidth, +halfHeight, +halfDepth, 0.0f, 1.0f, 0.0f,
        +halfWidth, +halfHeight, +halfDepth, 0.0f, 1.0f, 0.0f,
        +halfWidth, +halfHeight, -halfDepth, 0.0f, 1.0f, 0.0f,

        // front
        -halfWidth, +halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f,
        -halfWidth, -halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f,
        +halfWidth, -halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f,
        +halfWidth, +halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f,

        // back
        +halfWidth, +halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f,
        +halfWidth, -halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f,
        -halfWidth, -halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f,
        -halfWidth, +halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f,

        // left
        -halfWidth, +halfHeight, -halfDepth, -1.0f, 0.0f, 0.0f,
        -halfWidth, -halfHeight, -halfDepth, -1.0f, 0.0f, 0.0f,
        -halfWidth, -halfHeight, +halfDepth, -1.0f, 0.0f, 0.0f,
        -halfWidth, +halfHeight, +halfDepth, -1.0f, 0.0f, 0.0f,

        // right
        +halfWidth, +halfHeight, +halfDepth, 1.0f, 0.0f, 0.0f,
        +halfWidth, -halfHeight, +halfDepth, 1.0f, 0.0f, 0.0f,
        +halfWidth, -halfHeight, -halfDepth, 1.0f, 0.0f, 0.0f,
        +halfWidth, +halfHeight, -halfDepth, 1.0f, 0.0f, 0.0f,

        // bottom
        +halfWidth, -halfHeight, -halfDepth, 0.0f, -1.0f, 0.0f,
        +halfWidth, -halfHeight, +halfDepth, 0.0f, -1.0f, 0.0f,
        -halfWidth, -halfHeight, +halfDepth, 0.0f, -1.0f, 0.0f,
        -halfWidth, -halfHeight, -halfDepth, 0.0f, -1.0f, 0.0f,
    };

    unsigned short indices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    VertexBufferObjectPtr vbo( new VertexBufferObject( VertexFormat::VF_P3_N3, 24, vertices ) );
    setVertexBuffer( vbo );

    IndexBufferObjectPtr ibo( new IndexBufferObject( 36, indices ) );
    setIndexBuffer( ibo );
}

BoxPrimitive::~BoxPrimitive( void )
{
    
}

