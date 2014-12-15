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

#include "BlurShaderProgram.hpp"
#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *BlurShaderProgram::UNIFORM_BLUR_INPUT = "uBlurInput";
const char *BlurShaderProgram::UNIFORM_BLUR_ORIENTATION = "uBlurOrientation";
const char *BlurShaderProgram::UNIFORM_BLUR_AMOUNT = "uBlurAmount";
const char *BlurShaderProgram::UNIFORM_BLUR_SCALE = "uBlurScale";
const char *BlurShaderProgram::UNIFORM_BLUR_STRENTH = "uBlurStrength";

const char *blur_vs = { CRIMILD_TO_STRING(
   in vec3 aPosition;
   in vec2 aTextureCoord;
   
   out vec2 vTextureCoord;
   
   void main ()
   {
       gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
       vTextureCoord = aTextureCoord;
   }
)};

const char *blur_fs = { CRIMILD_TO_STRING(
    in vec2 vTextureCoord;

    uniform sampler2D uBlurInput;
    uniform int uBlurOrientation;
    uniform int uBlurAmount;
    uniform float uBlurScale;
    uniform float uBlurStrength;
   
    out vec4 vFragColor;
                                          
    float gaussian( float x, float deviation )
    {
        return ( 1.0 / sqrt( 2.0 * 3.141592 * deviation ) ) * exp( -( ( x * x ) / ( 2.0 * deviation ) ) );
    }
   
    void main( void )
    {
        float halfBlur = float( uBlurAmount ) * 0.5;
        vec4 color = vec4( 0.0 );
        vec4 texColor = vec4( 0.0 );
        
        float deviation = halfBlur * 0.35;
        deviation *= deviation;
        float strength = 1.0 - uBlurStrength;
        
        vec2 texelSize = vec2( 1.0 / 1024.0, 1.0 / 1024.0 );
        
        if ( uBlurOrientation == 0 ) {
            // Horizontal blur
            for ( int i = 0; i < 10; ++i ) {
                if ( i >= uBlurAmount ) {
                    break;
                }
                
                float offset = float( i ) - halfBlur;
                texColor = texture( uBlurInput, vTextureCoord + vec2( offset * texelSize.x * uBlurScale, 0.0 ) ) * gaussian( offset * strength, deviation );
                color += texColor;
            }
        }
        else {
            // Vertical blur
            for ( int i = 0; i < 10; ++i ) {
                if ( i >= uBlurAmount ) {
                    break;
                }
                
                float offset = float( i ) - halfBlur;
                texColor = texture( uBlurInput, vTextureCoord + vec2( 0.0, offset * texelSize.y * uBlurScale ) ) * gaussian( offset * strength, deviation );
                color += texColor;
            }
        }
        
        // Apply colour
        vFragColor = clamp( color, 0.0, 1.0 );
        vFragColor.w = uBlurOrientation == 0 ? 1.0 : 1.0;
        
    }
)};

BlurShaderProgram::BlurShaderProgram( void )
    : ShaderProgram( Utils::getVertexShaderInstance( blur_vs ), Utils::getFragmentShaderInstance( blur_fs ) )
{
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
    
    registerLocation( std::make_shared< ShaderLocation >( ShaderLocation::Type::UNIFORM, UNIFORM_BLUR_INPUT ) );
    registerLocation( std::make_shared< ShaderLocation >( ShaderLocation::Type::UNIFORM, UNIFORM_BLUR_ORIENTATION ) );
    registerLocation( std::make_shared< ShaderLocation >( ShaderLocation::Type::UNIFORM, UNIFORM_BLUR_AMOUNT ) );
    registerLocation( std::make_shared< ShaderLocation >( ShaderLocation::Type::UNIFORM, UNIFORM_BLUR_SCALE ) );
    registerLocation( std::make_shared< ShaderLocation >( ShaderLocation::Type::UNIFORM, UNIFORM_BLUR_STRENTH ) );
}

BlurShaderProgram::~BlurShaderProgram( void )
{
    
}

