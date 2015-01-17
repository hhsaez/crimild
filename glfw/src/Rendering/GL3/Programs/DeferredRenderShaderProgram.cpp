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
    struct VertexData {
        vec3 position;
        vec3 normal;
        vec3 tangent;
        vec3 bitangent;
        vec2 textureCoord;
        float linearDepth;
        vec3 viewSpaceNormal;
    };

    in vec3 aPosition;
    in vec2 aTextureCoord;
    in vec3 aTangent;
    in vec3 aNormal;

    uniform mat4 uPMatrix;
    uniform mat4 uVMatrix;
    uniform mat4 uMMatrix;
                                              
    uniform float uLinearDepth;
                                              
    out VertexData vVertex;

    void main ()
    {
        vVertex.position = ( uMMatrix * vec4( aPosition, 1.0 ) ).xyz;
        vec4 vsPosition = uVMatrix * vec4( vVertex.position, 1.0 );
        vVertex.linearDepth = length( vsPosition ) / uLinearDepth;
        vVertex.normal = normalize( ( uMMatrix * vec4( aNormal, 0.0 ) ).xyz );
        vVertex.tangent = normalize( ( uMMatrix * vec4( aTangent, 0.0 ) ).xyz );
        vVertex.bitangent = cross( vVertex.tangent, vVertex.normal );
        vVertex.textureCoord = aTextureCoord;
        vVertex.viewSpaceNormal = normalize( ( uVMatrix * uMMatrix * vec4( aNormal, 0.0 ) ).xyz );
        
        gl_Position = uPMatrix * vsPosition;
    }
)};

const char *deferred_fs = { CRIMILD_TO_STRING(
    struct Material {
        vec4 ambient;
        vec4 diffuse;
        vec4 specular;
        float shininess;
    };

    struct VertexData {
        vec3 position;
        vec3 normal;
        vec3 tangent;
        vec3 bitangent;
        vec2 textureCoord;
        float linearDepth;
        vec3 viewSpaceNormal;
    };

    in VertexData vVertex;

    uniform Material uMaterial;
    uniform sampler2D uColorMap;
    uniform bool uUseColorMap;
    uniform sampler2D uNormalMap;
    uniform bool uUseNormalMap;
    uniform sampler2D uSpecularMap;
    uniform bool uUseSpecularMap;
    uniform sampler2D uEmissiveMap;
    uniform bool uUseEmissiveMap;
                                              
    uniform mat4 uVMatrix;
    uniform mat4 uMMatrix;

    out vec4 vFragData[ 4 ];
                                              
    vec3 encodeNormal( vec3 normal )
    {
        return 0.5 + 0.5 * normal;
    }
                                              
    void main( void )
    {
        vFragData[ 0 ] = uMaterial.ambient + uMaterial.diffuse * ( uUseColorMap ? texture( uColorMap, vVertex.textureCoord ) : vec4( 1.0 ) );
        
        vFragData[ 1 ] = vec4( vVertex.position, vVertex.linearDepth );
        
        vec3 normal = vVertex.normal;
        if ( uUseNormalMap ) {
            mat3 tbn = mat3( vVertex.tangent, vVertex.bitangent, vVertex.normal );
            normal = tbn * ( 2.0 * texture( uNormalMap, vVertex.textureCoord ).xyz - 1.0 );
        }
        normal = normalize( normal );
        float specularFactor = uUseSpecularMap ? texture( uSpecularMap, vVertex.textureCoord ).r : 1.0;
        vFragData[ 2 ] = vec4( encodeNormal( normal ), specularFactor );
        
        float emissiveFactor = uUseEmissiveMap ? texture( uEmissiveMap, vVertex.textureCoord ).r : 0.0;
        vFragData[ 3 ] = vec4( encodeNormal( vVertex.viewSpaceNormal ), emissiveFactor );
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
    
    registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LINEAR_DEPTH_CONSTANT_UNIFORM, "uLinearDepth" );
}

DeferredRenderShaderProgram::~DeferredRenderShaderProgram( void )
{
    
}

