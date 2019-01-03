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

#ifndef CRIMILD_CORE_RENDER_GRAPH_PASSES_LIGHT_ACCUMULATION_PASS_
#define CRIMILD_CORE_RENDER_GRAPH_PASSES_LIGHT_ACCUMULATION_PASS_

#include "Rendering/RenderGraph/RenderGraphPass.hpp"
#include "Mathematics/Matrix.hpp"

namespace crimild {

	class Light;
	class Primitive;
	class UnlitShaderProgram;
	class ScreenColorShaderProgram;
	class PhongDiffuseShaderProgram;
	class PhongSpecularShaderProgram;

	namespace rendergraph {

		namespace passes {

			/**
			   \brief Accumulate lighting color using depth and normal information
			 */
			class LightAccumulationPass : public RenderGraphPass {
				CRIMILD_IMPLEMENT_RTTI( crimild::rendergraph::LightAccumulationPass )
				
			public:
				LightAccumulationPass( RenderGraph *graph, std::string name = "Light Accumulation" );
				virtual ~LightAccumulationPass( void );

				void setDepthInput( RenderGraphAttachment *attachment ) { _depthInput = attachment; }
				RenderGraphAttachment *getDepthInput( void ) { return _depthInput; }

				void setNormalInput( RenderGraphAttachment *attachment ) { _normalInput = attachment; }
				RenderGraphAttachment *getNormalInput( void ) { return _normalInput; }

				void setAmbientOutput( RenderGraphAttachment *attachment ) { _ambientOutput = attachment; }
				RenderGraphAttachment *getAmbientOutput( void ) { return _ambientOutput; }
				
				void setDiffuseOutput( RenderGraphAttachment *attachment ) { _diffuseOutput = attachment; }
				RenderGraphAttachment *getDiffuseOutput( void ) { return _diffuseOutput; }
				
				void setSpecularOutput( RenderGraphAttachment *attachment ) { _specularOutput = attachment; }
				RenderGraphAttachment *getSpecularOutput( void ) { return _specularOutput; }
				
				virtual void setup( rendergraph::RenderGraph *graph ) override;
				virtual void execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue ) override;

			private:
				void accumAmbient( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue );
				void accumDiffuse( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue );
				void accumSpecular( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue );
				
				void renderPointLight( Renderer *renderer, Light *light, const Matrix4f &pMatrix, const Matrix4f &vMatrix );

			private:
				SharedPointer< ScreenColorShaderProgram > _ambientLightProgram;
				SharedPointer< PhongDiffuseShaderProgram > _directionalLightProgram;
				SharedPointer< PhongSpecularShaderProgram > _directionalSpecularProgram;
				SharedPointer< UnlitShaderProgram > _pointLightProgram;			  
				SharedPointer< Primitive > _pointLightShape;
				
				RenderGraphAttachment *_depthInput = nullptr;
				RenderGraphAttachment *_normalInput = nullptr;
				RenderGraphAttachment *_ambientOutput = nullptr;
				RenderGraphAttachment *_diffuseOutput = nullptr;
				RenderGraphAttachment *_specularOutput = nullptr;
			};
		}

	}

}

#endif

