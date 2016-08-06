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

#ifndef CRIMILD_CORE_COMPONENTS_UI_RESPONDER_
#define CRIMILD_CORE_COMPONENTS_UI_RESPONDER_

#include "NodeComponent.hpp"

#include "SceneGraph/Node.hpp"

#include "Mathematics/Ray.hpp"

namespace crimild {

	class BoundingVolume;

	class UIResponder : public NodeComponent {
		CRIMILD_IMPLEMENT_RTTI( crimild::UIResponder )

	private:
		using CallbackType = std::function< bool( Node * ) >;

	public:
		explicit UIResponder( CallbackType callback );
		UIResponder( CallbackType callback, BoundingVolume *boundingVolume );
		virtual ~UIResponder( void );

		virtual void onAttach( void ) override;
		virtual void onDetach( void ) override;

		virtual void start( void ) override;

        BoundingVolume *getBoundingVolume( void );
        void setBoundingVolume( BoundingVolume *boundingVolume );

		bool testIntersection( const Ray3f &ray );

		bool invoke( void );

		virtual void renderDebugInfo( Renderer *renderer, Camera *camera ) override;

	private:
		CallbackType _callback;
        SharedPointer< BoundingVolume > _boundingVolume;
	};

}

#endif

