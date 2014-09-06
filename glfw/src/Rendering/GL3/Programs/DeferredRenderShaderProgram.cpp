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

#include "DeferredRenderShaderProgram.hpp"
#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *deferred_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;
    in vec3 aTangent;
    in vec3 aNormal;

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
    out float vLinearDepth;

    void main ()
    {
        // replace this with an actual uniform
        float uLinearDepth = 1000.0;
        
        vWorldVertex = uMMatrix * vec4( aPosition, 1.0 );
        vec4 viewVertex = uVMatrix * vWorldVertex;
        gl_Position = uPMatrix * viewVertex;
     
        vLinearDepth = length( viewVertex ) / uLinearDepth;
        vWorldNormal = mat3( uVMatrix * uNMatrix ) * aNormal;
        
	    if ( uUseNormalMap ) {
	    	vWorldTangent = normalize( mat3( uNMatrix ) * aTangent );
	    	vWorldBiTangent = cross( vWorldNormal, vWorldTangent );
	    }
        
        vTextureCoord = aTextureCoord;
    }
)};

const char *deferred_fs = { CRIMILD_TO_STRING(
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
    in float vLinearDepth;
                                              
    uniform Material uMaterial;
    uniform sampler2D uColorMap;
    uniform bool uUseColorMap;
    uniform sampler2D uNormalMap;
    uniform bool uUseNormalMap;
    uniform sampler2D uSpecularMap;
    uniform bool uUseSpecularMap;
    uniform sampler2D uEmissiveMap;
    uniform bool uUseEmissiveMap;

    out vec4 vFragData[ 4 ];
                                              
    void main( void )
    {
        if ( uUseColorMap ) {
            vFragData[ 0 ] = uMaterial.ambient + uMaterial.diffuse * texture( uColorMap, vTextureCoord );
        }
        else {
            vFragData[ 0 ] = uMaterial.ambient + uMaterial.diffuse;
        }
        
        vFragData[ 1 ] = vec4( vWorldVertex.xyz, vLinearDepth );
        
        vec3 normal = vWorldNormal;
        if ( uUseNormalMap ) {
            mat3 tangentToWorld = mat3( vWorldTangent.x, vWorldBiTangent.x, vWorldNormal.x,
                                       vWorldTangent.y, vWorldBiTangent.y, vWorldNormal.y,
                                       vWorldTangent.z, vWorldBiTangent.z, vWorldNormal.z );
            normal = ( 2.0 * texture( uNormalMap, vTextureCoord ).xyz - 1.0 ) * tangentToWorld;
            
        }
        normalize( normal );
        
        vFragData[ 2 ] = vec4( normal, 1.0 );
        
//        if ( uUseSpecularMap ) {
//            float specularColor = uUseSpecularMap ? texture( uSpecularMap, vTextureCoord ).r : 1.0;
//            vFragData[ 2 ].a = specularColor;
//        }
        
        if ( uUseEmissiveMap ) {
            vFragData[ 3 ] = texture( uEmissiveMap, vTextureCoord );
        }
        else {
            vFragData[ 3 ] = vec4( 0.0, 0.0, 0.0, 1.0 );
        }
    }
)};

DeferredRenderShaderProgram::DeferredRenderShaderProgram( void )
    : ShaderProgram( Utils::getVertexShaderInstance( deferred_vs ), Utils::getFragmentShaderInstance( deferred_fs ) )
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
}

DeferredRenderShaderProgram::~DeferredRenderShaderProgram( void )
{
    
}

