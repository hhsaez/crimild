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

#include "RigidBodyComponent.hpp"
#include "ColliderComponent.hpp"
#include "NodeComponentCatalog.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

RigidBodyComponent::RigidBodyComponent( void )
	: _gravity( 0.0f, -4.8f, 0.0f )
{
	NodeComponentCatalog< RigidBodyComponent >::getInstance().registerComponent( this );
}

RigidBodyComponent::~RigidBodyComponent( void )
{
	NodeComponentCatalog< RigidBodyComponent >::getInstance().unregisterComponent( this );
}

void RigidBodyComponent::update( const Time &t )
{
	_previousPosition = getNode()->getLocal().getTranslate();
	Vector3f result = _previousPosition;

	Vector3f gravityStep = t.getDeltaTime() * _gravity;
	result += gravityStep;

	Vector3f forceStep = t.getDeltaTime() * _force;
	_force -= forceStep;
	result += forceStep;

	getNode()->local().setTranslate( result );
}

bool RigidBodyComponent::testCollision( ColliderComponent *other ) 
{
	if ( other->getNode() == getNode() ) {
		return false;
	}

	ColliderComponent *ownCollider = getNode()->getComponent< ColliderComponent >();
	if ( ownCollider == nullptr || ownCollider == other ) {
		return false;
	}

	return other->testCollision( ownCollider );
}

void RigidBodyComponent::resolveCollision( ColliderComponent *other ) 
{
	ColliderComponent *ownCollider = getNode()->getComponent< ColliderComponent >();
	if ( ownCollider == nullptr || ownCollider == other ) {
		return;
	}

	TransformationImpl result;
	ownCollider->getBoundingVolume()->resolveIntersection( other->getBoundingVolume(), result );

	getNode()->local().translate() += result.getTranslate();
	getNode()->perform( UpdateWorldState() );

	RigidBodyComponent *otherRB = other->getNode()->getComponent< RigidBodyComponent >();
	if ( otherRB ) {
		otherRB->setForce( getForce() );
		setForce( Vector3f( 0.0f, 0.0f, 0.0f ) );
	}
}

