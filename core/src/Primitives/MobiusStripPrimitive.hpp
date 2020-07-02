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

#ifndef CRIMILD_PRIMITIVES_MOBIUS_STRIP_
#define CRIMILD_PRIMITIVES_MOBIUS_STRIP_

#include "ParametricPrimitive.hpp"
#include "Rendering/Vertex.hpp"

namespace crimild {

    /**
     */
    class MobiusStripPrimitive : public ParametricPrimitive {
    public:
        struct Params {
            Primitive::Type type = Primitive::Type::TRIANGLES;
            VertexLayout layout = VertexP3N3::getLayout();
            ParametricPrimitive::ColorMode colorMode;
            Real32 scale = 1.0f;
            Vector2i divisions = Vector2i( 40, 20 );
        };

    public:
        MobiusStripPrimitive( const Params &params ) noexcept;
        virtual ~MobiusStripPrimitive( void ) = default;

    protected:
        virtual Vector3f evaluate( const Vector2f &domain ) const;

    private:
        float _scale;
    };

    using MobiusStripPrimitivePtr = SharedPointer< MobiusStripPrimitive >;

}

#endif
