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

#ifndef CRIMILD_PRIMITIVES_PARAMETRIC_
#define CRIMILD_PRIMITIVES_PARAMETRIC_

#include "Primitive.hpp"

#include "Mathematics/Vector.hpp"

namespace crimild {

    class ParametricInterval {
    public:
        Vector2i divisions;
        Vector2f upperBound;
        Vector2f textureCount;
    };

    class ParametricPrimitive : public Primitive {
    public:
        ParametricPrimitive( Primitive::Type type, const VertexLayout &layout ) noexcept;
        virtual ~ParametricPrimitive( void ) = default;

    protected:
        void setInterval( const ParametricInterval &interval );
        void generate( void );
        virtual Vector3f evaluate( const Vector2f &domain ) const = 0;
        virtual bool invertNormal( const Vector2f &domain ) const { return false; }

    private:
        int getVertexCount( void ) const;
        int getLineIndexCount( void ) const;
        int getTriangleIndexCount( void ) const;
        Vector2f computeDomain( float i, float j ) const;
        void generateVertexBuffer( void );
        void generateLineIndexBuffer( void );
        void generateTriangleIndexBuffer( void );

        VertexLayout _layout;
        Vector2f _upperBound;
        Vector2i _slices;
        Vector2i _divisions;
        Vector2i _textureCount;
    };

    using ParametricPrimitivePtr = SharedPointer< ParametricPrimitive >;

}

#endif
