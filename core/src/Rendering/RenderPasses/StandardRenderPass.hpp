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

#ifndef CRIMILD_RENDERER_RENDER_PASS_STANDARD_
#define CRIMILD_RENDERER_RENDER_PASS_STANDARD_

#include "RenderPass.hpp"
 
#include "Rendering/ShadowMap.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/ShadowMap.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Light.hpp"

#include <map>

namespace crimild {
    
	class StandardRenderPass : public RenderPass {
	public:
		StandardRenderPass( void );
		virtual ~StandardRenderPass( void );
        
        virtual void render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera ) override;

    protected:
        void computeShadowMaps( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        
        void renderOccluders( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        virtual void renderOpaqueObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        virtual void renderTranslucentObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera );
        
        void renderStandardGeometry( Renderer *renderer, Geometry *geometry, ShaderProgram *program, Material *material, const Matrix4f &modelTransform );

    protected:
        inline ShaderProgram *getStandardProgram( void );

    private:
        ShaderProgram *_standardProgram = nullptr;
        
    public:
        void setLightingEnabled( bool enabled ) { _lightingEnabled = enabled; }
        bool isLightingEnabled( void ) const { return _lightingEnabled; }

    private:
        bool _lightingEnabled = true;

    public:
        void setShadowMappingEnabled( bool enabled ) { _shadowMapping = enabled; }
        bool isShadowMappingEnabled( void ) const { return _shadowMapping; }

    private:
        bool _shadowMapping = true;
        
        std::map< Light *, SharedPointer< ShadowMap >> _shadowMaps;
        
	};
    
}

#endif

