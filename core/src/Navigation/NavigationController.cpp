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

#include "NavigationController.hpp"

#include "Foundation/Log.hpp"
#include "Mathematics/Intersection.hpp"
#include "Mathematics/LineSegment3.hpp"
#include "Mathematics/Ray3.hpp"
#include "NavigationMeshContainer.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/Apply.hpp"

#include <list>

using namespace crimild;
using namespace crimild::navigation;

NavigationController::NavigationController( void )
{
}

NavigationController::NavigationController( NavigationMeshPtr const &mesh )
    : _navigationMesh( mesh )
{
}

NavigationController::~NavigationController( void )
{
}

void NavigationController::start( void )
{
    if ( _navigationMesh == nullptr ) {
        // No navigation mesh assigned. Find the first one in the scene
        getNode()->getRootParent()->perform( Apply( [ this ]( Node *node ) {
            auto nav = node->getComponent< NavigationMeshContainer >();
            if ( nav != nullptr ) {
                _navigationMesh = crimild::retain( nav->getNavigationMesh() );
            }
        } ) );
    }
}

Vector3f NavigationController::move( const Vector3f &from, const Vector3f &to )
{
    /*
    if ( _navigationMesh == nullptr ) {
        Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No navigation mesh found" );
        return from;
    }

    NavigationCell *cell = nullptr;
    getNavigationMesh()->foreachCell( [ &cell, to ]( NavigationCellPtr const &c ) {
        if ( c->containsPoint( to ) ) {
            cell = crimild::get_ptr( c );
        }
    } );

    if ( cell == nullptr ) {
        return from;
    }

    auto r = Ray3( Point3( to ), -Vector3f::Constants::UNIT_Y );
    const auto p = cell->getPlane();

    float t = Intersection::find( p, r );
    if ( t < 0 ) {
        r = Ray3f( to, Vector3f::UNIT_Y );
        t = Intersection::find( p, r );
        if ( t < 0 ) {
            return from;
        }
    }

    return r.getPointAt( t );
    */
    assert( true );
    return Vector3 {};
}

bool NavigationController::snap( void )
{
    // TODO: project current position into current cell and update position

    return true;
}

bool NavigationController::teleport( const Vector3f &target )
{
    auto cell = findCellForPoint( target );

    setCurrentCell( cell );
    assert( false );
    //getNode()->local().setTranslate( target );

    return cell != nullptr;
}

bool NavigationController::move( const Vector3f &target )
{
#if 0
    // TODO: what about local/world conversion?

    auto currentCell = getCurrentCell();
    if ( currentCell == nullptr ) {
        findCurrentCell();
        currentCell = getCurrentCell();
    }

    if ( currentCell == nullptr ) {
        // not in a cell
        return false;
    }

    auto motionPath = LineSegment3f( getNode()->getLocal().getTranslate(), target );

    bool done = false;

    auto testCell = currentCell;

    NavigationCellEdge *intersectionEdge = nullptr;
    Vector3f intersectionPoint;

    int attempts = 0;

    // Search for the cell containing the destination point
    // or update the motion path accordingly to keep it within
    // the nav mesh
    while ( !done && ( testCell != nullptr ) && ( motionPath.getOrigin() != motionPath.getDestination() ) ) {

        // classify the motion path based on the test cell
        auto result = testCell->classifyPath( motionPath, intersectionPoint, &intersectionEdge );

        if ( result == NavigationCell::ClassificationResult::INSIDE ) {
            // We found the cell containing the destination point
            // Project that point into the cell's plane and terminate
            motionPath.setDestination( testCell->getPlane().project( motionPath.getDestination() ) );
            done = true;
        } else if ( result == NavigationCell::ClassificationResult::OUTSIDE ) {
            // the motion path goes outside of the test cell
            if ( intersectionEdge->getNeighbor() != nullptr ) {
                // Moving to an adjacent cell. Set motion path origin
                // to intersection point and continue with next cell
                motionPath.setOrigin( intersectionPoint );
                testCell = intersectionEdge->getNeighbor();
                attempts = 0;
            } else {
                // We hit a wall.
                // Project the motion path on the intersection edge
                // and terminate
                motionPath = intersectionEdge->projectPath( motionPath );
                done = true;
            }
        } else {
            // this may happen if, for some reason, the start point of the motion path
            // lies outside of the current cell (maybe due to round errors) or it
            // coincides with one of the vertices
            // Force the motion path to start within the cell boundaries and try again
            motionPath.setOrigin( testCell->snapPoint( motionPath.getOrigin() ) );

            // failsafe
            ++attempts;
            if ( attempts > 100 ) {
                return false;
            }
        }
    }

    if ( testCell == nullptr ) {
        return false;
    }

    setCurrentCell( testCell );

    getNode()->local().setTranslate( motionPath.getDestination() );
#endif
    return true;
}

