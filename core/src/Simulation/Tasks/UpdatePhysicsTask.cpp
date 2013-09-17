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

#include "UpdatePhysicsTask.hpp"
#include "Components/NodeComponentCatalog.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/ColliderComponent.hpp"

using namespace crimild;

UpdatePhysicsTask::UpdatePhysicsTask( unsigned int priority, NodePtr scene )
	: Task( priority ),
	  _scene( scene )
{

}

UpdatePhysicsTask::~UpdatePhysicsTask( void )
{

}

void UpdatePhysicsTask::update( void ) 
{
	NodeComponentCatalog< RigidBodyComponent >::getInstance().forEach( []( RigidBodyComponent *rigidBody ) {
		NodeComponentCatalog< ColliderComponent >::getInstance().forEach( [&]( ColliderComponent *collider ) {
			if ( rigidBody->testCollision( collider ) ) {
				rigidBody->resolveCollision( collider );
				ColliderComponent *rbCollider = rigidBody->getNode()->getComponent< ColliderComponent >();
				if ( rbCollider ) {
					collider->onCollision( rbCollider );
				}
			}
		});
	});
}

