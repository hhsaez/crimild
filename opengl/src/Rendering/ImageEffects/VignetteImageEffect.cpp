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

#include "VignetteImageEffect.hpp"

#include "Rendering/OpenGLUtils.hpp"

using namespace crimild;
using namespace crimild::opengl;

const char *vignette_apply_vs = R"(
    CRIMILD_GLSL_ATTRIBUTE vec3 aPosition;
    CRIMILD_GLSL_ATTRIBUTE vec2 aTextureCoord;

    CRIMILD_GLSL_VARYING_OUT vec2 vTextureCoord;

    void main()
    {
        vTextureCoord = aTextureCoord;
        CRIMILD_GLSL_VERTEX_OUTPUT = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
    }
)";

const char *vignette_apply_fs = R"(
    CRIMILD_GLSL_PRECISION_FLOAT_HIGH

    CRIMILD_GLSL_VARYING_IN vec2 vTextureCoord;

    uniform sampler2D uColorMap;

    uniform float uInnerVignetting = 0.75;
    uniform float uOuterVignetting = 1.0;

    CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

    void main( void ) 
    {
        vec4 color = CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord );
        
        float d = distance( vec2( 0.5, 0.5 ), vTextureCoord ) * 1.414213;
        float vignetting = clamp( ( uOuterVignetting - d ) / ( uOuterVignetting - uInnerVignetting ), 0.0, 1.0 );
        color.rgb *= vignetting;

        CRIMILD_GLSL_FRAGMENT_OUTPUT = vec4( color.rgb, 1.0 );
    }
)";

VignetteImageEffect::VignetteImageEffect( void )
{
    _program = crimild::alloc< ShaderProgram >( OpenGLUtils::getVertexShaderInstance( vignette_apply_vs ), OpenGLUtils::getFragmentShaderInstance( vignette_apply_fs ) );

    _program->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
    _program->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
    
    _program->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM, "uColorMap" );

    _innerCutoff = crimild::alloc< FloatUniform >( "uInnerVignetting", 0.0f );
    _program->attachUniform( _innerCutoff );

    _outerCutoff = crimild::alloc< FloatUniform >( "uOuterVignetting", 0.0f );
    _program->attachUniform( _outerCutoff );
}

VignetteImageEffect::~VignetteImageEffect( void )
{

}

void VignetteImageEffect::compute( Renderer *renderer, Camera *camera )
{

}

void VignetteImageEffect::apply( crimild::Renderer *renderer, crimild::Camera * )
{
    auto fbo = renderer->getFrameBuffer( RenderPass::S_BUFFER_NAME );
    auto color = fbo->getRenderTargets().get( RenderPass::S_BUFFER_COLOR_TARGET_NAME );
    if ( color == nullptr || color->getTexture() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid color texture" );
        return;
    }

    renderer->bindProgram( getProgram() );
    renderer->bindTexture( getProgram()->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), color->getTexture() );
    
    renderer->drawScreenPrimitive( getProgram() );

    renderer->unbindTexture( getProgram()->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), color->getTexture() );
    renderer->unbindProgram( getProgram() );
}

