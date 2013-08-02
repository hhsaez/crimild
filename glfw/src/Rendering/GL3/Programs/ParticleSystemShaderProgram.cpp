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

#include "ParticleSystemShaderProgram.hpp"
#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *particle_system_vs = { CRIMILD_TO_STRING(
	struct Shape {
		vec3 center;
		float radius;
	};

	in vec3 aPosition;
	in vec3 aNormal;   // used for initial velocity
	in vec3 aTextureCoord;   // tc0 = lifeTime, tc1 = unused

	uniform mat4 uPMatrix;
	uniform mat4 uVMatrix;
	uniform mat4 uMMatrix;

	uniform float uTime;
	uniform float uLifeTime;
	uniform vec3 uGravity;
	uniform Shape uShape;

	out float vParticleTime;

	void main(void) {
	    float size = aTextureCoord[ 0 ];
	    float offset = aTextureCoord[ 1 ];

	    vParticleTime = uTime + offset;
	    vParticleTime = vParticleTime - uLifeTime * floor( vParticleTime / uLifeTime );
	    
	    vec4 worldGravity = inverse( uMMatrix ) * vec4( uGravity, 0.0 );
	    vec3 position = aPosition + aNormal * vParticleTime + 0.5 * vParticleTime * vParticleTime * worldGravity.xyz;

	    vec4 worldPosition = uMMatrix * vec4( position, 1.0 );
	    vec4 worldGeneratorPosition = uMMatrix * vec4( aPosition, 1.0 );
	    vec4 worldVelocity = uMMatrix * vec4( aNormal, 0.0 );
	    if ( uShape.radius > 0.0 ) {
		    vec3 diff = worldPosition.xyz - uShape.center;
		    float d = distance( worldPosition.xyz , uShape.center );
		    if ( d < uShape.radius ) {
		    	worldPosition.xyz = uShape.center + uShape.radius * normalize( diff );
		    }
		}

	    gl_Position = uPMatrix * uVMatrix * worldPosition;
	    gl_PointSize = ( uLifeTime - vParticleTime ) * size;
	}
)};

const char *particle_system_fs = { CRIMILD_TO_STRING( 
	struct Material {
	    vec4 ambient;
	    vec4 diffuse;
	    vec4 specular;
	    float shininess;
	};

	in float vParticleTime;

	uniform Material uMaterial; 
	uniform sampler2D uColorMap;
	uniform bool uUseColorMap;

	uniform float uLifeTime;

	out vec4 vFragColor;

	void main( void ) 
	{ 
    	vec4 color = uUseColorMap ? texture( uColorMap, gl_PointCoord ) : vec4( 1.0, 1.0, 1.0, 1.0 );
    	color *= uMaterial.diffuse;
    	color.a *= 1.0 - ( vParticleTime / uLifeTime );
    	if ( color.a == 0.0 || color.r + color.g + color.b == 0.0 ) {
    		discard;
    	}

		vFragColor = color; 
	}
)};

ParticleSystemShaderProgram::ParticleSystemShaderProgram( void )
	: ShaderProgram( gl3::Utils::getVertexShaderInstance( particle_system_vs ), gl3::Utils::getFragmentShaderInstance( particle_system_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::NORMAL_ATTRIBUTE, "aNormal" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM, "uPMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM, "uVMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM, "uMMatrix" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_AMBIENT_UNIFORM, "uMaterial.ambient" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM, "uMaterial.diffuse" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_SPECULAR_UNIFORM, "uMaterial.specular" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_SHININESS_UNIFORM, "uMaterial.shininess" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM, "uColorMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_USE_COLOR_MAP_UNIFORM, "uUseColorMap" );
}

ParticleSystemShaderProgram::~ParticleSystemShaderProgram( void )
{ 
}

