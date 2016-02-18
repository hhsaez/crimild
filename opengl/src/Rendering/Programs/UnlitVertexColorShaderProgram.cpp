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

#include "UnlitVertexColorShaderProgram.hpp"

#include "Rendering/OpenGLUtils.hpp"

using namespace crimild;
using namespace crimild::opengl;

const char *unlit_vertex_color_vs = { CRIMILD_TO_STRING(
    CRIMILD_GLSL_ATTRIBUTE vec3 aPosition;
    CRIMILD_GLSL_ATTRIBUTE vec4 aColor;

	uniform mat4 uPMatrix; 
	uniform mat4 uVMatrix; 
	uniform mat4 uMMatrix;

	CRIMILD_GLSL_VARYING_OUT vec4 vColor;

	void main()
	{
		vColor = aColor;
		CRIMILD_GLSL_VERTEX_OUTPUT = uPMatrix * uVMatrix * uMMatrix * vec4(aPosition, 1.0);
	}
)};

const char *unlit_vertex_color_fs = { CRIMILD_TO_STRING(

    CRIMILD_GLSL_PRECISION_FLOAT_HIGH

	struct Material {
		vec4 diffuse;
	};

	uniform Material uMaterial;

	CRIMILD_GLSL_VARYING_IN vec4 vColor;

    CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

	void main( void ) 
	{
		CRIMILD_GLSL_FRAGMENT_OUTPUT = vColor * uMaterial.diffuse;
	}
)};

UnlitVertexColorShaderProgram::UnlitVertexColorShaderProgram( void )
	: ShaderProgram( OpenGLUtils::getVertexShaderInstance( unlit_vertex_color_vs ), OpenGLUtils::getFragmentShaderInstance( unlit_vertex_color_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::COLOR_ATTRIBUTE, "aColor" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM, "uPMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM, "uVMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM, "uMMatrix" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM, "uMaterial.diffuse" );
}

UnlitVertexColorShaderProgram::~UnlitVertexColorShaderProgram( void )
{ 

}

