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

#include "UIResponder.hpp"

#include "Debug/DebugRenderHelper.hpp"
#include "SceneGraph/Node.hpp"

using namespace crimild;

UIResponder::UIResponder( CallbackType callback )
    : UIResponder( callback, nullptr )
{
}

UIResponder::UIResponder( CallbackType callback, BoundingVolume *boundingVolume )
    : _callback( callback )
{
    setBoundingVolume( boundingVolume );
}

UIResponder::~UIResponder( void )
{
}

void UIResponder::onAttach( void )
{
}

void UIResponder::onDetach( void )
{
}

void UIResponder::start( void )
{
}

crimild::BoundingVolume *UIResponder::getBoundingVolume( void )
{
    if ( _boundingVolume != nullptr ) {
        return crimild::get_ptr( _boundingVolume );
    }

    return getNode()->worldBound();
}

void UIResponder::setBoundingVolume( crimild::BoundingVolume *boundingVolume )
{
    _boundingVolume = crimild::retain( boundingVolume );
}

void UIResponder::setBoundingVolume( const SharedPointer< BoundingVolume > &volume )
{
    _boundingVolume = volume;
}

bool UIResponder::testIntersection( const Ray3 &ray )
{
    if ( getBoundingVolume() != nullptr ) {
        return getBoundingVolume()->testIntersection( ray );
    }

    return getNode()->getWorldBound()->testIntersection( ray );
}

bool UIResponder::invoke( void )
{
    if ( _callback != nullptr ) {
        return _callback( getNode() );
    }

    return false;
}

void UIResponder::renderDebugInfo( Renderer *renderer, Camera *camera )
{
    if ( auto bounds = getBoundingVolume() ) {
        bounds->renderDebugInfo( renderer, camera );
    }

    /*
	const Vector3f &c = bounds->getCenter();
	auto min = bounds->getMin();
	auto max = bounds->getMax();

	Vector3f lines[] = {
		c + Vector3f( min[ 0 ], min[ 1 ], min[ 2 ] ), c + Vector3f( max[ 0 ], min[ 1 ], min[ 2 ] ),
		c + Vector3f( max[ 0 ], min[ 1 ], min[ 2 ] ), c + Vector3f( max[ 0 ], min[ 1 ], max[ 2 ] ),
		c + Vector3f( max[ 0 ], min[ 1 ], max[ 2 ] ), c + Vector3f( min[ 0 ], min[ 1 ], max[ 2 ] ),
		c + Vector3f( min[ 0 ], min[ 1 ], max[ 2 ] ), c + Vector3f( min[ 0 ], min[ 1 ], min[ 2 ] ),

		c + Vector3f( min[ 0 ], min[ 1 ], min[ 2 ] ), c + Vector3f( min[ 0 ], max[ 1 ], min[ 2 ] ),
		c + Vector3f( max[ 0 ], min[ 1 ], min[ 2 ] ), c + Vector3f( max[ 0 ], max[ 1 ], min[ 2 ] ),
		c + Vector3f( max[ 0 ], min[ 1 ], max[ 2 ] ), c + Vector3f( max[ 0 ], max[ 1 ], max[ 2 ] ),
		c + Vector3f( min[ 0 ], min[ 1 ], max[ 2 ] ), c + Vector3f( min[ 0 ], max[ 1 ], max[ 2 ] ),

		c + Vector3f( min[ 0 ], max[ 1 ], min[ 2 ] ), c + Vector3f( max[ 0 ], max[ 1 ], min[ 2 ] ),
		c + Vector3f( max[ 0 ], max[ 1 ], min[ 2 ] ), c + Vector3f( max[ 0 ], max[ 1 ], max[ 2 ] ),
		c + Vector3f( max[ 0 ], max[ 1 ], max[ 2 ] ), c + Vector3f( min[ 0 ], max[ 1 ], max[ 2 ] ),
		c + Vector3f( min[ 0 ], max[ 1 ], max[ 2 ] ), c + Vector3f( min[ 0 ], max[ 1 ], min[ 2 ] ),
	};

	DebugRenderHelper::renderLines( renderer, camera, lines, 12 * 2, RGBAColorf( 1.0f, 1.0f, 0.0f, 1.0f ) );
     */
}
