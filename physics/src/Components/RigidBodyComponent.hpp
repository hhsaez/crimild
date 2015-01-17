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

#ifndef CRIMILD_PHYSICS_COMPONENTS_RIGID_BODY_
#define CRIMILD_PHYSICS_COMPONENTS_RIGID_BODY_

#include "Foundation/BulletUtils.hpp"

namespace crimild {

	namespace physics {
        
        class RigidBodyComponent;
        
        using RigidBodyComponentPtr = SharedPointer< RigidBodyComponent >;

		class RigidBodyComponent : public NodeComponent {
			CRIMILD_DISALLOW_COPY_AND_ASSIGN( RigidBodyComponent )
			CRIMILD_NODE_COMPONENT_NAME( "rigidBody" );

		public:
			typedef std::function< void ( RigidBodyComponentPtr const & ) > CollisionCallback;

		public:
			RigidBodyComponent( void );
			RigidBodyComponent( float mass, bool convex = true );
			virtual ~RigidBodyComponent( void );

			virtual void onAttach( void ) override;
			virtual void onDetach( void ) override;

			virtual void start( void ) override;
			virtual void update( const Time &t ) override;

			void setMass( float mass ) { _mass = mass; }
			float getMass( void ) const { return _mass; }

			void setConvex( bool convex ) { _convex = convex; }
			bool isConvex( void ) const { return _convex; } 

			void setKinematic( bool kinematic ) { _kinematic = kinematic; }
			bool isKinematic( void ) const { return _kinematic; }

			void setLinearFactor( const Vector3f &linearFactor ) { _linearFactor = linearFactor; }
			const Vector3f &getLinearFactor( void ) const { return _linearFactor; }

			void setLinearVelocity( const Vector3f &linearVelocity ) { _linearVelocity = linearVelocity; }
			const Vector3f &getLinearVelocity( void ) const { return _linearVelocity; }

			void setConstraintVelocity( bool value ) { _constraintVelocity = value; }
			bool shouldConstraintVelocity( void ) const { return _constraintVelocity; }

			void onCollision( RigidBodyComponentPtr const &other );

			void setCollisionCallback( CollisionCallback callback ) { _collisionCallback = callback; }
			CollisionCallback getCollisionCallback( void ) { return _collisionCallback; }

		private:
			void createShape( void );
			void createBody( void );

			void cleanup( void );

		private:
			btRigidBody *_body;
			btCollisionShape *_shape;

			float _mass;
			bool _convex;
			bool _kinematic;
			Vector3f _linearFactor;
			Vector3f _linearVelocity;
			bool _constraintVelocity;
			float _desiredVelocity;

			CollisionCallback _collisionCallback;
		};

	}
	
}

#endif

