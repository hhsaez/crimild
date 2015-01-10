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

#ifndef CRIMILD_RENDERER_RENDER_PASS_
#define CRIMILD_RENDERER_RENDER_PASS_

#include "Foundation/SharedObject.hpp"
#include "Foundation/Pointer.hpp"
#include "Foundation/SharedObjectList.hpp"

#include "RenderQueue.hpp"

#include <map>

namespace crimild {
    
    class Camera;
    class Geometry;
    class ImageEffect;
    class Material;
    class Primitive;
    class Renderer;
    class ShaderProgram;
    class Texture;

	class RenderPass : public SharedObject {
	public:
		RenderPass( void );
		virtual ~RenderPass( void );

        virtual void render( std::shared_ptr< Renderer > const &renderer, RenderQueuePtr const &renderQueue, std::shared_ptr< Camera > const &camera );
        virtual void render( std::shared_ptr< Renderer > const &renderer, RenderQueuePtr const &renderQueue, std::shared_ptr< Camera > const &camera, RenderQueue::MaterialMap const &objects );
        virtual void render( std::shared_ptr< Renderer > const &renderer, std::shared_ptr< Texture > const &texture, std::shared_ptr< ShaderProgram > const &program );
        
        std::shared_ptr< SharedObjectList< ImageEffect >> &getImageEffects( void ) { return _imageEffects; }
        
    protected:
        std::shared_ptr< Primitive > &getScreenPrimitive( void ) { return _screen; }

        virtual void renderOpaqueObjects( std::shared_ptr< Renderer > const &renderer, std::shared_ptr< RenderQueue > const &renderQueue, std::shared_ptr< Camera > const &camera );
        virtual void renderTranslucentObjects( std::shared_ptr< Renderer > const &renderer, std::shared_ptr< RenderQueue > const &renderQueue, std::shared_ptr< Camera > const &camera );
        virtual void renderScreenObjects( std::shared_ptr< Renderer > const &renderer, std::shared_ptr< RenderQueue > const &renderQueue, std::shared_ptr< Camera > const &camera );

	private:
		std::shared_ptr< Primitive > _screen;
        std::shared_ptr< SharedObjectList< ImageEffect >> _imageEffects;
	};
    
    using RenderPassPtr = std::shared_ptr< RenderPass >;

    using BasicRenderPass = RenderPass;
    using BasicRenderPassPtr = RenderPassPtr;

}

#endif

