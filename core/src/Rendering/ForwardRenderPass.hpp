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

#ifndef CRIMILD_RENDERER_FORWARD_RENDER_PASS_
#define CRIMILD_RENDERER_FORWARD_RENDER_PASS_

#include "RenderPass.hpp"
#include "ShadowMap.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"
#include "RenderQueue.hpp"
#include "ShadowMap.hpp"

#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Light.hpp"

#include <map>

namespace crimild {
    
	class ForwardRenderPass : public RenderPass {
	public:
		ForwardRenderPass( void );
		virtual ~ForwardRenderPass( void );
        
        virtual void render( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera ) override;
        
    protected:
        virtual void renderShadedObjects( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera );
        virtual void renderTranslucentObjects( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera );
        
    private:
        void buildAccumBuffer( int width, int height );
        void computeShadowMaps( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera );
        
        std::map< LightPtr, ShadowMapPtr > _shadowMaps;
        FrameBufferObjectPtr _forwardPassBuffer;
        TexturePtr _forwardPassResult;
        
        FrameBufferObjectPtr _accumBuffer;
        TexturePtr _accumBufferOutput;
	};
    
}

#endif

