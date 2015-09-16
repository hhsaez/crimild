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
#include "Foundation/SharedObjectArray.hpp"

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
        static constexpr const char *G_BUFFER_NAME = "G_BUFFER";
        static constexpr const char *G_BUFFER_DIFFUSE_TARGET_NAME = "0-DIFFUSE";
        static constexpr const char *G_BUFFER_POSITION_TARGET_NAME = "1-POSITION";
        static constexpr const char *G_BUFFER_NORMAL_TARGET_NAME = "2-NORMAL";
        static constexpr const char *G_BUFFER_VIEW_SPACE_NORMAL_TARGET_NAME = "3-VIEW_SPACE_NORMAL";
        static constexpr const char *G_BUFFER_DEPTH_TARGET_NAME = "DEPTH";
        
        static constexpr const char *S_BUFFER_NAME = "S_BUFFER";
        static constexpr const char *S_BUFFER_COLOR_TARGET_NAME = "0-COLOR";
        static constexpr const char *S_BUFFER_DEPTH_TARGET_NAME = "DEPTH";
        
        static constexpr const char *D_BUFFER_NAME = "D_BUFFER";
        static constexpr const char *D_BUFFER_COLOR_TARGET_NAME = "0-COLOR";
        static constexpr const char *D_BUFFER_DEPTH_TARGET_NAME = "DEPTH";
        
	public:
		RenderPass( void );
		virtual ~RenderPass( void );

        virtual void render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        virtual void render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera, RenderQueue::Renderables const &objects );
        virtual void render( Renderer *renderer, Texture *texture, ShaderProgram *program );
        
        SharedObjectArray< ImageEffect > &getImageEffects( void ) { return _imageEffects; }
        
    protected:
        Primitive *getScreenPrimitive( void ) { return crimild::get_ptr( _screen ); }

        virtual void renderOpaqueObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        virtual void renderTranslucentObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        virtual void renderScreenObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );

	private:
		SharedPointer< Primitive > _screen;
        SharedObjectArray< ImageEffect > _imageEffects;
	};

}

#endif

