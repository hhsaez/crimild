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

#ifndef CRIMILD_RENDERING_SHADER_GRAPH_VARIABLE_
#define CRIMILD_RENDERING_SHADER_GRAPH_VARIABLE_

#include "ShaderGraphNode.hpp"

namespace crimild {

	namespace shadergraph {

		class Variable : public ShaderGraphNode {
			CRIMILD_IMPLEMENT_RTTI( crimild::shadergraph::Variable )
			
		public:
			enum class Storage {
				DEFAULT,
				INPUT,
				OUTPUT,
				UNIFORM,
				CONSTANT,
			};

			enum class Precision {
				HIGH,
				MEDIUM,
				LOW,
			};
			
			enum class Type {
				ANY,
				SCALAR,
				VECTOR_2,
				VECTOR_3,
				VECTOR_4,
				MATRIX_3,
				MATRIX_4,
				SAMPLER_2D,
				SAMPLER_CUBE_MAP,
			};

		public:
			explicit Variable( ShaderGraph *, Type type, std::string name = "" );
			explicit Variable( ShaderGraph *, Storage storage, Type type, std::string name = "" );
			virtual ~Variable( void );

			virtual ShaderGraphNode::NodeType getNodeType( void ) const override { return ShaderGraphNode::NodeType::VARIABLE; }

			Type getType( void ) const { return _type; }
			Storage getStorage( void ) const { return _storage; }

			Variable *setVariableName( std::string name ) { setName( name ); return this; }

			Variable *setLayoutLocation( crimild::Int16 location ) { _layoutLocation = location; return this; }
			crimild::Int16 getLayoutLocation( void ) const { return _layoutLocation; }

		private:
			Type _type;
			Storage _storage;
			crimild::Int16 _layoutLocation = -1;
		};

	}

}

#endif

