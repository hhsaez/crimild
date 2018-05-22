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

#include "BlendShaderProgram.hpp"
#include "Rendering/OpenGLUtils.hpp"

using namespace crimild;
using namespace crimild::opengl;

const char *blend_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;

    out vec2 vTextureCoord;

    void main ()
    {
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
        vTextureCoord = aTextureCoord;
    }
)};

const char *blend_fs = { CRIMILD_TO_STRING(
    in vec2 vTextureCoord;

    uniform sampler2D uBlendSrcMap;
    uniform sampler2D uBlendDstMap;
    uniform int uBlendMode;

    out vec4 vFragColor;

    void main( void )
    {
        vec4 src = texture( uBlendSrcMap, vTextureCoord );
        vec4 dst = texture( uBlendDstMap, vTextureCoord );
        
        if ( uBlendMode == 0 ) {
            // additive blending (strong result, high overexposure)
            vFragColor = min( src + dst, 1.0 );
        }
        else if ( uBlendMode == 1 ) {
            // screen blending (mild result, medium overexposure)
            vFragColor = clamp( ( src + dst ) - ( src * dst ), 0.0, 1.0 );
            vFragColor.a = 1.0;
        }
        else if ( uBlendMode == 2 ) {
            // softlight blending (light result, no overexposure)
            src = ( src * 0.5 ) + 0.5;
            
            vFragColor.xyz = vec3( ( src.x <= 0.5 ) ? ( dst.x - ( 1.0 - 2.0 * src.x ) * dst.x * ( 1.0 - dst.x ) ) : ( ( ( src.x > 0.5 ) && ( dst.x <= 0.25 ) ) ? ( dst.x + ( 2.0 * src.x - 1.0 ) * ( 4.0 * dst.x * ( 4.0 * dst.x + 1.0 ) * ( dst.x - 1.0 ) + 7.0 * dst.x ) ) : ( dst.x + ( 2.0 * src.x - 1.0 ) * ( sqrt( dst.x ) - dst.x ) ) ),
                                   ( src.y <= 0.5 ) ? ( dst.y - ( 1.0 - 2.0 * src.y ) * dst.y * ( 1.0 - dst.y ) ) : ( ( ( src.y > 0.5 ) && ( dst.y <= 0.25 ) ) ? ( dst.y + ( 2.0 * src.y - 1.0 ) * ( 4.0 * dst.y * ( 4.0 * dst.y + 1.0 ) * ( dst.y - 1.0 ) + 7.0 * dst.y ) ) : ( dst.y + ( 2.0 * src.y - 1.0 ) * ( sqrt( dst.y ) - dst.y ) ) ),
                                   ( src.z <= 0.5 ) ? ( dst.z - ( 1.0 - 2.0 * src.z ) * dst.z * ( 1.0 - dst.z ) ) : ( ( ( src.z > 0.5 ) && ( dst.z <= 0.25 ) ) ? ( dst.z + ( 2.0 * src.z - 1.0 ) * ( 4.0 * dst.z * ( 4.0 * dst.z + 1.0 ) * ( dst.z - 1.0 ) + 7.0 * dst.z ) ) : ( dst.z + ( 2.0 * src.z - 1.0 ) * ( sqrt( dst.z ) - dst.z ) ) ) );
            vFragColor.w = 1.0;
        }
        else if ( uBlendMode == 10 ) {
            vFragColor = src;
        }
        else if ( uBlendMode == 11 ) {
            vFragColor = dst;
        }
    }
)};

BlendShaderProgram::BlendShaderProgram( void )
    : ShaderProgram( OpenGLUtils::getVertexShaderInstance( blend_vs ), OpenGLUtils::getFragmentShaderInstance( blend_fs ) )
{
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
    
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::BLEND_SRC_MAP_UNIFORM, "uBlendSrcMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::BLEND_DST_MAP_UNIFORM, "uBlendDstMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::BLEND_MODE_UNIFORM, "uBlendMode" );
}

BlendShaderProgram::~BlendShaderProgram( void )
{
    
}

