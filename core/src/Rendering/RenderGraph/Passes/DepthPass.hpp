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

#ifndef CRIMILD_CORE_RENDER_GRAPH_PASSES_DEPTH_
#define CRIMILD_CORE_RENDER_GRAPH_PASSES_DEPTH_

#include "Rendering/RenderGraph/RenderGraphPass.hpp"
#include "Rendering/RenderQueue.hpp"

namespace crimild {

	namespace rendergraph {

		namespace passes {

			/**
			   \brief Render a depth buffer based on objects position

			   It also renders a normal buffer for deferred 
			   lighting purporses.

			   \todo Add support for normal mapping
			   \todo Use a flag for Roughness/Shininess on alpha-channel
			 */
			class DepthPass : public RenderGraphPass {
				CRIMILD_IMPLEMENT_RTTI( crimild::rendergraph::DepthPass )
				
			public:
				DepthPass( RenderGraph *graph );
				virtual ~DepthPass( void );
			
				void setDepthOutput( RenderGraphAttachment *attachment ) { _depthOutput = attachment; }
				RenderGraphAttachment *getDepthOutput( void ) { return _depthOutput; }
				
				void setNormalOutput( RenderGraphAttachment *attachment ) { _normalOutput = attachment; }
				RenderGraphAttachment *getNormalOutput( void ) { return _normalOutput; }

				virtual void setup( rendergraph::RenderGraph *graph ) override;
				virtual void execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue ) override;

			private:
				void renderObjects( Renderer *renderer, RenderQueue *renderQueue, RenderQueue::RenderableType renderableType, ShaderProgram *program );

			private:
				SharedPointer< ShaderProgram > _program;
                SharedPointer< ShaderProgram > _programInstanced;
			
				RenderGraphAttachment *_depthOutput = nullptr;
				RenderGraphAttachment *_normalOutput = nullptr;
			};
		}

	}

}

#endif

