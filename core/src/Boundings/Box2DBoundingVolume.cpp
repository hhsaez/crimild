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

#include "Box2DBoundingVolume.hpp"

#include "Debug/DebugRenderHelper.hpp"
#include "Mathematics/Intersection.hpp"
#include "Mathematics/Point3_constants.hpp"
#include "Mathematics/Vector3Ops.hpp"
#include "Mathematics/Vector3_constants.hpp"
#include "Mathematics/max.hpp"

using namespace crimild;

Box2DBoundingVolume::Box2DBoundingVolume( void )
    : _sphere { Point3::Constants::ZERO, 1.0f }
{
}

Box2DBoundingVolume::Box2DBoundingVolume( crimild::Real32 halfWidth, crimild::Real32 halfHeight )
{
    setRAxis( halfWidth * Vector3::Constants::UNIT_X );
    setSAxis( halfHeight * Vector3::Constants::UNIT_Y );
    setTAxis( Vector3::Constants::UNIT_Z );

    _sphere = Sphere {
        Point3::Constants::ZERO,
        crimild::max( halfWidth, halfHeight ),
    };
}

Box2DBoundingVolume::~Box2DBoundingVolume( void )
{
}

SharedPointer< BoundingVolume > Box2DBoundingVolume::clone( void ) const
{
    auto bb = crimild::alloc< Box2DBoundingVolume >();
    bb->_sphere = _sphere;
    bb->setMin( getMin() );
    bb->setMax( getMin() );
    return bb;
}

void Box2DBoundingVolume::computeFrom( const BoundingVolume *volume )
{
    //computeFrom( volume->getCenter() + volume->getMin(), volume->getCenter() + volume->getMax() );
}

void Box2DBoundingVolume::computeFrom( const BoundingVolume *volume, const Transformation &transformation )
{
    /*
    Vector3f c, r, s, t;

    transformation.applyToPoint( volume->getCenter(), c );
    transformation.applyToVector( volume->getRAxis(), r );
    transformation.applyToVector( volume->getSAxis(), s );
    transformation.applyToVector( volume->getTAxis(), t );

    _sphere.setCenter( c );
    _sphere.setRadius( volume->getRadius() * transformation.getScale() );

    setRAxis( r );
    setSAxis( s );
    setTAxis( t );

    Vector3f p0, p1;
    transformation.applyToPoint( volume->getCenter() + volume->getMin(), p0 );
    transformation.applyToPoint( volume->getCenter() + volume->getMax(), p1 );

    Vector3f min( Numericf::min( p0[ 0 ], p1[ 0 ] ), Numericf::min( p0[ 1 ], p1[ 1 ] ), Numericf::min( p0[ 2 ], p1[ 2 ] ) );
    Vector3f max( Numericf::max( p0[ 0 ], p1[ 0 ] ), Numericf::max( p0[ 1 ], p1[ 1 ] ), Numericf::max( p0[ 2 ], p1[ 2 ] ) );

    computeFrom( min, max );
    */
}

void Box2DBoundingVolume::computeFrom( const Point3 *positions, unsigned int positionCount )
{
    // TODO
}

void Box2DBoundingVolume::computeFrom( const VertexBuffer *vbo )
{
    // TODO
}

void Box2DBoundingVolume::computeFrom( const Point3 &min, const Point3 &max )
{
    // TODO
}

void Box2DBoundingVolume::expandToContain( const Point3 &p )
{
    // TODO
}

void Box2DBoundingVolume::expandToContain( const Point3 *positions, unsigned int positionCount )
{
    // TODO
}

void Box2DBoundingVolume::expandToContain( const VertexBuffer *vbo )
{
    // TODO
}

void Box2DBoundingVolume::expandToContain( const BoundingVolume *input )
{
    // TODO
}

int Box2DBoundingVolume::whichSide( const Plane3 &plane ) const
{
    // TODO
    //return _sphere.whichSide( plane );
    return 0;
}

