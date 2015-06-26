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

#include "ForwardRenderShaderProgram.hpp"
#include "Rendering/Utils.hpp"

using namespace crimild;

const char *forward_vs = { CRIMILD_TO_STRING(
    // The scale matrix is used to push the projected vertex into the 0.0 - 1.0 region.
    const mat4 ScaleMatrix = mat4( 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0 );

    attribute vec3 aPosition;
    attribute vec3 aNormal;
    attribute vec3 aTangent;
    attribute vec2 aTextureCoord;

    uniform mat4 uPMatrix;
    uniform mat4 uVMatrix;
    uniform mat4 uMMatrix;
    uniform mat4 uNMatrix;
    uniform mat4 uLightSourceProjectionMatrix;
    uniform mat4 uLightSourceViewMatrix;

    uniform bool uUseNormalMap;

    varying vec4 vWorldVertex;
    varying vec3 vWorldNormal;
    varying vec3 vWorldTangent;
    varying vec3 vWorldBiTangent;
    varying vec2 vTextureCoord;
    varying vec3 vViewVec;
    varying vec4 vPosition;

    void main( void )
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
         
         vPosition = ScaleMatrix * uLightSourceProjectionMatrix * uLightSourceViewMatrix * vWorldVertex;
     }
 )};

const char *forward_fs = { CRIMILD_TO_STRING(
    precision highp float;
                                             
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

    varying vec4 vWorldVertex;
    varying vec3 vWorldNormal;
    varying vec3 vWorldTangent;
    varying vec3 vWorldBiTangent;
    varying vec3 vViewVec;
    varying vec2 vTextureCoord;
    varying vec4 vPosition;

    uniform int uLightCount;
    uniform Light uLights[ 4 ];
    uniform Material uMaterial;

    uniform sampler2D uColorMap;
    uniform bool uUseColorMap;
    uniform sampler2D uNormalMap;
    uniform bool uUseNormalMap;
    uniform sampler2D uSpecularMap;
    uniform bool uUseSpecularMap;
    uniform sampler2D uEmissiveMap;
    uniform bool uUseEmissiveMap;
    uniform sampler2D uShadowMap;
    uniform bool uUseShadowMap;

    uniform float uLinearDepthConstant;
                                             
    float unpack( vec4 color )
    {
        const vec4 bitShifts = vec4(1.0,
                                    1.0 / 255.0,
                                    1.0 / (255.0 * 255.0),
                                    1.0 / (255.0 * 255.0 * 255.0));
        return dot( color, bitShifts );
    }
     
    void main( void )
    {
        // vWorldNormal gets interpolated when passed to the fragment shader
        // we need to re-normalize it again
        vec3 normal = normalize( vWorldNormal );
        
    	vec4 color = uUseColorMap ? texture2D( uColorMap, vTextureCoord ) : vec4( 1.0, 1.0, 1.0, 1.0 );
    	color *= uMaterial.diffuse;
    	if ( color.a == 0.0 ) {
    		discard;
    	}
        
    	vec4 specularColor = uUseSpecularMap ? texture2D( uSpecularMap, vTextureCoord ) : vec4( 1.0, 1.0, 1.0, 1.0 );
    	specularColor *= uMaterial.specular;
        
    	vec4 emissiveColor = uUseEmissiveMap ? texture2D( uEmissiveMap, vTextureCoord ) : vec4( 0.0, 0.0, 0.0, 1.0 );
        
    	gl_FragColor.rgb = uMaterial.ambient.rgb + emissiveColor.rgb;
    	gl_FragColor.a = color.a;
        
        for ( int i = 0; i < 4; i++ ) {
            if ( i >= uLightCount ) {
                break;
            }
            
            vec3 lightVec = normalize( uLights[ i ].position - vWorldVertex.xyz );
            vec3 halfVector = -normalize( reflect( lightVec, vWorldNormal ) );
            
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
                
                normal = 2.0 * texture2D( uNormalMap, vTextureCoord ).xyz - 1.0;
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
                
                gl_FragColor.rgb += ( ( color.rgb * l ) + ( specularColor.rgb * s ) ) * uLights[ i ].color.rgb * a * spotlight;
            }
        }
        
        if ( uUseShadowMap ) {
            vec3 depth = vPosition.xyz / vPosition.w;
            depth.z = length( vWorldVertex.xyz - uLights[ 0 ].position ) * uLinearDepthConstant;
            float shadow = 1.0;
            //depth.z *= 0.99;
            vec4 shadowColor = texture2D( uShadowMap, depth.xy );
            float shadowDepth = unpack( shadowColor );
            if ( depth.z > shadowDepth ) {
                shadow = 0.5;
            }
            
            gl_FragColor = clamp( vec4( gl_FragColor.rgb * shadow, gl_FragColor.a ), 0.0, 1.0 );
            gl_FragColor.a = 1.0;
        }
    }
)};

gles::ForwardRenderShaderProgram::ForwardRenderShaderProgram( void )
    : ShaderProgram( new VertexShader( forward_vs ), new FragmentShader( forward_fs ) )
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
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_EMISSIVE_MAP_UNIFORM, "uEmissiveMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_USE_EMISSIVE_MAP_UNIFORM, "uUseEmissiveMap" );
    
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_COUNT_UNIFORM, "uLightCount" );
	for ( int i = 0; i < 4; i++ ) {
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "position" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_ATTENUATION_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "attenuation" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_DIRECTION_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "direction" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_COLOR_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "color" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_OUTER_CUTOFF_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "outerCutoff" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_INNER_CUTOFF_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "innerCutoff" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_EXPONENT_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "exponent" ) );
	}
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_SOURCE_PROJECTION_MATRIX_UNIFORM, "uLightSourceProjectionMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_SOURCE_VIEW_MATRIX_UNIFORM, "uLightSourceViewMatrix" );
    
    registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::SHADOW_MAP_UNIFORM, "uShadowMap" );
    registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::USE_SHADOW_MAP_UNIFORM, "uUseShadowMap" );
    
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LINEAR_DEPTH_CONSTANT_UNIFORM, "uLinearDepthConstant" );
}

gles::ForwardRenderShaderProgram::~ForwardRenderShaderProgram( void )
{ 
}

