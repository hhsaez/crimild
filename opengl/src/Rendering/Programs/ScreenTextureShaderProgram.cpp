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

#include "ScreenTextureShaderProgram.hpp"
#include "Rendering/OpenGLUtils.hpp"

using namespace crimild;
using namespace crimild::opengl;

const char *screen_texture_vs = { CRIMILD_TO_STRING(
		layout ( location = 0 ) CRIMILD_GLSL_ATTRIBUTE vec3 aPosition;
		layout ( location = 4 ) CRIMILD_GLSL_ATTRIBUTE vec2 aTextureCoord;

	CRIMILD_GLSL_VARYING_OUT vec2 vTextureCoord;

	void main()
	{
		vTextureCoord = aTextureCoord;
		CRIMILD_GLSL_VERTEX_OUTPUT = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
	}
)};

const char *screen_texture_fs = { CRIMILD_TO_STRING(
    CRIMILD_GLSL_PRECISION_FLOAT_HIGH
                                                    
    CRIMILD_GLSL_VARYING_IN vec2 vTextureCoord;

    uniform sampler2D uColorMap;

    CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

    void main( void ) 
    {
        vec4 color = CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord );
        CRIMILD_GLSL_FRAGMENT_OUTPUT = color;
    }
)};

ScreenTextureShaderProgram::ScreenTextureShaderProgram( void )
	: ShaderProgram( OpenGLUtils::getVertexShaderInstance( screen_texture_vs ), OpenGLUtils::getFragmentShaderInstance( screen_texture_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM, "uColorMap" );
}

ScreenTextureShaderProgram::~ScreenTextureShaderProgram( void )
{ 
}

