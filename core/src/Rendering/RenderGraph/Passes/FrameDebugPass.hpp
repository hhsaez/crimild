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

#ifndef CRIMILD_CORE_RENDER_GRAPH_FRAME_DEBUG_BLEND_
#define CRIMILD_CORE_RENDER_GRAPH_FRAME_DEBUG_BLEND_

#include "Rendering/RenderGraph/RenderGraphPass.hpp"

namespace crimild {

	class Texture;
	class ShaderProgram;

	namespace rendergraph {

		namespace passes {

			/**
			   \brief Display frame components in different layouts
			 */
			class FrameDebugPass : public RenderGraphPass {
				CRIMILD_IMPLEMENT_RTTI( crimild::rendergraph::FrameDebugPass )
				
			public:
				FrameDebugPass( RenderGraph *graph, std::string name = "Frame Debug" );
				virtual ~FrameDebugPass( void );
			
				void addInput( RenderGraphAttachment *input )
				{
					_inputs.add( input );
				}
				
				void setOutput( RenderGraphAttachment *attachment ) { _output = attachment; }
				RenderGraphAttachment *getOutput( void ) { return _output; }
				
				virtual void setup( rendergraph::RenderGraph *graph ) override;
				virtual void execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue ) override;

			private:
				void render( Renderer *renderer, Texture *texture );

			private:
				Array< RenderGraphAttachment * > _inputs;
				
				RenderGraphAttachment *_output = nullptr;

				SharedPointer< ShaderProgram > _program;
			};
		}

	}

}

#endif

