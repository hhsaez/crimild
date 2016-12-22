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

#include "CharacterController.hpp"
#include "Collider.hpp"

#include "Foundation/PhysicsContext.hpp"

using namespace crimild;
using namespace crimild::physics;

CharacterController::CharacterController( void )
{

}

CharacterController::~CharacterController( void )
{

}

void CharacterController::onAttach( void )
{

}

void CharacterController::onDetach( void )
{

}

void CharacterController::start( void )
{
	auto collider = getComponent< Collider >();
	if ( collider == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No collider found for character controller" );
		return;
	}

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin( btVector3( 0.0f, 0.0f, 0.0f ) );

	_ghostObject = crimild::alloc< btPairCachingGhostObject >();
	_ghostObject->setWorldTransform( startTransform );

	_shape = crimild::cast_ptr< btConvexShape >( collider->generateShape() );

	_ghostObject->setCollisionShape( crimild::get_ptr( _shape ) );
	_ghostObject->setCollisionFlags( btCollisionObject::CF_CHARACTER_OBJECT );

	btScalar stepHeight = btScalar( 0.35 );
	_character = crimild::alloc< btKinematicCharacterController >( crimild::get_ptr( _ghostObject ), crimild::get_ptr( _shape ), stepHeight );

	_character->setGravity( 10.0f );

	PhysicsContext::getInstance()->getWorld()->addCollisionObject( crimild::get_ptr( _ghostObject ), btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter );

	PhysicsContext::getInstance()->getWorld()->addAction( crimild::get_ptr( _character ) );
}

void CharacterController::update( const Clock &c )
{
	if ( _ghostObject == nullptr || _character == nullptr ) {
		return;
	}

	_ghostObject->setWorldTransform( BulletUtils::convert( getNode()->getWorld() ) );

    Vector3f move( 0.0f, 0.0f, 0.0f );

    if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_RIGHT ) ) {
        move[ 0 ] = Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT ) ? 3.0f : 1.0f;
        // getNode()->local().rotate().fromAxisAngle( Vector3f( 0.0f, 1.0f, 0.0f ), Numericf::HALF_PI );
    }
    else if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT ) ) {
        move[ 0 ] = Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT ) ? -3.0f : -1.0f;
        // getNode()->local().rotate().fromAxisAngle( Vector3f( 0.0f, 1.0f, 0.0f ), -Numericf::HALF_PI );
    }

	_character->setWalkDirection( BulletUtils::convert( move ) );
}

void CharacterController::renderDebugInfo( Renderer *renderer, Camera *camera )
{

}

