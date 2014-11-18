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

#include "SignedDistanceFieldShaderProgram.hpp"
#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *sdf_vs = { CRIMILD_TO_STRING(
	in vec3 aPosition;
	in vec2 aTextureCoord;

	uniform mat4 uPMatrix; 
	uniform mat4 uVMatrix; 
	uniform mat4 uMMatrix;

	out vec2 vTextureCoord;
	out float vOneU;
	out float vOneV;

	void main()
	{
		vTextureCoord = aTextureCoord;
		vOneU = 1.0 / 1024.0;
		vOneV = 1.0 / 1024.0;
		gl_Position = uPMatrix * uVMatrix * uMMatrix * vec4( aPosition, 1.0 );
	}
)};

const char *sdf_fs = { CRIMILD_TO_STRING( 
	struct Material {
	    vec4 ambient;
	    vec4 diffuse;
	    vec4 specular;
	    float shininess;
	};

	in vec2 vTextureCoord;
	in float vOneU;
	in float vOneV;

	uniform sampler2D uColorMap;
	uniform Material uMaterial; 

	out vec4 vFragColor;

	float aastep( float threshold, float value ) {
	  float afwidth = 0.7 * length( vec2( dFdx( value ), dFdy( value ) ) );
	  return smoothstep( threshold - afwidth, threshold + afwidth, value);
	}

	void main( void )
	{
		vec2 uv = vTextureCoord * vec2( 1024.0, 1024.0 );

		vec2 uv00 = floor( uv - vec2( 0.5 ) );
		vec2 uvlerp = uv - uv00 - vec2( 0.5 );
		vec2 st00 = ( uv00 + vec2( 0.5 ) ) * vec2( vOneU, vOneV );

		vec4 D00 = texture( uColorMap, st00 );
		vec4 D10 = texture( uColorMap, st00 + vec2( vOneU, 0.0 ) );
		vec4 D01 = texture( uColorMap, st00 + vec2( 0.0, vOneV ) );
		vec4 D11 = texture( uColorMap, st00 + vec2( vOneU, vOneV ) );

		vec2 D00_10 = vec2( D00.r, D10.r ) * 255.0 - 128.0 + vec2( D00.g, D10.g ) * ( 255.0 / 256.0 );
		vec2 D01_11 = vec2( D01.r, D11.r ) * 255.0 - 128.0 + vec2( D01.g, D11.g ) * ( 255.0 / 256.0 );

		vec2 D0_1 = mix( D00_10, D01_11, uvlerp.y );
		float D = mix( D0_1.x, D0_1.y, uvlerp.x );

		float g = 1.0 - aastep( 0.0, D );
		if ( g < 0.001 ) {
			discard;
		}

		vFragColor = vec4( uMaterial.diffuse.rgb, uMaterial.diffuse.a * g );
	}
)};

gl3::SignedDistanceFieldShaderProgram::SignedDistanceFieldShaderProgram( bool ignored )
	: ShaderProgram( gl3::Utils::getVertexShaderInstance( sdf_vs ), gl3::Utils::getFragmentShaderInstance( sdf_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM, "uPMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM, "uVMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM, "uMMatrix" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM, "uColorMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_AMBIENT_UNIFORM, "uMaterial.ambient" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM, "uMaterial.diffuse" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_SPECULAR_UNIFORM, "uMaterial.specular" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_SHININESS_UNIFORM, "uMaterial.shininess" );
}

gl3::SignedDistanceFieldShaderProgram::~SignedDistanceFieldShaderProgram( void )
{ 
}

