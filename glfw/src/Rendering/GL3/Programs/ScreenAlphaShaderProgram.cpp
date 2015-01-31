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

#include "ScreenAlphaShaderProgram.hpp"

#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *screen_alpha_vs = { CRIMILD_TO_STRING( 
	in vec3 aPosition;
	in vec2 aTextureCoord;

	uniform mat4 uMMatrix;

	out vec2 vTextureCoord;

	void main()
	{
		vTextureCoord = aTextureCoord;
		gl_Position = uMMatrix * vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
	}
)};

const char *screen_alpha_fs = { CRIMILD_TO_STRING( 
	in vec2 vTextureCoord;

	uniform sampler2D uColorMap;

	out vec4 vFragColor;

	void main( void ) 
	{ 
		vec4 color = texture( uColorMap, vTextureCoord );
		vFragColor = vec4( vec3( color.a ), 1.0 );
	}
)};

ScreenAlphaShaderProgram::ScreenAlphaShaderProgram( void )
	: ShaderProgram( Utils::getVertexShaderInstance( screen_alpha_vs ), Utils::getFragmentShaderInstance( screen_alpha_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM, "uMMatrix" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM, "uColorMap" );
}

ScreenAlphaShaderProgram::~ScreenAlphaShaderProgram( void )
{ 
}

