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

#include "AABBBoundingVolume.hpp"

#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Vector3Ops.hpp"
#include "Mathematics/Vector3_constants.hpp"
#include "Mathematics/distance.hpp"
#include "Mathematics/intersect.hpp"
#include "Mathematics/length.hpp"
#include "Mathematics/max.hpp"
#include "Mathematics/min.hpp"
#include "Mathematics/swizzle.hpp"
#include "Mathematics/whichSide.hpp"
#include "Rendering/VertexBuffer.hpp"

using namespace crimild;

AABBBoundingVolume::AABBBoundingVolume( void )
    : AABBBoundingVolume( Sphere {} )
{
    // no-op
}

AABBBoundingVolume::AABBBoundingVolume( const Point3 &center, float radius )
    : AABBBoundingVolume( Sphere { center, radius } )
{
    // no-op
}

AABBBoundingVolume::AABBBoundingVolume( const Sphere &sphere )
    : _sphere( sphere )
{
    const auto v = Vector3 {
        numbers::SQRT_3_DIV_3,
        numbers::SQRT_3_DIV_3,
        numbers::SQRT_3_DIV_3,
    };

    setMin( getCenter() - getRadius() * v );
    setMax( getCenter() + getRadius() * v );
}

AABBBoundingVolume::~AABBBoundingVolume( void )
{
    // no-op
}

SharedPointer< BoundingVolume > AABBBoundingVolume::clone( void ) const
{
    auto bb = crimild::alloc< AABBBoundingVolume >();
    bb->_sphere = _sphere;
    bb->setMin( getMin() );
    bb->setMax( getMax() );
    return bb;
}

void AABBBoundingVolume::computeFrom( const BoundingVolume *volume )
{
    computeFrom( volume->getMin(), volume->getMax() );
}

void AABBBoundingVolume::computeFrom( const BoundingVolume *volume, const Transformation &transformation )
{
    const auto p0 = transformation( volume->getMin() );
    const auto p1 = transformation( volume->getMax() );

    const auto min = crimild::min( p0, p1 );
    const auto max = crimild::max( p0, p1 );

    computeFrom( min, max );
}

void AABBBoundingVolume::computeFrom( const Point3 *positions, unsigned int positionCount )
{
    if ( positionCount == 0 || positions == NULL ) {
        return;
    }

    Point3 max = positions[ 0 ];
    Point3 min = positions[ 0 ];

    for ( unsigned int i = 1; i < positionCount; i++ ) {
        const Point3 &pos = point3( positions[ i ] );
        min = crimild::min( min, pos );
        max = crimild::max( max, pos );
    }

    computeFrom( min, max );
}

void AABBBoundingVolume::computeFrom( const VertexBuffer *vbo )
{
    if ( vbo->getVertexCount() == 0 ) {
        return;
    }

    auto positions = vbo->get( VertexAttribute::Name::POSITION );
    if ( positions == nullptr ) {
        return;
    }

    Point3 max;
    Point3 min;

    positions->each< Point3 >(
        [ & ]( auto &pos, auto i ) {
            if ( i == 0 ) {
                max = pos;
                min = pos;
            } else {
                max = crimild::max( max, pos );
                min = crimild::min( min, pos );
            }
        } );

    computeFrom( min, max );
}

void AABBBoundingVolume::computeFrom( const Point3 &min, const Point3 &max )
{
    const auto center = 0.5 * ( max + min );
    const auto radius = crimild::max( Real( numbers::EPSILON ), distance( point3( max ), point3( center ) ) );
    _sphere = Sphere { center, radius };

    setMin( min );
    setMax( max );
}

void AABBBoundingVolume::expandToContain( const Point3 &p )
{
    const auto min = crimild::min( getMin(), p );
    const auto max = crimild::max( getMax(), p );

    computeFrom( min, max );
}

void AABBBoundingVolume::expandToContain( const Point3 *positions, unsigned int positionCount )
{
    if ( positionCount == 0 || positions == NULL ) {
        return;
    }

    Point3 max = positions[ 0 ];
    Point3 min = positions[ 0 ];

    for ( unsigned int i = 1; i < positionCount; i++ ) {
        const Point3 &pos = positions[ i ];

        max = crimild::max( max, pos );
        min = crimild::min( min, pos );
    }

    expandToContain( max );
    expandToContain( min );
}

void AABBBoundingVolume::expandToContain( const VertexBuffer *vbo )
{
    if ( vbo->getVertexCount() == 0 ) {
        return;
    }

    auto positions = vbo->get( VertexAttribute::Name::POSITION );
    if ( positions == nullptr ) {
        return;
    }

    Point3 max;
    Point3 min;

    positions->each< Vector3f >(
        [ & ]( auto &pos, auto i ) {
            const auto P = point3( pos );
            if ( i == 0 ) {
                max = P;
                min = P;
            } else {
                max = crimild::max( max, P );
                min = crimild::min( min, P );
            }
        } );

    expandToContain( max );
    expandToContain( min );
}

void AABBBoundingVolume::expandToContain( const BoundingVolume *input )
{
    expandToContain( input->getMin() );
    expandToContain( input->getMax() );
}

int AABBBoundingVolume::whichSide( const Plane3 &plane ) const
{
    return crimild::whichSide( plane, _sphere );
}

bool AABBBoundingVolume::contains( const Point3 &point ) const
{
    const auto centerDiffSqr = distanceSquared( getCenter(), point );
    float radiusSqr = radius( _sphere ) * radius( _sphere );
    return ( centerDiffSqr < radiusSqr );
}

bool AABBBoundingVolume::testIntersection( const Ray3 &ray ) const
{
    Real t0, t1;
    return intersect( ray, _sphere, t0, t1 );
}

bool AABBBoundingVolume::testIntersection( const BoundingVolume *other ) const
{
    return other->testIntersection( _sphere );
}

bool AABBBoundingVolume::testIntersection( const Sphere &sphere ) const
{
    //return Intersection::test( _sphere, sphere );
    assert( false );
    return false;
}

bool AABBBoundingVolume::testIntersection( const Plane3 &plane ) const
{
    return whichSide( plane ) == 0;
}

void AABBBoundingVolume::resolveIntersection( const BoundingVolume *other, Transformation &result ) const
{
    other->resolveIntersection( _sphere, result );
}

void AABBBoundingVolume::resolveIntersection( const Sphere &other, Transformation &result ) const
{
    /*
    Vector3f direction = other.getCenter() - _sphere.getCenter();
    float d = direction.getMagnitude();
    float diff = ( _sphere.getRadius() + other.getRadius() ) - d;
    result.setTranslate( direction.normalize() * diff );
    */
    assert( false );
}

void AABBBoundingVolume::resolveIntersection( const Plane3 &plane, Transformation &result ) const
{
}
