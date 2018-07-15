/*
 * Copyright (c) 2013-2018, Hernan Saez
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

#ifndef CRIMILD_RENDERING_SHADER_GRAPH_NODES_VERTEX_SHADER_INPUT_
#define CRIMILD_RENDERING_SHADER_GRAPH_NODES_VERTEX_SHADER_INPUT_

#include "Rendering/ShaderGraph/Node.hpp"

namespace crimild {

	namespace shadergraph {

		namespace nodes {

			class VertexShaderInput : public Node {
				CRIMILD_IMPLEMENT_RTTI( crimild::shadergraph::nodes::VertexShaderInput )

			public:
				VertexShaderInput( void );
				virtual ~VertexShaderInput( void );

				// attributes
				Outlet *getPosition( void ) { return _position; }
				Outlet *getNormal( void ) { return _normal; }
				Outlet *getUV( void ) { return _uv; }
				Outlet *getColor( void ) { return _color; }

				// uniforms
				Outlet *getMMatrix( void ) { return _mMatrix; }
				Outlet *getVMatrix( void ) { return _vMatrix; }
				Outlet *getPMatrix( void ) { return _pMatrix; }
				Outlet *getMVMatrix( void ) { return _mvMatrix; }
				Outlet *getMVPMatrix( void ) { return _mvpMatrix; }

			private:
				Outlet *_position = nullptr;
				Outlet *_normal = nullptr;
				Outlet *_uv = nullptr;
				Outlet *_color = nullptr;

				Outlet *_mMatrix = nullptr;
				Outlet *_vMatrix = nullptr;
				Outlet *_pMatrix = nullptr;
				Outlet *_mvMatrix = nullptr;
				Outlet *_mvpMatrix = nullptr;

			public:
				virtual void prepare( ShaderGraph *graph, ShaderProgram *program ) override;
			};

		}

	}

}

#endif

