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

#include "SphereBoundingVolume.hpp"

#include "Mathematics/Intersection.hpp"

using namespace crimild;

SphereBoundingVolume::SphereBoundingVolume( void )
    : _sphere( Point3( 0.0f, 0.0f, 0.0f ), 1.0f )
{
}

SphereBoundingVolume::SphereBoundingVolume( const Point3 &center, float radius )
    : _sphere( center, radius )
{
}

SphereBoundingVolume::SphereBoundingVolume( const Sphere &sphere )
    : _sphere( sphere )
{
}

SphereBoundingVolume::~SphereBoundingVolume( void )
{
}

void SphereBoundingVolume::computeFrom( const BoundingVolume *volume )
{
    //computeFrom( volume->getCenter() + volume->getMin(), volume->getCenter() + volume->getMax() );
}

void SphereBoundingVolume::computeFrom( const BoundingVolume *volume, const Transformation &transformation )
{
    /*
    Vector3f newCenter;
    transformation.applyToPoint( volume->getCenter(), newCenter );
    _sphere.setCenter( newCenter );
    _sphere.setRadius( volume->getRadius() * transformation.getScale() );

    setMin( -getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
    setMax( +getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
    */
}

void SphereBoundingVolume::computeFrom( const Vector3f *positions, unsigned int positionCount )
{
    /*
    if ( positionCount == 0 || positions == NULL ) {
        _sphere.setCenter( Vector3f( 0.0f, 0.0f, 0.0f ) );
        _sphere.setRadius( 1.0f );
        return;
    }

    Vector3f max = positions[ 0 ];
    Vector3f min = positions[ 0 ];

    for ( unsigned int i = 1; i < positionCount; i++ ) {
        const Vector3f &pos = positions[ i ];

        if ( pos[ 0 ] > max[ 0 ] )
            max[ 0 ] = pos[ 0 ];
        if ( pos[ 1 ] > max[ 1 ] )
            max[ 1 ] = pos[ 1 ];
        if ( pos[ 2 ] > max[ 2 ] )
            max[ 2 ] = pos[ 2 ];

        if ( pos[ 0 ] < min[ 0 ] )
            min[ 0 ] = pos[ 0 ];
        if ( pos[ 1 ] < min[ 1 ] )
            min[ 1 ] = pos[ 1 ];
        if ( pos[ 2 ] < min[ 2 ] )
            min[ 2 ] = pos[ 2 ];
    }

    _sphere.setCenter( 0.5f * ( max + min ) );
    _sphere.setRadius( Numericf::max( 0.1f, ( max - _sphere.getCenter() ).getMagnitude() ) );

    setMin( -getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
    setMax( +getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
    */
}

void SphereBoundingVolume::computeFrom( const VertexBuffer *vbo )
{
    assert( false );
    /*
	if ( vbo->getVertexCount() == 0 || !vbo->getVertexFormat().hasPositions() ) {
		_sphere.setCenter( Vector3f( 0.0f, 0.0f, 0.0f ) );
		_sphere.setRadius( 0.5f );
		return;
	}

	Vector3f max = vbo->getPositionAt( 0 );
	Vector3f min = vbo->getPositionAt( 0 );
	for ( unsigned int i = 1; i < vbo->getVertexCount(); i++ ) {
		Vector3f pos = vbo->getPositionAt( i );

		if ( pos[ 0 ] > max[ 0 ] ) max[ 0 ] = pos[ 0 ];
		if ( pos[ 1 ] > max[ 1 ] ) max[ 1 ] = pos[ 1 ];
		if ( pos[ 2 ] > max[ 2 ] ) max[ 2 ] = pos[ 2 ];

		if ( pos[ 0 ] < min[ 0 ] ) min[ 0 ] = pos[ 0 ];
		if ( pos[ 1 ] < min[ 1 ] ) min[ 1 ] = pos[ 1 ];
		if ( pos[ 2 ] < min[ 2 ] ) min[ 2 ] = pos[ 2 ];
	}

	Vector3f center = 0.5f * ( max + min );

	float radius = 0.0f;
	for ( unsigned int i = 0; i < vbo->getVertexCount(); i++ ) {
		Vector3f pos = vbo->getPositionAt( i );
		float mag2 = ( pos - center ).getSquaredMagnitude();
		if ( mag2 > radius * radius ) {
			radius = sqrt( mag2 );
		}
	}

	_sphere.setCenter( center );
	_sphere.setRadius( radius );

	setMin( -getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
	setMax( +getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
    */
}

void SphereBoundingVolume::computeFrom( const Vector3f &min, const Vector3f &max )
{
    // TODO
}

