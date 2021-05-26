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

#ifndef CRIMILD_PRIMITIVES_PARAMETRIC_SPHERE_
#define CRIMILD_PRIMITIVES_PARAMETRIC_SPHERE_

#include "ParametricPrimitive.hpp"
#include "Rendering/Vertex.hpp"

namespace crimild {

    /**
        Sphere parametrization
        x = r * sin(u) * cos(v)
        y = r * cos(u)
        z = r * -sin(u) * cos(v)
        for 0 <= u <= PI, 0 <= v <= 2 * PI
     */
    class ParametricSpherePrimitive : public ParametricPrimitive {
    public:
        struct Params {
            Primitive::Type type = Primitive::Type::TRIANGLES;
            VertexLayout layout = VertexP3N3::getLayout();
            Real32 radius = 1.0f;
            Vector2i divisions = Vector2i { 20, 20 };
            ColorMode colorMode;
        };

    public:
        ParametricSpherePrimitive( void ) noexcept;
        explicit ParametricSpherePrimitive( const Params &params ) noexcept;
        virtual ~ParametricSpherePrimitive( void ) = default;

    protected:
        virtual Vector3f evaluate( const Vector2f &domain ) const override;

    private:
        float _radius;
    };

    using ParametricSpherePrimitivePtr = SharedPointer< ParametricSpherePrimitive >;

}

#endif
