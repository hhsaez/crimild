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

#ifndef CRIMILD_CORE_BOUNDINGS_BOUNDING_VOLUME_
#define CRIMILD_CORE_BOUNDINGS_BOUNDING_VOLUME_

#include "Foundation/Macros.hpp"
#include "Foundation/SharedObject.hpp"
#include "Mathematics/Plane.hpp"
#include "Mathematics/Ray3.hpp"
#include "Mathematics/Sphere.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/Vector3.hpp"

namespace crimild {

    class Renderer;
    class Camera;
    class VertexBuffer;

    class BoundingVolume : public SharedObject {
    protected:
        BoundingVolume( void );

    public:
        virtual ~BoundingVolume( void );

        virtual const Point3 &getCenter( void ) const = 0;
        virtual Real getRadius( void ) const = 0;

        virtual SharedPointer< BoundingVolume > clone( void ) const { return nullptr; }

        const Vector3f &getMin( void ) const { return _min; }
        const Vector3f &getMax( void ) const { return _max; }

    protected:
        void setMin( const Vector3f &min ) { _min = min; }
        void setMax( const Vector3f &max ) { _max = max; }

    private:
        Vector3f _min;
        Vector3f _max;

    public:
        void setRAxis( const Vector3f &r ) { _r = r; }
        const Vector3f &getRAxis( void ) const { return _r; }

        void setSAxis( const Vector3f &s ) { _s = s; }
        const Vector3f &getSAxis( void ) const { return _s; }

        void setTAxis( const Vector3f &t ) { _t = t; }
        const Vector3f &getTAxis( void ) const { return _t; }

    private:
        Vector3f _r;
        Vector3f _s;
        Vector3f _t;

    public:
        virtual void computeFrom( const BoundingVolume *volume ) = 0;
        virtual void computeFrom( const BoundingVolume *volume, const Transformation &transform ) = 0;
        virtual void computeFrom( const Vector3f *positions, unsigned int positionCount ) = 0;
        virtual void computeFrom( const VertexBuffer *vbo ) = 0;
        virtual void computeFrom( const Vector3f &min, const Vector3f &max ) = 0;

    public:
        virtual void expandToContain( const Point3 &point ) = 0;
        virtual void expandToContain( const Vector3f *positions, unsigned int positionCount ) = 0;
        virtual void expandToContain( const VertexBuffer *vbo ) = 0;
        virtual void expandToContain( const BoundingVolume *other ) = 0;

    public:
        virtual int whichSide( const Plane3 &plane ) const = 0;
        virtual bool contains( const Vector3 &point ) const = 0;

    public:
        virtual bool testIntersection( const Ray3 &ray ) const = 0;
        virtual bool testIntersection( const BoundingVolume *other ) const = 0;
        virtual bool testIntersection( const Sphere &sphere ) const = 0;
        virtual bool testIntersection( const Plane3 &plane ) const = 0;

        virtual void resolveIntersection( const BoundingVolume *other, Transformation &result ) const = 0;
        virtual void resolveIntersection( const Sphere &sphere, Transformation &result ) const = 0;
        virtual void resolveIntersection( const Plane3 &plane, Transformation &result ) const = 0;

        /*
		  \name Debug
		 */
        //@{

    public:
        virtual void renderDebugInfo( Renderer *, Camera * );

        //@}
    };

}

#endif
