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

#include "SSAOShaderProgram.hpp"
#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *ssao_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;

    out vec2 vTextureCoord;

    void main ()
    {
		gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
        vTextureCoord = aTextureCoord;
    }
)};

const char *ssao_fs = { CRIMILD_TO_STRING(
    in vec2 vTextureCoord;
                                          
    uniform sampler2D uPositionMap;
    uniform sampler2D uNormalMap;
                                          
    out vec4 vFragColor;
                                          
    float samplePixels( vec3 srcPosition, vec3 srcNormal, vec2 uv )
    {
        // move this to an actual uniform
        float uOccluderBias = 0.01;
        vec2 uAttenuation = vec2( 1.0, 1.0 );
        
        vec3 dstPosition = texture( uPositionMap, uv ).xyz;
        
        vec3 positionVec = dstPosition - srcPosition;
        float intensity = max( dot( normalize( positionVec ), srcNormal ) - uOccluderBias, 0.0 );
        
        float dist = length( positionVec );
        float attenuation = 1.0 / ( uAttenuation.x + ( uAttenuation.y * dist ) );
        
        return intensity * attenuation;
    }
                                          
    float rand( vec2 co ){
        return fract( sin( dot( co.xy ,vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
    }
                                          
    void main( void )
    {
        // move these to actual uniforms
        float uSamplingRadius = 20.0;
        float uTexelSize = 1.0 / 1024.0;
        
        vec3 srcPosition = texture( uPositionMap, vTextureCoord ).xyz;
        vec3 srcNormal = texture( uNormalMap, vTextureCoord ).xyz;
        float srcDepth = texture( uPositionMap, vTextureCoord ).w;
        vec2 randVec = normalize( vec2( 0.5 * rand( srcNormal.xy ) - 0.5, 0.5 * rand( srcPosition.xy ) - 0.5 ) );
        
        float kernelRadius = uSamplingRadius * ( 1.0 - srcDepth );
        
        vec2 kernel[ 4 ];
        kernel[ 0 ] = vec2( 0.0, 1.0 );
        kernel[ 1 ] = vec2( 1.0, 0.0 );
        kernel[ 2 ] = vec2( 0.0, -1.0 );
        kernel[ 3 ] = vec2( -1.0, 0.0 );
        
        const float sin45 = 0.707107;
        
        float occlusion = 0.0;
        for ( int i = 0; i < 4; i++ ) {
            vec2 k1 = reflect( kernel[ i ], randVec );
            vec2 k2 = vec2( k1.x * sin45 - k1.y * sin45, k1.x * sin45 + k1.y * sin45 );
            k1 *= uTexelSize;
            k2 *= uTexelSize;
            
            occlusion += samplePixels( srcPosition, srcNormal, vTextureCoord + k1 * kernelRadius * 1.0 );
            occlusion += samplePixels( srcPosition, srcNormal, vTextureCoord + k2 * kernelRadius * 0.75 );
            occlusion += samplePixels( srcPosition, srcNormal, vTextureCoord + k1 * kernelRadius * 0.50 );
            occlusion += samplePixels( srcPosition, srcNormal, vTextureCoord + k2 * kernelRadius * 0.25 );
        }
        
        occlusion /= 16.0;
        occlusion = clamp( occlusion, 0.0, 1.0 );
        
        vFragColor = vec4( occlusion );
    }
)};

SSAOShaderProgram::SSAOShaderProgram( void )
: ShaderProgram( Utils::getVertexShaderInstance( ssao_vs ), Utils::getFragmentShaderInstance( ssao_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
    
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM, "uPositionMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM, "uNormalMap" );
}

SSAOShaderProgram::~SSAOShaderProgram( void )
{
    
}

