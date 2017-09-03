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

#ifndef CRIMILD_CORE_COMPONENTS_SKINNED_MESH_
#define CRIMILD_CORE_COMPONENTS_SKINNED_MESH_

#include "NodeComponent.hpp"

#include "Foundation/SharedObject.hpp"

namespace crimild {

	class SkinnedMesh;

	class SkinnedMeshComponent : public NodeComponent {
		CRIMILD_IMPLEMENT_RTTI( crimild::SkinnedMeshComponent )

	public:
		using AnimationProgressCallback = std::function< void( float ) >;

	public:
		SkinnedMeshComponent( void );
		SkinnedMeshComponent( SharedPointer< SkinnedMesh > const &skinnedMesh );
		virtual ~SkinnedMeshComponent( void );

		virtual void start( void ) override;
		virtual void update( const Clock &c ) override;
		virtual void renderDebugInfo( Renderer *renderer, Camera *camera ) override;

		void setSkinnedMesh( SharedPointer< SkinnedMesh > const &mesh ) { _skinnedMesh = mesh; }
		SkinnedMesh *getSkinnedMesh( void ) { return crimild::get_ptr( _skinnedMesh ); }

		void setAnimationParams( 
			float firstFrame, 
			float lastFrame, 
			bool loop, 
			float timeScale = 1.0f,
			float timeOffset = 0.0f,
			AnimationProgressCallback const &animationProgressCallack = nullptr );

	private:
		float _time = 0.0f;
		unsigned int _currentAnimation = 0;

		SharedPointer< SkinnedMesh > _skinnedMesh;

		float _firstFrame;
		float _lastFrame;
		bool _loop;
		float _timeScale;
		AnimationProgressCallback _animationProgressCallback;

		/**
			\name Streaming
		*/
		//@{

	public:
		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;

		//@}

	};

}

#endif