bool NavigationController::findCurrentCell( void )
{
    assert( false );
    //auto cell = NavigationController::findCellForPoint( getNode()->getLocal().getTranslate() );
    //if ( cell != nullptr ) {
        //setCurrentCell( cell );
    //}

    return getCurrentCell();
}

NavigationCell *NavigationController::findCellForPoint( const Vector3f &point )
{
    NavigationCell *cell = nullptr;
    getNavigationMesh()->foreachCell( [ &cell, point ]( NavigationCellPtr const &c ) {
        if ( c->containsPoint( point ) ) {
            cell = crimild::get_ptr( c );
        }
    } );

    return cell;
}

std::vector< Vector3f > NavigationController::computePathToTarget( const Vector3f &target )
{
    std::vector< Vector3f > result;

#if 0
    auto start = getCurrentCell();
    auto end = findCellForPoint( target );
    if ( end == nullptr ) {
        Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No cell found for target position" );
        // no path found
        return result;
    }

    if ( end == start ) {
        // simplest case. both origin and target at the same cell
        // the result is a straight line
        result.push_back( target );
        return result;
    }

    std::list< NavigationCell * > frontier;
    std::list< NavigationCell * > explored;
    std::map< NavigationCell *, crimild::Real32 > hs;
    std::map< NavigationCell *, NavigationCell * > cameFrom;

    hs[ start ] = -1;
    frontier.push_back( start );

    while ( frontier.size() > 0 ) {
        NavigationCell *next = nullptr;
        crimild::Real32 minH = 0;
        for ( auto candidate : frontier ) {
            auto h = hs[ candidate ];
            if ( next == nullptr || h < minH ) {
                next = candidate;
                minH = hs[ candidate ];
            }
        }

        if ( next == nullptr ) {
            // no moves left. Cannot find path
            Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No moves left. Cannot find path to target position" );
            return result;
        }

        if ( next == end ) {
            // found a path
            break;
        }

        frontier.remove( next );
        explored.push_back( next );

        next->foreachEdge( [ &frontier, &explored, &hs, &cameFrom, next, target ]( NavigationCellEdgePtr const &e ) {
            auto n = e->getNeighbor();
            if ( n != nullptr ) {
                if ( std::find( frontier.begin(), frontier.end(), n ) == frontier.end() && std::find( explored.begin(), explored.end(), n ) == explored.end() ) {
                    auto h = Distance::computeSquared( n->getCenter(), target );
                    frontier.push_back( n );
                    hs[ n ] = h;
                    cameFrom[ n ] = next;
                }
            }
        } );
    }

    std::list< Vector3f > path;
    path.push_front( target );

    auto current = end;
    while ( current != nullptr && current != start ) {
        if ( current != end ) {
            path.push_front( current->getCenter() );
        }
        current = cameFrom[ current ];
    }

    for ( auto p : path ) {
        result.push_back( p );
    }
#endif

    return result;
}
