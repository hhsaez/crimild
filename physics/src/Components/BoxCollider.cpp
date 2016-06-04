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

#include "BoxCollider.hpp"

using namespace crimild;
using namespace crimild::physics;

BoxCollider::BoxCollider( void )
	: BoxCollider( Vector3f( 1.0f, 1.0f, 1.0f ) )
{

}

BoxCollider::BoxCollider( const Vector3f &boxHalfExtents )
	: _boxHalfExtents( boxHalfExtents ),
	  _offset( 0.0f, 0.0f, 0.0f )
{

}

BoxCollider::~BoxCollider( void )
{

}

SharedPointer< btCollisionShape > BoxCollider::generateShape( void )
{
	Log::Debug << "Generating shape for box collider" << Log::End;
	
	auto box = new btBoxShape( btVector3( _boxHalfExtents[ 0 ], _boxHalfExtents[ 1 ], _boxHalfExtents[ 2 ] ) );
	btTransform boxTransform;
	boxTransform.setIdentity();
	boxTransform.setOrigin( btVector3( _offset[ 0 ], _offset[ 1 ], _offset[ 2 ] ) );

	auto shape = crimild::alloc< btCompoundShape >();
	shape->addChildShape( boxTransform, box );

	return shape;
}

void BoxCollider::renderDebugInfo( Renderer *renderer, Camera *camera )
{
	auto size = 2.0f * getHalfExtents();
	auto center = getNode()->getWorld().getTranslate() + getOffset();

	DebugRenderHelper::renderBox( renderer, camera, center, size, RGBAColorf( 1.0f, 0.0f, 0.0f, 0.5f ) );
}

