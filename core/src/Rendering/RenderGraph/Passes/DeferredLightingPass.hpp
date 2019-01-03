/*
 * Copyright (c) 2002-present, H. Hernán Saez
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

#ifndef CRIMILD_CORE_RENDER_GRAPH_PASSES_PHONG_OPAQUE_
#define CRIMILD_CORE_RENDER_GRAPH_PASSES_PHONG_OPAQUE_

#include "Rendering/RenderGraph/RenderGraphPass.hpp"

namespace crimild {

	class DepthState;
	class PhongDeferredLightingShaderProgram;

	namespace rendergraph {

		namespace passes {

			/**
			   \brief Render opaque objects based on a deferred version of the Phong lighting model

			   \todo Handle skinned meshes, textured, diffuse-only separatelly
			 */
			class DeferredLightingPass : public RenderGraphPass {
				CRIMILD_IMPLEMENT_RTTI( crimild::rendergraph::DeferredLightingPass )
				
			public:
				DeferredLightingPass( RenderGraph *graph, std::string name = "Deferred Lighting" );
				virtual ~DeferredLightingPass( void );

				void setDepthInput( RenderGraphAttachment *attachment ) { _depthInput = attachment; }
				RenderGraphAttachment *getDepthInput( void ) { return _depthInput; }

				void setAmbientAccumInput( RenderGraphAttachment *attachment ) { _ambientAccumInput = attachment; }
				RenderGraphAttachment *getAmbientAccumInput( void ) { return _ambientAccumInput; }
				
				void setDiffuseAccumInput( RenderGraphAttachment *attachment ) { _diffuseAccumInput = attachment; }
				RenderGraphAttachment *getDiffuseAccumInput( void ) { return _diffuseAccumInput; }
				
				void setSpecularAccumInput( RenderGraphAttachment *attachment ) { _specularAccumInput = attachment; }
				RenderGraphAttachment *getSpecularAccumInput( void ) { return _specularAccumInput; }
				
				void setColorOutput( RenderGraphAttachment *attachment ) { _colorOutput = attachment; }
				RenderGraphAttachment *getColorOutput( void ) { return _colorOutput; }
				
				virtual void setup( rendergraph::RenderGraph *graph ) override;
				virtual void execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue ) override;

			private:
				SharedPointer< PhongDeferredLightingShaderProgram > _program;
				crimild::Int8 _clearFlags;
				SharedPointer< DepthState > _depthState;
				
				RenderGraphAttachment *_depthInput = nullptr;
				RenderGraphAttachment *_ambientAccumInput = nullptr;
				RenderGraphAttachment *_diffuseAccumInput = nullptr;
				RenderGraphAttachment *_specularAccumInput = nullptr;
				RenderGraphAttachment *_colorOutput = nullptr;
			};
		}

	}

}

#endif

