/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#ifndef CRIMILD_RENDERING_SHADER_GRAPH_NODES_VERTEX_SHADER_INPUTS_
#define CRIMILD_RENDERING_SHADER_GRAPH_NODES_VERTEX_SHADER_INPUTS_

#include "Rendering/ShaderGraph/ShaderGraphOperation.hpp"

namespace crimild {

	namespace shadergraph {

		class Variable;

		class VertexShaderInputs : public ShaderGraphOperation {
			CRIMILD_IMPLEMENT_RTTI( crimild::shadergraph::VertexShaderInputs )
				
		public:
			VertexShaderInputs( ShaderGraph *graph );
			virtual ~VertexShaderInputs( void );
			
			Variable *getPositionAttribute( void ) { return _positionAttribute; }
			Variable *getNormalAttribute( void ) { return _normalAttribute; }
			Variable *getUVAttribute( void ) { return _uvAttribute; }
			Variable *getColorAttribute( void ) { return _colorAttribute; }
			
			Variable *getModelMatrixUniform( void ) { return _modelMatrixUniform; }
			Variable *getViewMatrixUniform( void ) { return _viewMatrixUniform; }
			Variable *getProjectionMatrixUniform( void ) { return _projectionMatrixUniform; }

			Variable *getModelPosition( void ) const { return _modelPosition; }
			Variable *getWorldPosition( void ) const { return _worldPosition; }
			Variable *getViewPosition( void ) const { return _viewPosition; }
			Variable *getProjectedPosition( void ) const { return _projectedPosition; }
			
		private:
			Variable *_positionAttribute = nullptr;
			Variable *_normalAttribute = nullptr;
			Variable *_uvAttribute = nullptr;
			Variable *_colorAttribute = nullptr;
			
			Variable *_modelMatrixUniform = nullptr;
			Variable *_viewMatrixUniform = nullptr;
			Variable *_projectionMatrixUniform = nullptr;
			
			Variable *_modelPosition = nullptr;
			Variable *_worldPosition = nullptr;
			Variable *_viewPosition = nullptr;
			Variable *_projectedPosition = nullptr;

		public:
			virtual void setup( ShaderGraph *graph ) override;
		};

		using StandardVertexInputs = VertexShaderInputs;
		
	}

}

#endif

