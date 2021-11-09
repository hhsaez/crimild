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

#ifndef CRIMILD_CORE_VISITORS_INTERSECT_WORLD_
#define CRIMILD_CORE_VISITORS_INTERSECT_WORLD_

#include "Foundation/Containers/Array.hpp"
#include "Mathematics/Normal3Ops.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Ray3.hpp"
#include "Mathematics/Vector3.hpp"
#include "Mathematics/dot.hpp"
#include "Visitors/NodeVisitor.hpp"

namespace crimild {

    class Geometry;
    class Primitive;

    /**
     * Collects geometries that intersect with a given ray
     * 
     * \todo Test intersection with groups to speed things up
     */
    class IntersectWorld : public NodeVisitor {
    public:
        struct Result {
            Geometry *geometry = nullptr;
            Real t = numbers::POSITIVE_INFINITY;
            Point3 point;
            Normal3 normal;
            Bool frontFace;

            inline void setFaceNormal( const Ray3 &R, const Normal3 &N ) noexcept
            {
                frontFace = dot( direction( R ), N ) < 0;
                normal = frontFace ? N : -N;
            }
        };

        using Results = Array< Result >;

    public:
        IntersectWorld( const Ray3 &ray, Results &results ) noexcept
            : m_ray( ray ),
              m_results( results )
        {
            // no-op
        }

        ~IntersectWorld( void ) = default;

        virtual void traverse( Node *node ) noexcept override;
        virtual void visitGroup( Group *group ) noexcept override;
        virtual void visitGeometry( Geometry *geometry ) noexcept override;
        virtual void visitCSGNode( CSGNode *csg ) noexcept override;

    private:
        void intersect( Geometry *geometry, Primitive *primitive, Bool isInVolume ) noexcept;

    private:
        Ray3 m_ray;
        Results &m_results;
    };

}

#endif
