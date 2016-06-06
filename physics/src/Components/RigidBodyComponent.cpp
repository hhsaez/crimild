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
#include "Collider.hpp"

#include "Foundation/PhysicsContext.hpp"

using namespace crimild;
using namespace crimild::physics;

physics::RigidBodyComponent::RigidBodyComponent( void )
	: physics::RigidBodyComponent( 0.0f )
{

}

physics::RigidBodyComponent::RigidBodyComponent( float mass )
	: _body( nullptr ),
      _shape( nullptr ),
      _mass( mass ),
      _kinematic( false ),
      _linearFactor( 0.0f, 0.0f, 0.0f ),
	  _linearVelocity( 0.0f, 0.0f, 0.0f ),
	  _angularFactor( 0.0f, 0.0f, 0.0f ),
	  _constraintVelocity( false ),
	  _restitution( 0.0f ),
	  _friction( 0.5f )
{

}

physics::RigidBodyComponent::~RigidBodyComponent( void )
{
	cleanup();
}

void physics::RigidBodyComponent::onAttach( void )
{

}

void physics::RigidBodyComponent::onDetach( void )
{
	if ( _body != nullptr ) {
		PhysicsContext::getInstance()->getWorld()->removeRigidBody( crimild::get_ptr( _body ) );
	}

	cleanup();
}

void physics::RigidBodyComponent::start( void )
{
	createShape();

	if ( _shape != nullptr ) {
		createBody();
	}

	if ( _body != nullptr ) {
		PhysicsContext::getInstance()->getWorld()->addRigidBody( crimild::get_ptr( _body ) );
	}
}

void physics::RigidBodyComponent::update( const Clock &t )
{
    if ( _body == nullptr ) {
    	Log::Warning << "No rigid body found" << Log::End;
        return;
    }
    
	if ( isKinematic() ) {
    	_body->setWorldTransform( BulletUtils::convert( getNode()->getWorld() ) );
	}
    else {
        btTransform trans = _body->getWorldTransform();
        getNode()->local().setTranslate( trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z() );
        //getNode()->perform( UpdateWorldState() );
        
        if ( shouldConstraintVelocity() ) {
            btVector3 currentVelocityDirection =_body->getLinearVelocity();
            btScalar currentVelocty = currentVelocityDirection.length();
            if ( currentVelocty != _desiredVelocity ) {
                currentVelocityDirection *= _desiredVelocity / currentVelocty;
                _body->setLinearVelocity( currentVelocityDirection );
            }
        }
    }
}

void physics::RigidBodyComponent::setLinearFactor( const Vector3f &linearFactor ) 
{ 
	_linearFactor = linearFactor; 

	if ( _body != nullptr ) {
    	_body->setLinearFactor( BulletUtils::convert( getLinearFactor() ) );
    }
}

void physics::RigidBodyComponent::setLinearVelocity( const Vector3f &linearVelocity )
{ 
	_linearVelocity = linearVelocity; 

	if ( _body != nullptr ) {
	    _body->setLinearVelocity( BulletUtils::convert( getLinearVelocity() ) );
	}

    if ( shouldConstraintVelocity() ) {
    	_desiredVelocity = getLinearVelocity().getMagnitude();
    }
}

Vector3f physics::RigidBodyComponent::getCurrentLinearVelocity( void ) const
{
	if ( _body == nullptr ) {
		return getLinearVelocity();
	}

	return BulletUtils::convert( _body->getLinearVelocity() );
}

void physics::RigidBodyComponent::setAngularFactor( const Vector3f &angularFactor ) 
{ 
	_angularFactor = angularFactor; 

	if ( _body != nullptr ) {
	    _body->setAngularFactor( BulletUtils::convert( getAngularFactor() ) );
	}
}

void physics::RigidBodyComponent::createShape( void )
{
	auto collider = getComponent< Collider >();
	if ( collider == nullptr ) {
		Log::Error << "No collider assigned to rigid body" << Log::End;
		return;
	}

	_shape = collider->generateShape();
}

void physics::RigidBodyComponent::createBody( void )
{
	if ( _shape == nullptr ) {
		Log::Debug << "No shape for rigid body" << Log::End;
		return;
	}

	getNode()->perform( UpdateWorldState() );

    btDefaultMotionState* motionState = new btDefaultMotionState( BulletUtils::convert( getNode()->getWorld() ) );
 
    btScalar bodyMass = isKinematic() ? 0.0f : getMass();
    btVector3 bodyInertia( 0.0f, 0.0f, 0.0f );
    _shape->calculateLocalInertia( bodyMass, bodyInertia );
 
    btRigidBody::btRigidBodyConstructionInfo bodyCI = btRigidBody::btRigidBodyConstructionInfo( bodyMass, motionState, crimild::get_ptr( _shape ), bodyInertia );
 
    bodyCI.m_restitution = _restitution;
    bodyCI.m_friction = _friction;
 
    _body = crimild::alloc< btRigidBody >( bodyCI );
    _body->setUserPointer( this );
    _body->setLinearFactor( BulletUtils::convert( getLinearFactor() ) );
    _body->setLinearVelocity( BulletUtils::convert( getLinearVelocity() ) );

    if ( shouldConstraintVelocity() ) {
    	_desiredVelocity = getLinearVelocity().getMagnitude();
    }

    _body->setAngularFactor( BulletUtils::convert( getAngularFactor() ) );
}

void physics::RigidBodyComponent::cleanup( void )
{
	if ( _body != nullptr ) {
		delete _body->getMotionState();
		_body = nullptr;
	}

	if ( _shape != nullptr ) {
		_shape = nullptr;
	}
}

void physics::RigidBodyComponent::onCollision( RigidBodyComponent *other )
{
	if ( _collisionCallback != nullptr ) {
		_collisionCallback( other );
	}
}

bool physics::RigidBodyComponent::checkGroundCollision( void ) const
{
	if ( PhysicsContext::getInstance() == nullptr ) {
		throw RuntimeException( "Invalid PhysicsContext instance" );
	}

    auto world = PhysicsContext::getInstance()->getWorld();
    if ( world == nullptr ) {
    	return false;
    }

	auto center = getNode()->getWorldBound()->getCenter();

    auto from = BulletUtils::convert( center );
    auto to = from + BulletUtils::convert( Vector3f( 0.0f, -20.0f * getNode()->getWorldBound()->getRadius(), 0.0f ) );
    btCollisionWorld::ClosestRayResultCallback res( from, to );

    world->rayTest( from, to, res );

    bool grounded = false;
    if ( res.hasHit() ) {
    	auto d = Distance::compute( center, BulletUtils::convert( res.m_hitPointWorld ) );
    	grounded = !Numericf::isZero( d ) && d < getNode()->getWorldBound()->getRadius();
    }

    return grounded;
}

