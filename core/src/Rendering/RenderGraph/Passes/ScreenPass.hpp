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

#ifndef CRIMILD_CORE_RENDER_GRAPH_PASSES_SCREEN_
#define CRIMILD_CORE_RENDER_GRAPH_PASSES_SCREEN_

#include "Rendering/RenderGraph/RenderGraphPass.hpp"
#include "Mathematics/Matrix.hpp"

namespace crimild {

	namespace rendergraph {

		namespace passes {

			/**
			   \brief Render objects projecting them to the screen

			   Uses an orthographic matrix to project objects onto the
			   screen. 

			   \remarks Objects should be scaled to fit the visible rect
			   [(-aspect, aspect), (1.0, -1.0)]

			   \todo We may split this into several other passes, including
			   depth pre-pass, opaque/transparent and custom shaders.
			 */
			class ScreenPass : public RenderGraphPass {
				CRIMILD_IMPLEMENT_RTTI( crimild::rendergraph::ScreenPass )
				
			public:
				ScreenPass( RenderGraph *graph );
				virtual ~ScreenPass( void );
			
				void setOutput( RenderGraphAttachment *attachment ) { _output = attachment; }
				RenderGraphAttachment *getOutput( void ) { return _output; }
				
				virtual void setup( rendergraph::RenderGraph *graph ) override;
				virtual void execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue ) override;

			private:
				RenderGraphAttachment *_depthOutput = nullptr;
				RenderGraphAttachment *_output = nullptr;
			};
		}

	}

}

#endif
