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

#include "SepiaToneShaderProgram.hpp"
#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *sepia_vs = { CRIMILD_TO_STRING( 
	in vec3 aPosition;
	in vec2 aTextureCoord;

	out vec2 vTextureCoord;

	void main()
	{
		vTextureCoord = aTextureCoord;
		gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
	}
)};

const char *sepia_fs = { CRIMILD_TO_STRING( 
	in vec2 vTextureCoord;

	uniform sampler2D uColorMap;

	out vec4 vFragColor;

	void main( void ) 
	{ 
		float uSepiaValue = 1.0;

		vec4 color = texture( uColorMap, vTextureCoord );

	    if ( uSepiaValue > 0.0 ) {
	        float gray = ( color.r + color.g + color.b ) / 3.0;
	        vec3 grayscale = vec3( gray, gray, gray );
	        vec4 sepia = vec4( 0.4392156863, 0.2588235294, 0.07843137255, 1.0 );
	        color = vec4( ( gray < 0.5 ? 2.0 * sepia.r * grayscale.x : 1.0 - 2.0 * ( 1.0 - grayscale.x ) * ( 1.0 - sepia.r ) ),
	                       ( gray < 0.5 ? 2.0 * sepia.g * grayscale.y : 1.0 - 2.0 * ( 1.0 - grayscale.y ) * ( 1.0 - sepia.g ) ),
	                       ( gray < 0.5 ? 2.0 * sepia.b * grayscale.z : 1.0 - 2.0 * ( 1.0 - grayscale.z ) * ( 1.0 - sepia.b ) ),
	                       1.0 );
	        color.rgb = grayscale + uSepiaValue * ( color.rgb - grayscale );
	    }

		vFragColor = color;
	}
)};

SepiaToneShaderProgram::SepiaToneShaderProgram( void )
	: ShaderProgram( Utils::getVertexShaderInstance( sepia_vs ), Utils::getFragmentShaderInstance( sepia_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM, "uColorMap" );
}

SepiaToneShaderProgram::~SepiaToneShaderProgram( void )
{ 
}

