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

#ifndef CRIMILD_CORE_RENDER_GRAPH_PASSES_FORWARD_LIGTHING_
#define CRIMILD_CORE_RENDER_GRAPH_PASSES_FORWARD_LIGTHING_

#include "Rendering/RenderGraph/RenderGraphPass.hpp"
#include "Rendering/RenderQueue.hpp"

namespace crimild {

	class ForwardShadingShaderProgram;

	namespace rendergraph {

		namespace passes {

			/**
			   \brief Computing lighting for renderables 

			   This also serves as a render pass for objects that cannot be
			   rendered on other passes.

			   \remarks Try to avoid this pass, since it's probably not the
			   fastest way to render objects. Other passes are more optimized.
			 */
			class ForwardLightingPass : public RenderGraphPass {
				CRIMILD_IMPLEMENT_RTTI( crimild::rendergraph::ForwardLightingPass )

			private:
				using RenderableTypeArray = containers::Array< RenderQueue::RenderableType >;
				
			public:
				ForwardLightingPass( RenderGraph *graph, crimild::Size maxLights = 10 );
				ForwardLightingPass( RenderGraph *graph, RenderableTypeArray const &renderableTypes, crimild::Size maxLights = 10 );
				virtual ~ForwardLightingPass( void );
				
				void setDepthInput( RenderGraphAttachment *attachment ) { _depthInput = attachment; }
				RenderGraphAttachment *getDepthInput( void ) { return _depthInput; }

				void setShadowInput( RenderGraphAttachment *attachment ) { _shadowInput = attachment; }
				RenderGraphAttachment *getShadowInput( void ) { return _shadowInput; }				
				
				void setColorOutput( RenderGraphAttachment *attachment ) { _colorOutput = attachment; }
				RenderGraphAttachment *getColorOutput( void ) { return _colorOutput; }
				
				virtual void setup( rendergraph::RenderGraph *graph ) override;
				virtual void execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue ) override;

			private:
                void render( Renderer *renderer, RenderQueue *renderQueue, RenderQueue::RenderableType renderableType );
                void renderInstanced( Renderer *renderer, RenderQueue *renderQueue, RenderQueue::RenderableType renderableType );

			private:
				RenderableTypeArray _renderableTypes;
				SharedPointer< ShaderProgram > _program;
				crimild::Int8 _clearFlags;
				SharedPointer< DepthState > _depthState;
				
				RenderGraphAttachment *_depthInput = nullptr;
				RenderGraphAttachment *_shadowInput = nullptr;
				RenderGraphAttachment *_colorOutput = nullptr;
			};

		}

	}

}

#endif
