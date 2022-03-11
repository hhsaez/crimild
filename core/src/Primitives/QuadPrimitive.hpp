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

#ifndef CRIMILD_PRIMITIVES_QUAD_
#define CRIMILD_PRIMITIVES_QUAD_

#include "Mathematics/Vector2.hpp"
#include "Mathematics/Vector2_constants.hpp"
#include "Primitive.hpp"
#include "Rendering/Vertex.hpp"

namespace crimild {

    /**
        \brief Constructs a quad primitive

        Builds a quad primitive using the provided params. The resulting primitive is
        oriented in the XY-plane, with the front face pointing at the +Z coordinate.

        [-size,+size,0]------------------[+size,+size,0]
              |                              |
              |                              |
              |                              |
              |                              |
              |                              |
              |           [0,0,0]            |
              |                              |
              |                              |
              |                              |
              |                              |
              |                              |
        [-size,-size,0]------------------[+size,-size,0]

        The quad vertices are centered at the local origin, and extend in the range [-size,+size],
        so the actual width and height is twice the given size.

        If the vertex layout requires normals to be generated, they will be created pointing
        at the +Z direction.

        If the vertex layout requires texture coordinates to be generated, they will be
        generated with the origin at the upper-left corner:

        [0,0]--------------[1,0]
          |                  |
          |                  |
          |                  |
          |                  |
          |                  |
        [0,1]--------------[1,1]

        Other layout properties are ignored.

        By default, we use TRIANGLES as primitive type, which may not be the most efficient
        one, but it's the easiest one to integrate with other primitives and pipelines.
     */
    class QuadPrimitive : public Primitive {
    public:
        static SharedPointer< Primitive > UNIT_QUAD;

    public:
        struct Params {
            Primitive::Type type = Primitive::Type::TRIANGLES;
            Vector2f size = Vector2f::Constants::ONE;
            VertexLayout layout = VertexP3N3TC2::getLayout();
            Vector2f texCoordOffset = Vector2f::Constants::ZERO;
            Vector2f texCoordScale = Vector2f::Constants::ONE;
        };

    public:
        QuadPrimitive( void ) noexcept;
        explicit QuadPrimitive( const Params &params ) noexcept;
        virtual ~QuadPrimitive( void ) = default;
    };

    using QuadPrimitivePtr = SharedPointer< QuadPrimitive >;

}

#endif