bool Box2DBoundingVolume::contains( const Point3 &point ) const
{
    /*
    float centerDiffSqr = ( _sphere.getCenter() - point ).getSquaredMagnitude();
    float radiusSqr = _sphere.getRadius() * _sphere.getRadius();
    return ( centerDiffSqr < radiusSqr );
    */
    return false;
}

bool Box2DBoundingVolume::testIntersection( const Ray3 &ray ) const
{
#if 0
    if ( !Intersection::test( _sphere, ray ) ) {
        return false;
    }

    const auto &C = getCenter();
    const auto &R = getRAxis();
    const auto &S = getSAxis();
    const auto &N = getTAxis();

    /*
	auto plane = Plane3( N, C );

	auto t = Intersection::find( plane, ray );
	if ( t < 0.0f ) {
		return false;
	}

	auto P = ray.getPointAt( t );

	auto test = []( const Vector3f &P, const Vector3f &A, const Vector3f &B, const Vector3f &C ) -> bool {
		auto sameSide = []( const Vector3f &p1, const Vector3f &p2, const Vector3f &a, const Vector3f &b ) -> bool
		{
			auto cp1 = ( b - a ) ^ ( p1 - a );
			auto cp2 = ( b - a ) ^ ( p2 - a );
			return ( cp1 * cp2 ) >= 0;
		};

		return sameSide( P, A, B, C ) && sameSide( P, B, A, C ) && sameSide( P, C, A, B );
	};

	auto x = C - R + S;
	auto y = C + R - S;
	auto z = C - R - S;
	auto w = C + R + S;

    return test( P, x, y, z );// || test( P, x, z, w );
	*/

    const auto &RO = ray.getOrigin();
    const auto &RD = ray.getDirection();

    if ( Numericf::isZero( N * RD ) ) {
        // ray is parallel to box
        return false;
    }

    auto P0 = C;
    auto S1 = R;
    auto S2 = S;

    auto t = ( ( P0 - RO ) * N ) / ( N * RD );
    auto P = ray.getPointAt( t );

    auto P0P = P - P0;

    auto Q1 = P0P.project( S1 );
    auto Q2 = P0P.project( S2 );

    auto dS1 = S1.getMagnitude();
    auto dS2 = S2.getMagnitude();
    auto dQ1 = Q1.getMagnitude();
    auto dQ2 = Q2.getMagnitude();

    return dQ1 >= 0.0f && dQ1 <= dS1 && dQ2 >= 0.0f && dQ2 <= dS2;
#endif
    return false;
}

bool Box2DBoundingVolume::testIntersection( const BoundingVolume *other ) const
{
    return other->testIntersection( _sphere );
}

bool Box2DBoundingVolume::testIntersection( const Sphere &sphere ) const
{
    //return Intersection::test( _sphere, sphere );
    return false;
}

bool Box2DBoundingVolume::testIntersection( const Plane3 &plane ) const
{
    return whichSide( plane ) == 0;
}

void Box2DBoundingVolume::resolveIntersection( const BoundingVolume *other, Transformation &result ) const
{
}

void Box2DBoundingVolume::resolveIntersection( const Sphere &other, Transformation &result ) const
{
}

void Box2DBoundingVolume::resolveIntersection( const Plane3 &plane, Transformation &result ) const
{
    // TODO
}

void Box2DBoundingVolume::renderDebugInfo( Renderer *renderer, Camera *camera )
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
        C + getRadius() * T,
    };

    DebugRenderHelper::renderLines(
        renderer,
        camera,
        axes,
        6,
        RGBAColorf( 0.0f, 0.0f, 1.0f, 1.0f ) );

    auto x = C - R + S;
    auto y = C - R - S;
    auto z = C + R - S;
    auto w = C + R + S;

    Vector3f box[] = {
        x,
        y,
        y,
        z,
        z,
        w,
        w,
        x,
    };

    DebugRenderHelper::renderLines(
        renderer,
        camera,
        box,
        8,
        RGBAColorf( 0.0f, 1.0f, 1.0f, 1.0f ) );
    */
}
