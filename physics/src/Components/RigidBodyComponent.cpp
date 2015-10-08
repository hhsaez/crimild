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

#include "Foundation/PhysicsContext.hpp"

using namespace crimild;
using namespace crimild::physics;

physics::RigidBodyComponent::RigidBodyComponent( void )
	: physics::RigidBodyComponent( 0.0f, false )
{

}

physics::RigidBodyComponent::RigidBodyComponent( float mass, bool convex )
	: _body( nullptr ),
      _shape( nullptr ),
      _mass( mass ),
      _convex( convex ),
      _kinematic( false ),
      _linearFactor( 0.0f, 0.0f, 0.0f ),
	  _linearVelocity( 0.0f, 0.0f, 0.0f ),
	  _constraintVelocity( false )
{

}

physics::RigidBodyComponent::~RigidBodyComponent( void )
{
	cleanup();
}

void physics::RigidBodyComponent::onAttach( void )
{
	createShape();

	if ( _shape != nullptr ) {
		createBody();
	}

	if ( _body != nullptr ) {
		PhysicsContext::getInstance().getWorld()->addRigidBody( _body );
	}
}

void physics::RigidBodyComponent::onDetach( void )
{
	if ( _body != nullptr ) {
		PhysicsContext::getInstance().getWorld()->removeRigidBody( _body );
	}

	cleanup();
}

void physics::RigidBodyComponent::start( void )
{

}

void physics::RigidBodyComponent::update( const Clock &t )
{
	/*
    if ( _body == nullptr ) {
        return;
    }
    
	if ( isKinematic() ) {
    	_body->setWorldTransform( BulletUtils::convert( getNode()->getWorld() ) );
	}
    else {
        btTransform trans = _body->getWorldTransform();
        getNode()->local().setTranslate( trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z() );
        getNode()->perform( UpdateWorldState() );
        
        if ( shouldConstraintVelocity() ) {
            btVector3 currentVelocityDirection =_body->getLinearVelocity();
            btScalar currentVelocty = currentVelocityDirection.length();
            if ( currentVelocty != _desiredVelocity ) {
                currentVelocityDirection *= _desiredVelocity / currentVelocty;
                _body->setLinearVelocity( currentVelocityDirection );
            }
        }
    }
	*/
}

void physics::RigidBodyComponent::createShape( void )
{
	/*
	if ( isConvex() ) {
		_shape = new btConvexHullShape();

		getNode()->perform( ApplyToGeometries( [&]( GeometryPtr const &geometry ) {
			geometry->foreachPrimitive( [&]( PrimitivePtr const &primitive ) {
				auto vbo = primitive->getVertexBuffer();
				for ( int i = 0; i < vbo->getVertexCount(); i++ ) {
					Vector3f v = vbo->getPositionAt( i );
					btVector3 btv = btVector3( v[ 0 ], v[ 1 ], v[ 2 ] );
            		( ( btConvexHullShape * ) _shape )->addPoint( btv );
				}
			});
		}));
	}
	else {
		btTriangleMesh* mesh = new btTriangleMesh();

		getNode()->perform( ApplyToGeometries( [&]( GeometryPtr const &geometry ) {
			geometry->foreachPrimitive( [&]( PrimitivePtr const &primitive ) {
				auto ibo = primitive->getIndexBuffer();
				auto vbo = primitive->getVertexBuffer();

				const unsigned short *indices = static_cast< const unsigned short * >( ibo->getData() );
				Vector3f vertices[ 3 ];
				for ( int i = 0; i < ibo->getIndexCount() / 3; i++ ) {
					for ( int j = 0; j < 3; j++ ) {
						vertices[ j ] = vbo->getPositionAt( indices[ i * 3 + j ] );
					}
					mesh->addTriangle( BulletUtils::convert( vertices[ 0 ] ), BulletUtils::convert( vertices[ 1 ] ), BulletUtils::convert( vertices[ 2 ] ) );
				}
			});
		}));

		_shape = new btBvhTriangleMeshShape( mesh, true );
	}
	*/
}

void physics::RigidBodyComponent::createBody( void )
{
	getNode()->perform( UpdateWorldState() );

    btDefaultMotionState* motionState = new btDefaultMotionState( BulletUtils::convert( getNode()->getWorld() ) );
 
    btScalar bodyMass = isKinematic() ? 0.0f : getMass();
    btVector3 bodyInertia;
    _shape->calculateLocalInertia( bodyMass, bodyInertia );
 
    btRigidBody::btRigidBodyConstructionInfo bodyCI = btRigidBody::btRigidBodyConstructionInfo( bodyMass, motionState, _shape, bodyInertia );
 
    bodyCI.m_restitution = 1.0f;
    bodyCI.m_friction = 0.5f;
 
    _body = new btRigidBody( bodyCI );
    _body->setUserPointer( this );
    _body->setLinearFactor( BulletUtils::convert( getLinearFactor() ) );
    _body->setLinearVelocity( BulletUtils::convert( getLinearVelocity() ) );

    if ( shouldConstraintVelocity() ) {
    	_desiredVelocity = getLinearVelocity().getMagnitude();
    }
}

void physics::RigidBodyComponent::cleanup( void )
{
	if ( _body != nullptr ) {
		delete _body->getMotionState();
		delete _body;
		_body = nullptr;
	}

	if ( _shape != nullptr ) {
		delete _shape;
		_shape = nullptr;
	}
}

void physics::RigidBodyComponent::onCollision( RigidBodyComponentPtr const &other )
{
	if ( _collisionCallback != nullptr ) {
		_collisionCallback( other );
	}
}

