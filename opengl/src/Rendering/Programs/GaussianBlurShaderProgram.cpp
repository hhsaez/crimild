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

#include "GaussianBlurShaderProgram.hpp"

#include "Rendering/OpenGLUtils.hpp"

using namespace crimild;
using namespace crimild::opengl;

const char *gaussian_blur_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;

    out vec2 vTextureCoord;
    out vec2 vBlurTextureCoords[ 14 ];

    void main()
    {
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
        
        vTextureCoord = aTextureCoord;
        vBlurTextureCoords[ 0 ] = vTextureCoord + vec2(0.0, -0.028);
        vBlurTextureCoords[ 1 ] = vTextureCoord + vec2(0.0, -0.024);
        vBlurTextureCoords[ 2 ] = vTextureCoord + vec2(0.0, -0.020);
        vBlurTextureCoords[ 3 ] = vTextureCoord + vec2(0.0, -0.016);
        vBlurTextureCoords[ 4 ] = vTextureCoord + vec2(0.0, -0.012);
        vBlurTextureCoords[ 5 ] = vTextureCoord + vec2(0.0, -0.008);
        vBlurTextureCoords[ 6 ] = vTextureCoord + vec2(0.0, -0.004);
        vBlurTextureCoords[ 7 ] = vTextureCoord + vec2(0.0,  0.004);
        vBlurTextureCoords[ 8 ] = vTextureCoord + vec2(0.0,  0.008);
        vBlurTextureCoords[ 9 ] = vTextureCoord + vec2(0.0,  0.012);
        vBlurTextureCoords[ 10 ] = vTextureCoord + vec2(0.0,  0.016);
        vBlurTextureCoords[ 11 ] = vTextureCoord + vec2(0.0,  0.020);
        vBlurTextureCoords[ 12 ] = vTextureCoord + vec2(0.0,  0.024);
        vBlurTextureCoords[ 13 ] = vTextureCoord + vec2(0.0,  0.028);
    }
)};

const char *gaussian_blur_fs = { CRIMILD_TO_STRING(
    in vec2 vTextureCoord;
    in vec2 vBlurTextureCoords[ 14 ];

    uniform bool uUseColorMap;
    uniform sampler2D uColorMap;

    out vec4 vFragColor;
                                                      
    void main()
    {
        vFragColor = vec4(0.0);
        vFragColor += texture(uColorMap, vBlurTextureCoords[ 0])*0.0044299121055113265;
        vFragColor += texture(uColorMap, vBlurTextureCoords[ 1])*0.00895781211794;
        vFragColor += texture(uColorMap, vBlurTextureCoords[ 2])*0.0215963866053;
        vFragColor += texture(uColorMap, vBlurTextureCoords[ 3])*0.0443683338718;
        vFragColor += texture(uColorMap, vBlurTextureCoords[ 4])*0.0776744219933;
        vFragColor += texture(uColorMap, vBlurTextureCoords[ 5])*0.115876621105;
        vFragColor += texture(uColorMap, vBlurTextureCoords[ 6])*0.147308056121;
        vFragColor += texture(uColorMap, vTextureCoord         )*0.159576912161;
        vFragColor += texture(uColorMap, vBlurTextureCoords[ 7])*0.147308056121;
        vFragColor += texture(uColorMap, vBlurTextureCoords[ 8])*0.115876621105;
        vFragColor += texture(uColorMap, vBlurTextureCoords[ 9])*0.0776744219933;
        vFragColor += texture(uColorMap, vBlurTextureCoords[10])*0.0443683338718;
        vFragColor += texture(uColorMap, vBlurTextureCoords[11])*0.0215963866053;
        vFragColor += texture(uColorMap, vBlurTextureCoords[12])*0.00895781211794;
        vFragColor += texture(uColorMap, vBlurTextureCoords[13])*0.0044299121055113265;
    }
)};

GaussianBlurShaderProgram::GaussianBlurShaderProgram( void )
    : ShaderProgram( OpenGLUtils::getVertexShaderInstance( gaussian_blur_vs ), OpenGLUtils::getFragmentShaderInstance( gaussian_blur_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
    
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM, "uColorMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_USE_COLOR_MAP_UNIFORM, "uUseColorMap" );
}

GaussianBlurShaderProgram::~GaussianBlurShaderProgram( void )
{ 
}

