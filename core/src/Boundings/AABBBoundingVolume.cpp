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

#include "Mathematics/Intersection.hpp"

using namespace crimild;

AABBBoundingVolume::AABBBoundingVolume( void )
	: _sphere( Vector3f( 0.0f, 0.0f, 0.0f ), 1.0f )
{
	setMin( -Numericf::COS_45 * getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
	setMax( +Numericf::COS_45 * getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
}

AABBBoundingVolume::AABBBoundingVolume( const Vector3f &center, float radius )
	: _sphere( center, radius )
{
	setMin( -Numericf::COS_45 * getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
	setMax( +Numericf::COS_45 * getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
}

AABBBoundingVolume::AABBBoundingVolume( const Sphere3f &sphere )
	: _sphere( sphere )
{
	setMin( -Numericf::COS_45 * getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
	setMax( +Numericf::COS_45 * getRadius() * Vector3f( 1.0f, 1.0f, 1.0f ) );
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
	computeFrom( volume->getCenter() + volume->getMin(), volume->getCenter() + volume->getMax() );
}

void AABBBoundingVolume::computeFrom( const BoundingVolume *volume, const Transformation &transformation )
{
	Vector3f p0, p1;
	transformation.applyToPoint( volume->getCenter() + volume->getMin(), p0 );
	transformation.applyToPoint( volume->getCenter() + volume->getMax(), p1 );

	Vector3f min( Numericf::min( p0[ 0 ], p1[ 0 ] ), Numericf::min( p0[ 1 ], p1[ 1 ] ), Numericf::min( p0[ 2 ], p1[ 2 ] ) );
	Vector3f max( Numericf::max( p0[ 0 ], p1[ 0 ] ), Numericf::max( p0[ 1 ], p1[ 1 ] ), Numericf::max( p0[ 2 ], p1[ 2 ] ) );

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

		if ( pos[ 0 ] > max[ 0 ] ) max[ 0 ] = pos[ 0 ];
		if ( pos[ 1 ] > max[ 1 ] ) max[ 1 ] = pos[ 1 ];
		if ( pos[ 2 ] > max[ 2 ] ) max[ 2 ] = pos[ 2 ];

		if ( pos[ 0 ] < min[ 0 ] ) min[ 0 ] = pos[ 0 ];
		if ( pos[ 1 ] < min[ 1 ] ) min[ 1 ] = pos[ 1 ];
		if ( pos[ 2 ] < min[ 2 ] ) min[ 2 ] = pos[ 2 ];
	}

	computeFrom( min, max );
}

void AABBBoundingVolume::computeFrom( const VertexBufferObject *vbo )
{
	if ( vbo->getVertexCount() == 0 || !vbo->getVertexFormat().hasPositions() ) {
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

	computeFrom( min, max );
}

void AABBBoundingVolume::computeFrom( const Vector3f &min, const Vector3f &max ) 
{
	_sphere.setCenter( 0.5f * ( max + min ) );
	_sphere.setRadius( Numericf::max( 0.01f, ( max - _sphere.getCenter() ).getMagnitude() ) );

	setMin( min - getCenter() );
	setMax( max - getCenter() );
}

void AABBBoundingVolume::expandToContain( const Vector3f &p )
{
	Vector3f diff = p - getCenter();
	Vector3f min( Numericf::min( diff[ 0 ], getMin()[ 0 ] ), Numericf::min( diff[ 1 ], getMin()[ 1 ] ), Numericf::min( diff[ 2 ], getMin()[ 2 ] ) );
	Vector3f max( Numericf::max( diff[ 0 ], getMin()[ 0 ] ), Numericf::max( diff[ 1 ], getMin()[ 1 ] ), Numericf::max( diff[ 2 ], getMin()[ 2 ] ) );

	computeFrom( min, max );
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

		if ( pos[ 0 ] > max[ 0 ] ) max[ 0 ] = pos[ 0 ];
		if ( pos[ 1 ] > max[ 1 ] ) max[ 1 ] = pos[ 1 ];
		if ( pos[ 2 ] > max[ 2 ] ) max[ 2 ] = pos[ 2 ];

		if ( pos[ 0 ] < min[ 0 ] ) min[ 0 ] = pos[ 0 ];
		if ( pos[ 1 ] < min[ 1 ] ) min[ 1 ] = pos[ 1 ];
		if ( pos[ 2 ] < min[ 2 ] ) min[ 2 ] = pos[ 2 ];
	}

	expandToContain( max );
	expandToContain( min );
}

void AABBBoundingVolume::expandToContain( const VertexBufferObject *vbo )
{
	if ( vbo->getVertexCount() == 0 || !vbo->getVertexFormat().hasPositions() ) {
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
}

void AABBBoundingVolume::expandToContain( const BoundingVolume *input )
{
	expandToContain( input->getCenter() + input->getMin() );
	expandToContain( input->getCenter() + input->getMax() );
}

int AABBBoundingVolume::whichSide( const Plane3f &plane ) const
{
	return _sphere.whichSide( plane );
}

bool AABBBoundingVolume::contains( const Vector3f &point ) const
{
	float centerDiffSqr = ( _sphere.getCenter() - point ).getSquaredMagnitude();
	float radiusSqr = _sphere.getRadius() * _sphere.getRadius();
	return ( centerDiffSqr < radiusSqr );
}

bool AABBBoundingVolume::testIntersection( const Ray3f &ray ) const
{
	if ( !Intersection::test( _sphere, ray ) ) {
		return false;
	}

	return Intersection::test( getCenter() + getMin(), getCenter() + getMax(), ray );
}

bool AABBBoundingVolume::testIntersection( const BoundingVolume *other ) const
{
	return other->testIntersection( _sphere );
}

bool AABBBoundingVolume::testIntersection( const Sphere3f &sphere ) const
{
	return Intersection::test( _sphere, sphere );
}

bool AABBBoundingVolume::testIntersection( const Plane3f &plane ) const
{
	return whichSide( plane ) == 0;
}

void AABBBoundingVolume::resolveIntersection( const BoundingVolume *other, Transformation &result ) const
{
	other->resolveIntersection( _sphere, result );
}

void AABBBoundingVolume::resolveIntersection( const Sphere3f &other, Transformation &result ) const
{
	Vector3f direction = other.getCenter() - _sphere.getCenter();
	float d = direction.getMagnitude();
	float diff = ( _sphere.getRadius() + other.getRadius() ) - d;
	result.setTranslate( direction.normalize() * diff );
}

void AABBBoundingVolume::resolveIntersection( const Plane3f &plane, Transformation &result ) const
{

}

