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
#include "NavigationMeshContainer.hpp"

#include "Mathematics/Ray.hpp"
#include "Mathematics/Intersection.hpp"

#include "SceneGraph/Node.hpp"

#include "Visitors/Apply.hpp"

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
		getNode()->getRootParent()->perform( Apply( [this]( Node *node ) {
			auto nav = node->getComponent< NavigationMeshContainer >();
			if ( nav != nullptr ) {
				_navigationMesh = crimild::retain( nav->getNavigationMesh() );
			}
		}));
	}
}

Vector3f NavigationController::move( const Vector3f &from, const Vector3f &to )
{
	if ( _navigationMesh == nullptr ) {
		Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No navigation mesh found" );
		return from;
	}

	NavigationCell *cell = nullptr;
	getNavigationMesh()->foreachCell( [&cell, to]( NavigationCellPtr const &c ) {
		if ( c->containsPoint( to ) ) {
			cell = crimild::get_ptr( c );
		}
	});

	if ( cell == nullptr ) {
		return from;
	}

	auto r = Ray3f( to, -Vector3f::UNIT_Y );
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
}

