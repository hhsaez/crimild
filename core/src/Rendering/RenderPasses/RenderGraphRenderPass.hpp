/*
 * Copyright (c) 2013-present, H. Hernan Saez
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

#ifndef CRIMILD_RENDERER_RENDER_PASS_RENDER_GRAPH_
#define CRIMILD_RENDERER_RENDER_PASS_RENDER_GRAPH_

#include "RenderPass.hpp"

namespace crimild {

	class ShaderProgram;

	namespace rendergraph {

		class RenderGraph;

		/**
		   \brief A helper render pass that works with render graphs

		   \deprecated This will be removed, since it's just a helper
		 */
		class RenderGraphRenderPass : public RenderPass {
		public:
			RenderGraphRenderPass( SharedPointer< rendergraph::RenderGraph > const &renderGraph );
			virtual ~RenderGraphRenderPass( void );

			virtual void render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera ) override;
			
		private:
			SharedPointer< rendergraph::RenderGraph > _renderGraph;
			SharedPointer< ShaderProgram > _program;			
		};

	}

}

#endif

