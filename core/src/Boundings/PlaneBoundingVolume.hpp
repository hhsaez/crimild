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

#ifndef CRIMILD_CORE_BOUNDINGS_BOUNDING_VOLUME_PLANE_
#define CRIMILD_CORE_BOUNDINGS_BOUNDING_VOLUME_PLANE_

#include "BoundingVolume.hpp"

namespace crimild {

    class PlaneBoundingVolume : public BoundingVolume {
    public:
        PlaneBoundingVolume( void );
        explicit PlaneBoundingVolume( const Plane3 &plane );
        virtual ~PlaneBoundingVolume( void );

        virtual const Point3 &getCenter( void ) const override { return _center; }
        virtual float getRadius( void ) const override { return 0.0f; }

    private:
        Point3 _center;
        Plane3 _plane;

    public:
        virtual void computeFrom( const BoundingVolume *volume ) override;
        virtual void computeFrom( const BoundingVolume *volume, const Transformation &transform ) override;
        virtual void computeFrom( const Point3 *positions, unsigned int positionCount ) override;
        virtual void computeFrom( const VertexBuffer *vbo ) override;
        virtual void computeFrom( const Point3 &min, const Point3 &max ) override;

    public:
        virtual void expandToContain( const Point3 &point ) override;
        virtual void expandToContain( const Point3 *positions, unsigned int positionCount ) override;
        virtual void expandToContain( const VertexBuffer *vbo ) override;
        virtual void expandToContain( const BoundingVolume *input ) override;

    public:
        virtual int whichSide( const Plane3 &plane ) const override;
        virtual bool contains( const Point3 &point ) const override;

    public:
        virtual bool testIntersection( const Ray3 &ray ) const override;
        virtual bool testIntersection( const BoundingVolume *input ) const override;
        virtual bool testIntersection( const Sphere &sphere ) const override;
        virtual bool testIntersection( const Plane3 &plane ) const override;

        virtual void resolveIntersection( const BoundingVolume *other, Transformation &result ) const override;
        virtual void resolveIntersection( const Sphere &sphere, Transformation &result ) const override;
        virtual void resolveIntersection( const Plane3 &plane, Transformation &result ) const override;
    };

    using PlaneBoundingVolumePtr = SharedPointer< PlaneBoundingVolume >;

}

#endif
