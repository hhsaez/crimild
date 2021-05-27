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
#include "Mathematics/length.hpp"
#include "Mathematics/max.hpp"
#include "Mathematics/min.hpp"
#include "Mathematics/swizzle.hpp"
#include "Mathematics/whichSide.hpp"
#include "Rendering/VertexBuffer.hpp"

using namespace crimild;

AABBBoundingVolume::AABBBoundingVolume( void )
    : _sphere { Point3::Constants::ZERO, Real( 1 ) }
{
    setMin( -Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
    setMax( +Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
}

AABBBoundingVolume::AABBBoundingVolume( const Point3 &center, float radius )
    : _sphere { center, radius }
{
    setMin( -Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
    setMax( +Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
}

AABBBoundingVolume::AABBBoundingVolume( const Sphere &sphere )
    : _sphere( sphere )
{
    setMin( -Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
    setMax( +Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
}

AABBBoundingVolume::~AABBBoundingVolume( void )
{
}

SharedPointer< BoundingVolume > AABBBoundingVolume::clone( void ) const
{
    auto bb = crimild::alloc< AABBBoundingVolume >();
    bb->_sphere = _sphere;
    bb->setMin( getMin() );
    bb->setMax( getMin() );
    return bb;
}

void AABBBoundingVolume::computeFrom( const BoundingVolume *volume )
{
    //computeFrom( volume->getCenter() + volume->getMin(), volume->getCenter() + volume->getMax() );
}

void AABBBoundingVolume::computeFrom( const BoundingVolume *volume, const Transformation &transformation )
{
    const auto p0 = transformation( volume->getCenter() + volume->getMin() );
    const auto p1 = transformation( volume->getCenter() + volume->getMax() );

    const auto min = Vector3 { Numericf::min( p0[ 0 ], p1[ 0 ] ), Numericf::min( p0[ 1 ], p1[ 1 ] ), Numericf::min( p0[ 2 ], p1[ 2 ] ) };
    const auto max = Vector3 { Numericf::max( p0[ 0 ], p1[ 0 ] ), Numericf::max( p0[ 1 ], p1[ 1 ] ), Numericf::max( p0[ 2 ], p1[ 2 ] ) };

    computeFrom( min, max );
}

void AABBBoundingVolume::computeFrom( const Vector3f *positions, unsigned int positionCount )
{
    if ( positionCount == 0 || positions == NULL ) {
        return;
    }

    Vector3f max = positions[ 0 ];
    Vector3f min = positions[ 0 ];

    for ( unsigned int i = 1; i < positionCount; i++ ) {
        const Vector3f &pos = positions[ i ];
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

    Vector3f max;
    Vector3f min;

    positions->each< Vector3f >(
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

void AABBBoundingVolume::computeFrom( const Vector3f &min, const Vector3f &max )
{
    _sphere = Sphere {
        point3( Real( 0.5 ) * ( max + min ) ),
        crimild::max( Real( 0.01 ), length( max - vector3( center( _sphere ) ) ) ),
    };

    setMin( min - vector3( getCenter() ) );
    setMax( max - vector3( getCenter() ) );
}

void AABBBoundingVolume::expandToContain( const Point3 &p )
{
    /*
    auto min = getCenter() + getMin();
    auto max = getCenter() + getMax();

    min = Vector3f( Numericf::min( p[ 0 ], min[ 0 ] ), Numericf::min( p[ 1 ], min[ 1 ] ), Numericf::min( p[ 2 ], min[ 2 ] ) );
    max = Vector3f( Numericf::max( p[ 0 ], max[ 0 ] ), Numericf::max( p[ 1 ], max[ 1 ] ), Numericf::max( p[ 2 ], max[ 2 ] ) );

    computeFrom( min, max );
    */

    assert( false && "TODO" );
}

void AABBBoundingVolume::expandToContain( const Vector3f *positions, unsigned int positionCount )
{
    if ( positionCount == 0 || positions == NULL ) {
        return;
    }

    Vector3f max = positions[ 0 ];
    Vector3f min = positions[ 0 ];

    for ( unsigned int i = 1; i < positionCount; i++ ) {
        const Vector3f &pos = positions[ i ];

        max = crimild::max( max, pos );
        min = crimild::min( min, pos );
    }

    expandToContain( point3( max ) );
    expandToContain( point3( min ) );
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
    expandToContain( input->getCenter() + input->getMin() );
    expandToContain( input->getCenter() + input->getMax() );
}

int AABBBoundingVolume::whichSide( const Plane3 &plane ) const
{
    return crimild::whichSide( plane, _sphere );
}

bool AABBBoundingVolume::contains( const Vector3f &point ) const
{
    const auto centerDiffSqr = distanceSquared( getCenter(), point3( point ) );
    float radiusSqr = radius( _sphere ) * radius( _sphere );
    return ( centerDiffSqr < radiusSqr );
}

bool AABBBoundingVolume::testIntersection( const Ray3 &ray ) const
{
    /*
    if ( !checkIntersection( _sphere, ray ) ) {
        return false;
    }

    return checkIntersection( getCenter() + getMin(), getCenter() + getMax(), ray );
    if ( !Intersection::test( _sphere, ray ) ) {
        return false;
    }

    return Intersection::test( getCenter() + getMin(), getCenter() + getMax(), ray );
    */
    assert( false && "TODO" );
    return false;
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