void SphereBoundingVolume::expandToContain( const Point3 &point )
{
    //    _sphere.expandToContain( Sphere( point, 0.0f ) );

    setMin( -getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
    setMax( +getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
}

void SphereBoundingVolume::expandToContain( const Vector3f *positions, unsigned int positionCount )
{
    /*
    if ( positionCount == 0 || positions == NULL ) {
        _sphere.setCenter( Vector3f( 0.0f, 0.0f, 0.0f ) );
        _sphere.setRadius( 1.0f );
        return;
    }

    Vector3f max = positions[ 0 ];
    Vector3f min = positions[ 0 ];

    for ( unsigned int i = 1; i < positionCount; i++ ) {
        const Vector3f &pos = positions[ i ];

        if ( pos[ 0 ] > max[ 0 ] )
            max[ 0 ] = pos[ 0 ];
        if ( pos[ 1 ] > max[ 1 ] )
            max[ 1 ] = pos[ 1 ];
        if ( pos[ 2 ] > max[ 2 ] )
            max[ 2 ] = pos[ 2 ];

        if ( pos[ 0 ] < min[ 0 ] )
            min[ 0 ] = pos[ 0 ];
        if ( pos[ 1 ] < min[ 1 ] )
            min[ 1 ] = pos[ 1 ];
        if ( pos[ 2 ] < min[ 2 ] )
            min[ 2 ] = pos[ 2 ];
    }

    expandToContain( max );
    expandToContain( min );
    */
}

void SphereBoundingVolume::expandToContain( const VertexBuffer *vbo )
{
    assert( false );
    /*
	if ( vbo->getVertexCount() == 0 || !vbo->getVertexFormat().hasPositions() ) {
		_sphere.setCenter( Vector3f( 0.0f, 0.0f, 0.0f ) );
		_sphere.setRadius( 1.0f );
		return;
	}

	Vector3f max = vbo->getPositionAt( 0 );
	Vector3f min = vbo->getPositionAt( 0 );

	for ( unsigned int i = 1; i < vbo->getVertexCount(); i++ ) {
		Vector3f pos = vbo->getPositionAt( i );

		if ( pos[ 0 ] > max[ 0 ] ) max[ 0 ] = pos[ 0 ];
		if ( pos[ 1 ] > max[ 1 ] ) max[ 1 ] = pos[ 1 ];
		if ( pos[ 2 ] > max[ 2 ] ) max[ 2 ] = pos[ 2 ];

		if ( pos[ 0 ] < min[ 0 ] ) min[ 0 ] = pos[ 0 ];
		if ( pos[ 1 ] < min[ 1 ] ) min[ 1 ] = pos[ 1 ];
		if ( pos[ 2 ] < min[ 2 ] ) min[ 2 ] = pos[ 2 ];
	}

	expandToContain( max );
	expandToContain( min );
    */
}

void SphereBoundingVolume::expandToContain( const BoundingVolume *input )
{
    /*
    _sphere.expandToContain( Sphere( input->getCenter(), input->getRadius() ) );

    setMin( -getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
    setMax( +getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
    */
}

int SphereBoundingVolume::whichSide( const Plane3 &plane ) const
{
    //return _sphere.whichSide( plane );
    return false;
}

bool SphereBoundingVolume::contains( const Vector3f &point ) const
{
    /*
    float centerDiffSqr = ( _sphere.getCenter() - point ).getSquaredMagnitude();
    float radiusSqr = _sphere.getRadius() * _sphere.getRadius();
    return ( centerDiffSqr < radiusSqr );
    */
    return false;
}

bool SphereBoundingVolume::testIntersection( const Ray3 &ray ) const
{
    //return Intersection::test( _sphere, ray );
    return false;
}

bool SphereBoundingVolume::testIntersection( const BoundingVolume *other ) const
{
    return other->testIntersection( _sphere );
}

bool SphereBoundingVolume::testIntersection( const Sphere &sphere ) const
{
    //return Intersection::test( _sphere, sphere );
    return false;
}

bool SphereBoundingVolume::testIntersection( const Plane3 &plane ) const
{
    //return whichSide( plane ) == 0;
    return false;
}

void SphereBoundingVolume::resolveIntersection( const BoundingVolume *other, Transformation &result ) const
{
    other->resolveIntersection( _sphere, result );
}

void SphereBoundingVolume::resolveIntersection( const Sphere &other, Transformation &result ) const
{
    /*
    Vector3f direction = other.getCenter() - _sphere.getCenter();
    float d = direction.getMagnitude();
    float diff = ( _sphere.getRadius() + other.getRadius() ) - d;
    result.setTranslate( direction.normalize() * diff );
    */
}

void SphereBoundingVolume::resolveIntersection( const Plane3 &plane, Transformation &result ) const
{
}
