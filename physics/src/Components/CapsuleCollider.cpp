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

#include "CapsuleCollider.hpp"

using namespace crimild;
using namespace crimild::physics;

CapsuleCollider::CapsuleCollider( void )
	: CapsuleCollider( 1.75f, 1.75 )
{

}

CapsuleCollider::CapsuleCollider( float width, float height )
	: _width( width ),
	  _height( height ),
	  _offset( 0.0f, 0.0f, 0.0f )
{

}

CapsuleCollider::~CapsuleCollider( void )
{

}

SharedPointer< btCollisionShape > CapsuleCollider::generateShape( void ) 
{
	Log::Debug << "Generating shape for capsule collider" << Log::End;

	auto capsule = new btCapsuleShape( getWidth(), getHeight() );

	btTransform boxTransform;
	boxTransform.setIdentity();
	boxTransform.setOrigin( BulletUtils::convert( getOffset() ) );

	auto shape = crimild::alloc< btCompoundShape >();
	shape->addChildShape( boxTransform, capsule );

	return shape;
}

void CapsuleCollider::renderDebugInfo( Renderer *renderer, Camera *camera )
{
	DebugRenderHelper::renderBox( 
		renderer, 
		camera, 
		getNode()->getWorld().getTranslate() + getOffset(), 
		Vector3f( getWidth(), getHeight(), 1.0f ),
		RGBAColorf( 1.0f, 1.0f, 0.0f, 0.5f ) );
}

