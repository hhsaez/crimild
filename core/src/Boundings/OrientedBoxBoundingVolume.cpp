/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#include "OrientedBoxBoundingVolume.hpp"

#include "Debug/DebugRenderHelper.hpp"
#include "Mathematics/Intersection.hpp"
#include "Mathematics/Vector3Ops.hpp"

using namespace crimild;

OrientedBoxBoundingVolume::OrientedBoxBoundingVolume( void )
    : _sphere { Point3::Constants::ZERO, 1.0f }
{
    setMin( -Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
    setMax( +Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
}

OrientedBoxBoundingVolume::OrientedBoxBoundingVolume( const Vector3f &r, const Vector3f &s, const Vector3f &t )
{
    setRAxis( r );
    setSAxis( s );
    setTAxis( t );

    //_sphere.setCenter( Vector3f::ZERO );
    //_sphere.setRadius( Numericf::max( _r.getMagnitude(), Numericf::max( _s.getMagnitude(), _t.getMagnitude() ) ) );
}

OrientedBoxBoundingVolume::OrientedBoxBoundingVolume( const Point3 &center, float radius )
    : _sphere { center, radius }
{
    setMin( -Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
    setMax( +Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
}

OrientedBoxBoundingVolume::OrientedBoxBoundingVolume( const Sphere &sphere )
    : _sphere( sphere )
{
    setMin( -Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
    setMax( +Numericf::COS_45 * getRadius() * Vector3::Constants::ONE );
}

OrientedBoxBoundingVolume::~OrientedBoxBoundingVolume( void )
{
}

SharedPointer< BoundingVolume > OrientedBoxBoundingVolume::clone( void ) const
{
    auto bb = crimild::alloc< OrientedBoxBoundingVolume >();
    bb->_sphere = _sphere;
    bb->setMin( getMin() );
    bb->setMax( getMin() );
    return bb;
}

void OrientedBoxBoundingVolume::computeFrom( const BoundingVolume *volume )
{
    //computeFrom( volume->getCenter() + volume->getMin(), volume->getCenter() + volume->getMax() );
}

void OrientedBoxBoundingVolume::computeFrom( const BoundingVolume *volume, const Transformation &transformation )
{
    /*
    Vector3f c, r, s, t;

    transformation.applyToPoint( volume->getCenter(), c );
    transformation.applyToVector( volume->getRAxis(), r );
    transformation.applyToVector( volume->getSAxis(), s );
    transformation.applyToVector( volume->getTAxis(), t );

    _sphere.setCenter( c );

    setRAxis( r );
    setSAxis( s );
    setTAxis( t );
    */

    /*
	Vector3f p0, p1;
	transformation.applyToPoint( volume->getCenter() + volume->getMin(), p0 );
	transformation.applyToPoint( volume->getCenter() + volume->getMax(), p1 );

	Vector3f min( Numericf::min( p0[ 0 ], p1[ 0 ] ), Numericf::min( p0[ 1 ], p1[ 1 ] ), Numericf::min( p0[ 2 ], p1[ 2 ] ) );
	Vector3f max( Numericf::max( p0[ 0 ], p1[ 0 ] ), Numericf::max( p0[ 1 ], p1[ 1 ] ), Numericf::max( p0[ 2 ], p1[ 2 ] ) );

	computeFrom( min, max );
	*/
}

void OrientedBoxBoundingVolume::computeFrom( const Vector3f *positions, unsigned int positionCount )
{
    /*
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
	*/
}

void OrientedBoxBoundingVolume::computeFrom( const VertexBuffer *vbo )
{
    /*
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
	*/
}

void OrientedBoxBoundingVolume::computeFrom( const Vector3f &min, const Vector3f &max )
{
    /*
	_sphere.setCenter( 0.5f * ( max + min ) );
	_sphere.setRadius( Numericf::max( 0.01f, ( max - _sphere.getCenter() ).getMagnitude() ) );

	setMin( min - getCenter() );
	setMax( max - getCenter() );
	*/
}

void OrientedBoxBoundingVolume::expandToContain( const Point3 &p )
{
    /*
	auto min = getCenter() + getMin();
	auto max = getCenter() + getMax();

	min = Vector3f( Numericf::min( p[ 0 ], min[ 0 ] ), Numericf::min( p[ 1 ], min[ 1 ] ), Numericf::min( p[ 2 ], min[ 2 ] ) );
	max = Vector3f( Numericf::max( p[ 0 ], max[ 0 ] ), Numericf::max( p[ 1 ], max[ 1 ] ), Numericf::max( p[ 2 ], max[ 2 ] ) );

	computeFrom( min, max );
	*/
}

void OrientedBoxBoundingVolume::expandToContain( const Vector3f *positions, unsigned int positionCount )
{
    /*
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
	*/
}

void OrientedBoxBoundingVolume::expandToContain( const VertexBuffer *vbo )
{
    /*
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
	*/
}

void OrientedBoxBoundingVolume::expandToContain( const BoundingVolume *input )
{
    /*
	expandToContain( input->getCenter() + input->getMin() );
	expandToContain( input->getCenter() + input->getMax() );
	*/
}

int OrientedBoxBoundingVolume::whichSide( const Plane3 &plane ) const
{
    //return _sphere.whichSide( plane );
    return 0;
}

bool OrientedBoxBoundingVolume::contains( const Vector3f &point ) const
{
    /*
	float centerDiffSqr = ( _sphere.getCenter() - point ).getSquaredMagnitude();
	float radiusSqr = _sphere.getRadius() * _sphere.getRadius();
	return ( centerDiffSqr < radiusSqr );
	*/
    return false;
}

bool OrientedBoxBoundingVolume::testIntersection( const Ray3 &ray ) const
{
    /*
	if ( !Intersection::test( _sphere, ray ) ) {
		return false;
	}

	return Intersection::test( getCenter() + getMin(), getCenter() + getMax(), ray );
	*/

    return false;
}

bool OrientedBoxBoundingVolume::testIntersection( const BoundingVolume *other ) const
{
    return false; //other->testIntersection( _sphere );
}

bool OrientedBoxBoundingVolume::testIntersection( const Sphere &sphere ) const
{
    return false; //Intersection::test( _sphere, sphere );
}

bool OrientedBoxBoundingVolume::testIntersection( const Plane3 &plane ) const
{
    return false; //whichSide( plane ) == 0;
}

void OrientedBoxBoundingVolume::resolveIntersection( const BoundingVolume *other, Transformation &result ) const
{
    //other->resolveIntersection( _sphere, result );
}

void OrientedBoxBoundingVolume::resolveIntersection( const Sphere &other, Transformation &result ) const
{
    /*
	Vector3f direction = other.getCenter() - _sphere.getCenter();
	float d = direction.getMagnitude();
	float diff = ( _sphere.getRadius() + other.getRadius() ) - d;
	result.setTranslate( direction.normalize() * diff );
	*/
}

void OrientedBoxBoundingVolume::resolveIntersection( const Plane3 &plane, Transformation &result ) const
{
    // TODO
}

void OrientedBoxBoundingVolume::renderDebugInfo( Renderer *renderer, Camera *camera )
{
    /*
    const auto &C = getCenter();
    const auto &R = getRAxis();
    const auto &S = getSAxis();
    const auto &T = getTAxis();

    Vector3f axes[] = {
        C,
        C + R,
        C,
        C + S,
        C,
        C + T,
    };

    DebugRenderHelper::renderLines(
        renderer,
        camera,
        axes,
        6,
        RGBAColorf( 0.0f, 0.0f, 1.0f, 1.0f ) );

    Vector3f box[] = {
        C + R + S + T,
        C + R - S + T,
        C + R - S + T,
        C - R - S + T,
        C - R - S + T,
        C - R + S + T,
        C - R + S + T,
        C + R + S + T,
        C + R + S - T,
        C + R - S - T,
        C + R - S - T,
        C - R - S - T,
        C - R - S - T,
        C - R + S - T,
        C - R + S - T,
        C + R + S - T,
        C + R + S + T,
        C + R + S - T,
        C + R - S + T,
        C + R - S - T,
        C - R - S + T,
        C - R - S - T,
        C - R + S + T,
        C - R + S - T,
    };

    DebugRenderHelper::renderLines(
        renderer,
        camera,
        box,
        24,
        RGBAColorf( 0.0f, 1.0f, 1.0f, 1.0f ) );
    */
}
