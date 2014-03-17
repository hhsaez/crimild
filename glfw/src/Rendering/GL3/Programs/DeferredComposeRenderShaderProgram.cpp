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

#include "DeferredComposeRenderShaderProgram.hpp"
#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *deferred_compose_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;

    out vec2 vTextureCoord;

    void main ()
    {
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
        vTextureCoord = aTextureCoord;
    }
)};

const char *deferred_compose_fs = { CRIMILD_TO_STRING(
    struct Light {
        vec3 position;
        vec3 attenuation;
        vec3 direction;
        vec4 color;
        float outerCutoff;
        float innerCutoff;
        float exponent;
        vec4 ambient;
    };

    in vec2 vTextureCoord;

    uniform sampler2D uColorMap;
    uniform sampler2D uPositionMap;
    uniform sampler2D uNormalMap;

    uniform int uLightCount;
    uniform Light uLights[ 4 ];
    
    out vec4 vFragColor;

    void main( void )
    {
        vec3 srcPosition = texture( uPositionMap, vTextureCoord ).xyz;
        float srcDepth = texture( uPositionMap, vTextureCoord ).w;
        vec3 srcNormal = texture( uNormalMap, vTextureCoord ).xyz;
        vec4 srcColor = texture( uColorMap, vTextureCoord );
        
        vec4 specularColor = vec4( 1.0, 1.0, 1.0, 1.0 );
        
        vec4 color = srcColor;
        vFragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
        
        vec3 normal = normalize( srcNormal );
        
        for ( int i = 0; i < 4; i++ ) {
            if ( i >= uLightCount ) {
                break;
            }
            
            vec3 lightVec = normalize( uLights[ i ].position - srcPosition );
            vec3 halfVector = -normalize( reflect( lightVec, normal ) );
            
            vFragColor.rgb += uLights[ i ].ambient.rgb;
            
            float l = dot( normal, lightVec );
            if ( l > 0.0 ) {
                float spotlight = 1.0;
                if ( ( uLights[ i ].direction.x != 0.0 ) || ( uLights[ i ].direction.y != 0.0 ) || ( uLights[ i ].direction.z != 0.0 ) ) {
                    spotlight = max( -dot( lightVec, uLights[ i ].direction ), 0.0 );
                    float spotlightFade = clamp( ( uLights[ i ].outerCutoff - spotlight ) / ( uLights[ i ].outerCutoff - uLights[ i ].innerCutoff ), 0.0, 1.0 );
                    spotlight = pow( spotlight * spotlightFade, uLights[ i ].exponent );
                }
                
                float s = pow( max( dot( halfVector, normal ), 0.0 ), 50.0 );
                float d = distance( srcPosition, uLights[ i ].position );
                float a = 1.0 / ( uLights[ i ].attenuation.x + ( uLights[ i ].attenuation.y * d ) + ( uLights[ i ].attenuation.z * d * d ) );
                
                vFragColor.rgb += ( ( color.rgb * l ) + ( specularColor.rgb * s ) ) * uLights[ i ].color.rgb * a * spotlight;
            }
        }
    }
)};

DeferredComposeRenderShaderProgram::DeferredComposeRenderShaderProgram( void )
    : ShaderProgram( Utils::getVertexShaderInstance( deferred_compose_vs ), Utils::getFragmentShaderInstance( deferred_compose_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
    
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::G_BUFFER_COLOR_MAP_UNIFORM, "uColorMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM, "uPositionMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM, "uNormalMap" );
    
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_COUNT_UNIFORM, "uLightCount" );
	for ( int i = 0; i < 4; i++ ) {
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "position" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_ATTENUATION_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "attenuation" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_DIRECTION_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "direction" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_COLOR_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "color" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_OUTER_CUTOFF_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "outerCutoff" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_INNER_CUTOFF_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "innerCutoff" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_EXPONENT_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "exponent" ) );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_AMBIENT_UNIFORM + i, Utils::buildArrayShaderLocationName( "uLights", i, "ambient" ) );
	}
}

DeferredComposeRenderShaderProgram::~DeferredComposeRenderShaderProgram( void )
{
    
}

