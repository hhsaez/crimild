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

#ifndef CRIMILD_CORE_COMPONENTS_RIGID_BODY_
#define CRIMILD_CORE_COMPONENTS_RIGID_BODY_

#include "NodeComponent.hpp"
#include "Mathematics/Vector.hpp"

namespace crimild {

	class ColliderComponent;

	class RigidBodyComponent : public NodeComponent {
	public:
		static const char *COMPONENT_NAME;

	public:
		RigidBodyComponent( void );
		virtual ~RigidBodyComponent( void );

		void setGravity( const Vector3f &gravity ) { _gravity = gravity; }
		const Vector3f &getGravity( void ) const { return _gravity; }

		void setForce( const Vector3f &force ) { _force = force; }
		const Vector3f &getForce( void ) const { return _force; }

		virtual void update( const Time &t ) override;

		virtual bool testCollision( ColliderComponent *other );
		virtual void resolveCollision( ColliderComponent *other );

	private:
		Vector3f _gravity;
		Vector3f _force;
		Vector3f _previousPosition;
	};

}

#endif

