/*
 * Copyright (c) 2013, Hernan Saez
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

#include "FlatShaderProgram.hpp"
#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *flat_vs = { CRIMILD_TO_STRING(
	in vec3 aPosition;

	uniform mat4 uPMatrix; 
	uniform mat4 uVMatrix; 
	uniform mat4 uMMatrix;

	void main()
	{
		gl_Position = uPMatrix * uVMatrix * uMMatrix * vec4(aPosition, 1.0); 
	}
)};

const char *flat_fs = { CRIMILD_TO_STRING( 
	struct Material {
		vec4 diffuse;
	};

	uniform Material uMaterial; 

	out vec4 vFragColor;

	void main( void ) 
	{ 
		vFragColor = uMaterial.diffuse; 
	}
)};

FlatShaderProgram::FlatShaderProgram( void )
	: ShaderProgram( Utils::getVertexShaderInstance( flat_vs ), Utils::getFragmentShaderInstance( flat_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM, "uPMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM, "uVMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM, "uMMatrix" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM, "uMaterial.diffuse" );
}

FlatShaderProgram::~FlatShaderProgram( void )
{ 
}

