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

#include "PlaneBoundingVolume.hpp"

#include "Mathematics/Intersection.hpp"

using namespace crimild;

PlaneBoundingVolume::PlaneBoundingVolume( void )
    : _center { 0.0f, 0.0f, 0.0f },
      _plane { Normal3 { 0.0f, 1.0f, 0.0f }, 0.0f }
{
}

PlaneBoundingVolume::PlaneBoundingVolume( const Plane3 &plane )
    : _center { 0.0f, 0.0f, 0.0f },
      _plane( plane )
{
}

PlaneBoundingVolume::~PlaneBoundingVolume( void )
{
}

void PlaneBoundingVolume::computeFrom( const BoundingVolume *volume )
{
}

void PlaneBoundingVolume::computeFrom( const BoundingVolume *volume, const Transformation &transformation )
{
}

void PlaneBoundingVolume::computeFrom( const Point3 *positions, unsigned int positionCount )
{
}

void PlaneBoundingVolume::computeFrom( const VertexBuffer *vbo )
{
}

void PlaneBoundingVolume::computeFrom( const Point3 &min, const Point3 &max )
{
}

void PlaneBoundingVolume::expandToContain( const Point3 &point )
{
}

void PlaneBoundingVolume::expandToContain( const Point3 *positions, unsigned int positionCount )
{
}

void PlaneBoundingVolume::expandToContain( const VertexBuffer *vbo )
{
}

void PlaneBoundingVolume::expandToContain( const BoundingVolume *input )
{
}

int PlaneBoundingVolume::whichSide( const Plane3 &plane ) const
{
    return 0;
}

bool PlaneBoundingVolume::contains( const Point3 &point ) const
{
    return false;
}

bool PlaneBoundingVolume::testIntersection( const Ray3 &ray ) const
{
    //return Intersection::test( _plane, ray );
    return false;
}

bool PlaneBoundingVolume::testIntersection( const BoundingVolume *other ) const
{
    return other->testIntersection( _plane );
}

bool PlaneBoundingVolume::testIntersection( const Sphere &sphere ) const
{
    //return sphere.whichSide( _plane ) == 0;
    return false;
}

bool PlaneBoundingVolume::testIntersection( const Plane3 &plane ) const
{
    return 0;
}

void PlaneBoundingVolume::resolveIntersection( const BoundingVolume *other, Transformation &result ) const
{
    other->resolveIntersection( _plane, result );
}

void PlaneBoundingVolume::resolveIntersection( const Sphere &sphere, Transformation &result ) const
{
    /*
    float d = Distance::compute( _plane, sphere.getCenter() );
    if ( d < sphere.getRadius() ) {
        float diff = sphere.getRadius() - d;
        result.setTranslate( diff * _plane.getNormal() );
    } else {
        result.setTranslate( 0.0f, 0.0f, 0.0f );
    }
    */
}

void PlaneBoundingVolume::resolveIntersection( const Plane3 &plane, Transformation &result ) const
{
}
