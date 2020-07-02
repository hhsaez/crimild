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

#ifndef CRIMILD_PRIMITIVES_CONE_
#define CRIMILD_PRIMITIVES_CONE_

#include "ParametricPrimitive.hpp"
#include "Rendering/Vertex.hpp"

namespace crimild {

    /**
        Cone parametrization
        x = r * ( 1 - v ) * sin(u)
        y = h * ( v - 0.5)
        z = r * ( 1 - v ) * -sin(u)
        for 0 <= u <= 2 * PI, 0 <= v <= 1
     */
    class ConePrimitive : public ParametricPrimitive {
    public:
        struct Params {
            Primitive::Type type = Primitive::Type::TRIANGLES;
            VertexLayout layout = VertexP3N3::getLayout();
            Real32 height = 1.0f;
            Real32 radius = 0.5f;
            Vector2i divisions = Vector2i( 20, 20 );
            ParametricPrimitive::ColorMode colorMode;
        };

    public:
        ConePrimitive( const Params &params ) noexcept;
        virtual ~ConePrimitive( void ) = default;

    protected:
        virtual Vector3f evaluate( const Vector2f &domain ) const;

    private:
        float _height;
        float _radius;
    };

    using ConePrimitivePtr = SharedPointer< ConePrimitive >;

}

#endif
