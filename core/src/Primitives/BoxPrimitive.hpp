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

#ifndef CRIMILD_PRIMITIVES_BOX_
#define CRIMILD_PRIMITIVES_BOX_

#include "Mathematics/Vector3.hpp"
#include "Primitive.hpp"
#include "Rendering/Vertex.hpp"

namespace crimild {

    /**
       \brief Construct a Box
     */
    class BoxPrimitive : public Primitive {
    public:
        struct Params {
            Primitive::Type type = Primitive::Type::TRIANGLES;
            VertexLayout layout = VertexP3N3TC2::getLayout();
            Vector3f size = Vector3::Constants::ONE;

            // When set to `true`, faces will be inverted generating a hollow box instead
            // This is useful, for example, to quickly build a room.
            Bool invertFaces = false;
        };

    public:
        /**
           \brief Construct a box with positions, normals and texture coordinates of size 1, 1, 1
         */
        BoxPrimitive( void ) noexcept;

        /**
           \brief Construct a box with user defined layout and size
         */
        explicit BoxPrimitive( const Params &params ) noexcept;
        virtual ~BoxPrimitive( void ) = default;
    };

    using BoxPrimitivePtr = SharedPointer< BoxPrimitive >;

}

#endif
