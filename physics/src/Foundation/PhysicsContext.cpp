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

#include "PhysicsContext.hpp"

#include "../Components/RigidBodyComponent.hpp"

using namespace crimild;
using namespace crimild::physics;

PhysicsContext::PhysicsContext( void )
	: _broadphase( nullptr ),
	  _collisionConfiguration( nullptr ),
	  _dispatcher( nullptr ),
	  _solver( nullptr ),
	  _world( nullptr ),
	  _gravity( 0.0f, -9.8f, 0.0f )
{
	init();
}

PhysicsContext::~PhysicsContext( void )
{

}

void PhysicsContext::init( void )
{
	_broadphase = crimild::alloc< btDbvtBroadphase >();
 
    _collisionConfiguration = crimild::alloc< btDefaultCollisionConfiguration >();
    _dispatcher = crimild::alloc< btCollisionDispatcher >( crimild::get_ptr( _collisionConfiguration ) );
 
    _solver = crimild::alloc< btSequentialImpulseConstraintSolver >();
 
    _world = crimild::alloc< btDiscreteDynamicsWorld >( 
    	crimild::get_ptr( _dispatcher ), 
    	crimild::get_ptr( _broadphase ), 
    	crimild::get_ptr( _solver ), 
    	crimild::get_ptr( _collisionConfiguration ) );

    _world->getDispatchInfo().m_allowedCcdPenetration = 0.0001f; 
    _world->setGravity( BulletUtils::convert( _gravity ) );
}

void PhysicsContext::cleanup( void )
{
	_world = nullptr;
	_solver = nullptr;
	_dispatcher = nullptr;
	_collisionConfiguration = nullptr;
	_broadphase = nullptr;
}

void PhysicsContext::setGravity( const Vector3f &gravity ) 
{
	_gravity = gravity;
	if ( _world != nullptr ) {
		_world->setGravity( BulletUtils::convert( _gravity ) );
	}
}

void PhysicsContext::step( float dt )
{
	if ( _world == nullptr ) {
		return;
	}

	auto dispatcher = _world->getDispatcher();
	if ( dispatcher == nullptr ) {
		return;
	}

	_world->stepSimulation( dt );

	int numManifolds = dispatcher->getNumManifolds();
    for ( int i = 0; i < numManifolds; i++ ) {
		btPersistentManifold *contactManifold = dispatcher->getManifoldByIndexInternal( i );
 
		int numContacts = contactManifold->getNumContacts();
		if ( numContacts > 0 ) {
            const btCollisionObject* objA = contactManifold->getBody0();
            physics::RigidBodyComponent *rbA = static_cast< physics::RigidBodyComponent * >( objA->getUserPointer() );

            const btCollisionObject* objB = contactManifold->getBody1();
            physics::RigidBodyComponent *rbB = static_cast< physics::RigidBodyComponent * >( objB->getUserPointer() );
 
            if ( rbA != nullptr ) {
            	rbA->onCollision( rbB );
            }

            if ( rbB != nullptr ) {
            	rbB->onCollision( rbA );
            }
		}
    }
}

