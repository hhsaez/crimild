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

#include "LitTextureShaderProgram.hpp"
#include "Rendering/OpenGLUtils.hpp"

using namespace crimild;
using namespace crimild::opengl;

const char *lit_texture_vs = { CRIMILD_TO_STRING(
	in vec3 aPosition;
	in vec3 aNormal;
	in vec3 aTangent;
	in vec2 aTextureCoord;

	uniform mat4 uPMatrix;
	uniform mat4 uVMatrix;
	uniform mat4 uMMatrix;
	uniform mat4 uNMatrix;

	uniform bool uUseNormalMap;

	out vec4 vWorldVertex;
	out vec3 vWorldNormal;
	out vec3 vWorldTangent;
	out vec3 vWorldBiTangent;
	out vec2 vTextureCoord;
	out vec3 vViewVec;

	void main ()
	{
	    vWorldVertex = uMMatrix * vec4( aPosition, 1.0 );
	    vec4 viewVertex = uVMatrix * vWorldVertex;
	    gl_Position = uPMatrix * viewVertex;
	    vWorldNormal = normalize( mat3( uNMatrix ) * aNormal );
	    if ( uUseNormalMap ) {
	    	vWorldTangent = normalize( mat3( uNMatrix ) * aTangent );
	    	vWorldBiTangent = cross( vWorldNormal, vWorldTangent );
	    }

	    vViewVec = normalize( -viewVertex.xyz );
	    vTextureCoord = aTextureCoord;
	}
)};

const char *lit_texture_fs = { CRIMILD_TO_STRING( 
	struct Light {
	    vec3 position;
	    vec3 attenuation;
	    vec3 direction;
	    vec4 color;
	    float outerCutoff;
	    float innerCutoff;
	    float exponent;
	};

	struct Material {
	    vec4 ambient;
	    vec4 diffuse;
	    vec4 specular;
	    float shininess;
	};

	in vec4 vWorldVertex;
	in vec3 vWorldNormal;
	in vec3 vWorldTangent;
	in vec3 vWorldBiTangent;
	in vec2 vTextureCoord;
	in vec3 vViewVec;

	uniform int uLightCount;
	uniform Light uLights[ 4 ];
	uniform Material uMaterial;

	uniform sampler2D uColorMap;
	uniform bool uUseColorMap;
	uniform sampler2D uNormalMap;
	uniform bool uUseNormalMap;
	uniform sampler2D uSpecularMap;
	uniform bool uUseSpecularMap;

	out vec4 vFragColor;

	void main( void ) 
	{ 
    	vec4 color = uUseColorMap ? texture( uColorMap, vTextureCoord ) : vec4( 1.0, 1.0, 1.0, 1.0 );
    	color *= uMaterial.diffuse;
    	if ( color.a == 0.0 ) {
    		discard;
    	}

    	vec4 specularColor = uUseSpecularMap ? texture( uSpecularMap, vTextureCoord ) : vec4( 1.0, 1.0, 1.0, 1.0 );
    	specularColor *= uMaterial.specular;

    	vFragColor.rgb = uMaterial.ambient.rgb;
    	vFragColor.a = color.a;

        for ( int i = 0; i < 4; i++ ) {
            if ( i >= uLightCount ) {
                break;
            }
            
            vec3 lightVec = normalize( uLights[ i ].position - vWorldVertex.xyz );
            vec3 halfVector = -normalize( reflect( lightVec, vWorldNormal ) );
            vec3 normal = vWorldNormal;

            if ( uUseNormalMap ) {
            	vec3 temp;
            	vec3 lightDir = lightVec;

                temp.x = dot( lightVec, vWorldTangent );
                temp.y = dot( lightVec, vWorldBiTangent );
                temp.z = dot( lightVec, vWorldNormal );
                lightVec = normalize( temp );

                temp.x = dot( halfVector, vWorldTangent );
                temp.y = dot( halfVector, vWorldBiTangent );
                temp.z = dot( halfVector, vWorldNormal );
                halfVector = normalize( temp );

                normal = 2.0 * texture( uNormalMap, vTextureCoord ).xyz - 1.0;
                normal = normalize( normal );
            }

            float l = dot( normal, lightVec );
            if ( l > 0.0 ) {
                float spotlight = 1.0;
                if ( ( uLights[ i ].direction.x != 0.0 ) || ( uLights[ i ].direction.y != 0.0 ) || ( uLights[ i ].direction.z != 0.0 ) ) {
                    spotlight = max( -dot( lightVec, uLights[ i ].direction ), 0.0 );
                    float spotlightFade = clamp( ( uLights[ i ].outerCutoff - spotlight ) / ( uLights[ i ].outerCutoff - uLights[ i ].innerCutoff ), 0.0, 1.0 );
                    spotlight = pow( spotlight * spotlightFade, uLights[ i ].exponent );
                }

                float s = pow( max( dot( halfVector, uUseNormalMap ? normal : vViewVec ), 0.0 ), uMaterial.shininess );
                float d = distance( vWorldVertex.xyz, uLights[ i ].position );
                float a = 1.0 / ( uLights[ i ].attenuation.x + ( uLights[ i ].attenuation.y * d ) + ( uLights[ i ].attenuation.z * d * d ) );
                
                vFragColor.rgb += ( ( color.rgb * l ) + ( specularColor.rgb * s ) ) * uLights[ i ].color.rgb * a * spotlight;
            }
        }
	}
)};

LitTextureShaderProgram::LitTextureShaderProgram( void )
	: ShaderProgram( OpenGLUtils::getVertexShaderInstance( lit_texture_vs ), OpenGLUtils::getFragmentShaderInstance( lit_texture_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::NORMAL_ATTRIBUTE, "aNormal" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TANGENT_ATTRIBUTE, "aTangent" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM, "uPMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM, "uVMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM, "uMMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::NORMAL_MATRIX_UNIFORM, "uNMatrix" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_AMBIENT_UNIFORM, "uMaterial.ambient" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM, "uMaterial.diffuse" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_SPECULAR_UNIFORM, "uMaterial.specular" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_SHININESS_UNIFORM, "uMaterial.shininess" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM, "uColorMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_USE_COLOR_MAP_UNIFORM, "uUseColorMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_NORMAL_MAP_UNIFORM, "uNormalMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_USE_NORMAL_MAP_UNIFORM, "uUseNormalMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_SPECULAR_MAP_UNIFORM, "uSpecularMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_USE_SPECULAR_MAP_UNIFORM, "uUseSpecularMap" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_COUNT_UNIFORM, "uLightCount" );
	for ( int i = 0; i < 4; i++ ) {
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + i, OpenGLUtils::buildArrayShaderLocationName( "uLights", i, "position" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_ATTENUATION_UNIFORM + i, OpenGLUtils::buildArrayShaderLocationName( "uLights", i, "attenuation" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_DIRECTION_UNIFORM + i, OpenGLUtils::buildArrayShaderLocationName( "uLights", i, "direction" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_COLOR_UNIFORM + i, OpenGLUtils::buildArrayShaderLocationName( "uLights", i, "color" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_OUTER_CUTOFF_UNIFORM + i, OpenGLUtils::buildArrayShaderLocationName( "uLights", i, "outerCutoff" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_INNER_CUTOFF_UNIFORM + i, OpenGLUtils::buildArrayShaderLocationName( "uLights", i, "innerCutoff" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_EXPONENT_UNIFORM + i, OpenGLUtils::buildArrayShaderLocationName( "uLights", i, "exponent" ) );
	}	
}

LitTextureShaderProgram::~LitTextureShaderProgram( void )
{ 
}

