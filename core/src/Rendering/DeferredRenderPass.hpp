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

#ifndef CRIMILD_RENDERER_DEFERRED_RENDER_PASS_
#define CRIMILD_RENDERER_DEFERRED_RENDER_PASS_

#include "RenderPass.hpp"
#include "ShadowMap.hpp"

#include <map>

namespace crimild {
    
    class Light;
    
	class DeferredRenderPass : public RenderPass {
        CRIMILD_DISALLOW_COPY_AND_ASSIGN( DeferredRenderPass )
        
    protected:
		DeferredRenderPass( void );
        
	public:
		virtual ~DeferredRenderPass( void );
        
        virtual void render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        
    public:
        void setGBufferProgram( ShaderProgram *program ) { _gBufferProgram = program; }
        ShaderProgram *getGBufferProgram( void ) { return _gBufferProgram.get(); }
        
        void setGBufferCompositionProgram( ShaderProgram *program ) { _gBufferCompositionProgram = program; }
        ShaderProgram *getGBufferCompositionProgram( void ) { return _gBufferCompositionProgram.get(); }
        
    private:
        void buildAccumBuffer( int width, int heigth );
        
        void buildGBuffer( int width, int height );
        void renderToGBuffer( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        
        void buildFrameBuffer( int width, int height );
        void composeFrame( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        
        void renderTranslucentObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        
        void computeShadowMaps( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        
    private:
        Pointer< ShaderProgram > _gBufferProgram;
        Pointer< ShaderProgram > _gBufferCompositionProgram;
        
        Pointer< FrameBufferObject > _gBuffer;
        Pointer< Texture > _gBufferDepthOutput;
        Pointer< Texture > _gBufferPositionOutput;
        Pointer< Texture > _gBufferNormalOutput;
        Pointer< Texture > _gBufferColorOutput;
        Pointer< Texture > _gBufferEmissiveOutput;
        
        Pointer< FrameBufferObject > _frameBuffer;
        Pointer< Texture > _frameBufferOutput;
        
        Pointer< FrameBufferObject > _accumBuffer;
        Pointer< Texture > _accumBufferOutput;
        
        std::map< Light *, Pointer< ShadowMap > > _shadowMaps;
	};
    
}

#endif

