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

#include "NavigationCell.hpp"

#include "Mathematics/Intersection.hpp"

using namespace crimild;
using namespace crimild::navigation;

NavigationCell::NavigationCell( const Vector3f &v0, const Vector3f &v1, const Vector3f &v2 )
{
    _vertices[ 0 ] = v0;
    _vertices[ 1 ] = v1;
    _vertices[ 2 ] = v2;

    _center = ( _vertices[ 0 ] + _vertices[ 1 ] + _vertices[ 2 ] ) / 3.0f;

    _normal = normalize( cross( ( v2 - v0 ), ( v1 - v0 ) ) );

    _plane = Plane3( Point3( v0 ), Point3( v1 ), Point3( v2 ) );
}

NavigationCell::~NavigationCell( void )
{
}

bool NavigationCell::containsPoint( const Vector3f &p ) const
{
    assert( false );
    /*
    auto sameSide = []( const Vector3f &p1, const Vector3f &p2, const Vector3f &a, const Vector3f &b ) -> bool {
        auto cp1 = ( b - a ) ^ ( p1 - a );
        auto cp2 = ( b - a ) ^ ( p2 - a );
        return ( cp1 * cp2 ) >= 0;
    };

    auto a = _vertices[ 0 ];
    auto b = _vertices[ 1 ];
    auto c = _vertices[ 2 ];
    return sameSide( p, a, b, c ) && sameSide( p, b, a, c ) && sameSide( p, c, a, b ) && Numericf::fabs( Distance::compute( _plane, p ) ) <= 0.1f;
    */
    return false;
}

NavigationCell::ClassificationResult NavigationCell::classifyPath( const LineSegment3 &motionPath, Vector3f &intersectionPoint, NavigationCellEdge **intersectionEdge )
{
    /*
    // Project motion path on the cell's plane
    auto p0 = _plane.project( motionPath.getOrigin() );
    auto p1 = _plane.project( motionPath.getDestination() );

    int interiorCount = 0;

    // Use line segments in each edge to classify points.
    // First, test the end point (p1).
    for ( auto &e : _edges ) {
        // If the end point is to the left of an edge
        // then it should be outside of the cell
        if ( e->getLine().whichSide( p1, _plane.getNormal() ) < 0 ) {
            // Check the start point first
            if ( e->getLine().whichSide( p0, _plane.getNormal() ) >= 0 ) {
                // If the start point is to the other side of the edge,
                // then the path intersects the edge
                intersectionPoint = Intersection::find( LineSegment3f( p0, p1 ), e->getLine() );
                *intersectionEdge = crimild::get_ptr( e );
                return NavigationCell::ClassificationResult::OUTSIDE;
            }
        } else {
            // the point is to the right of (or over) the line
            // keep count and check the next segment
            interiorCount++;
        }
    }

    // If the point is on the right side of all edges, the there's no intersection
    if ( interiorCount == 3 ) {
        return NavigationCell::ClassificationResult::INSIDE;
    }

    */
    return NavigationCell::ClassificationResult::NONE;
}

Vector3f NavigationCell::snapPoint( const Vector3f &point )
{
    /*
    // Create a motion path from the cell's center to the given point
    LineSegment3f path( getCenter(), point );

    NavigationCellEdge *edge = nullptr;
    Vector3f intersectionPoint;
    Vector3f result = point;

    // classify the motion path
    auto classifyResult = classifyPath( path, intersectionPoint, &edge );

    if ( classifyResult == ClassificationResult::OUTSIDE ) {
        // if the motion path is outside, bring the point inside the cell
        // using the intersection point and a small offset
        result = getCenter() + 0.49f * ( intersectionPoint - getCenter() );
    } else if ( classifyResult == ClassificationResult::NONE ) {
        // if there's no intersection at all, use the center of the cell
        // as point (is the cell too small?)
        result = getCenter();
    }
    */

    return getCenter();
}
