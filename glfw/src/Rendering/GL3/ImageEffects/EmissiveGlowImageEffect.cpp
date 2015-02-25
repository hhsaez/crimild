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

#include "EmissiveGlowImageEffect.hpp"

#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

#define EMISSIVE_BUFFER_NAME ImageEffect::FBO_AUX_QUARTER_RES_1
#define GLOW_BUFFER_NAME ImageEffect::FBO_AUX_1

const char *glow_emissive_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;
                                                  
    out vec2 vTextureCoord;

    void main ()
    {
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
        vTextureCoord = aTextureCoord;
    }
)};

const char *glow_emissive_fs = { CRIMILD_TO_STRING(
 
    in vec2 vTextureCoord;

    uniform sampler2D uColorMap;
    uniform sampler2D uEmissiveMap;

    out vec4 vFragColor;

    void main( void )
    {
        vec4 color = texture( uColorMap, vTextureCoord );
        float emissive = texture( uEmissiveMap, vTextureCoord ).a;

        vFragColor = emissive * color;
    }                      
)};

const char *glow_compute_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;
                                                  
    out vec2 vTextureCoord;

    void main ()
    {
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
        vTextureCoord = aTextureCoord;
    }
)};

const char *glow_compute_fs = { CRIMILD_TO_STRING(
 
    in vec2 vTextureCoord;

    uniform sampler2D uEmissiveMap;

    uniform int uBlurSize = 16;
    uniform float uBlurScale = 1.0;
    uniform float uBlurStrength = 0.25;
    uniform int uOrientation = 0;

    out vec4 vFragColor;

    float gaussian( float x, float deviation )
    {
        return ( 1.0 / sqrt( 2.0 * 3.141592 * deviation ) ) * exp( -( ( x * x ) / ( 2.0 * deviation ) ) );
    }
                                                  
    void main( void )
    {
        vec2 texelSize = 1.0 / vec2( textureSize( uEmissiveMap, 0 ) );

        float halfBlur = 0.5 * float( uBlurSize );
        vec4 color = vec4( 0.0 );
        vec4 texColor = vec4( 0.0 );

        float deviation = 0.35 * halfBlur;
        deviation *= deviation;
        float strength = 1.0 - uBlurStrength;

        if ( uOrientation == 0 ) {
            // horizontal blur
            for ( int i = 0; i < uBlurSize; i++ ) {
                float offset = float( i ) - halfBlur;
                texColor = texture( uEmissiveMap, vTextureCoord + vec2( offset * texelSize.x * uBlurScale, 0.0 ) ) * gaussian( offset + strength, deviation );
                color += texColor;
            }
        }
        else {
            // vertical blur
            for ( int i = 0; i < uBlurSize; i++ ) {
                float offset = float( i ) - halfBlur;
                texColor = texture( uEmissiveMap, vTextureCoord + vec2( 0.0, offset * texelSize.y * uBlurScale ) ) * gaussian( offset + strength, deviation );
                color += texColor;
            }
        }

        vFragColor = clamp( color, 0.0, 1.0 );
        vFragColor.a = 1.0;
    }                      
)};

EmissiveGlowImageEffect::EmissiveGlowImageEffect( void )
{

}

EmissiveGlowImageEffect::~EmissiveGlowImageEffect( void )
{
    
}

void EmissiveGlowImageEffect::compute( RendererPtr const &renderer, CameraPtr const &camera )
{
    // compute emissives
    auto emissiveBuffer = getFrameBuffer( renderer, EMISSIVE_BUFFER_NAME );
    renderer->bindFrameBuffer( emissiveBuffer );

    auto emissiveProgram = renderer->getShaderProgram( "glow_emissive" );
    if ( emissiveProgram == nullptr ) {
        emissiveProgram = crimild::alloc< ShaderProgram >( Utils::getVertexShaderInstance( glow_emissive_vs ), Utils::getFragmentShaderInstance( glow_emissive_fs ) );
        emissiveProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
        emissiveProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
        
        emissiveProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uColorMap" ) );
        emissiveProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uEmissiveMap" ) );
        
        renderer->addShaderProgram( "glow_emissive", emissiveProgram );
    }

    auto gBuffer = renderer->getFrameBuffer( RenderPass::G_BUFFER_NAME );
    auto color = gBuffer->getRenderTargets()->get( RenderPass::G_BUFFER_DIFFUSE_TARGET_NAME );
    auto emissive = gBuffer->getRenderTargets()->get( RenderPass::G_BUFFER_VIEW_SPACE_NORMAL_TARGET_NAME );

    renderer->bindProgram( emissiveProgram );

    renderer->bindTexture( emissiveProgram->getLocation( "uColorMap" ), color->getTexture() );
    renderer->bindTexture( emissiveProgram->getLocation( "uEmissiveMap" ), emissive->getTexture() );

    renderer->drawScreenPrimitive( emissiveProgram );

    renderer->unbindTexture( emissiveProgram->getLocation( "uColorMap" ), color->getTexture() );
    renderer->unbindTexture( emissiveProgram->getLocation( "uEmissiveMap" ), emissive->getTexture() );

    renderer->unbindProgram( emissiveProgram );

    renderer->unbindFrameBuffer( emissiveBuffer );

    // compute glow map
    auto glowBuffer = getFrameBuffer( renderer, GLOW_BUFFER_NAME );
    renderer->bindFrameBuffer( glowBuffer );

    auto glowProgram = renderer->getShaderProgram( "glow_compute" );
    if ( glowProgram == nullptr ) {
        glowProgram = crimild::alloc< ShaderProgram >( Utils::getVertexShaderInstance( glow_compute_vs ), Utils::getFragmentShaderInstance( glow_compute_fs ) );
        glowProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
        glowProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
        
        glowProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uEmissiveMap" ) );
        glowProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uOrientation" ) );
        
        renderer->addShaderProgram( "glow_compute", glowProgram );
    }

    renderer->bindProgram( glowProgram );

    renderer->bindTexture( glowProgram->getLocation( "uEmissiveMap" ), emissiveBuffer->getRenderTargets()->get( "color" )->getTexture() );

    renderer->bindUniform( glowProgram->getLocation( "uOrientation" ), 0 );
    renderer->drawScreenPrimitive( glowProgram );

    renderer->bindUniform( glowProgram->getLocation( "uOrientation" ), 1 );
    renderer->setAlphaState( AlphaState::ENABLED_ADDITIVE_BLEND );
    renderer->setDepthState( DepthState::DISABLED );
    renderer->drawScreenPrimitive( glowProgram );
    renderer->setAlphaState( AlphaState::DISABLED );
    renderer->setDepthState( DepthState::ENABLED );

    renderer->unbindTexture( glowProgram->getLocation( "uEmissiveMap" ), emissiveBuffer->getRenderTargets()->get( "color" )->getTexture() );

    renderer->unbindProgram( glowProgram );

    renderer->unbindFrameBuffer( glowBuffer );
}

void EmissiveGlowImageEffect::apply( crimild::RendererPtr const &renderer, crimild::CameraPtr const &camera )
{
    auto scene = renderer->getFrameBuffer( RenderPass::S_BUFFER_NAME );
    renderScreen( renderer, scene->getRenderTargets()->get( RenderPass::S_BUFFER_COLOR_TARGET_NAME )->getTexture() );

    auto glow = renderer->getFrameBuffer( GLOW_BUFFER_NAME );
    renderer->setAlphaState( AlphaState::ENABLED_ADDITIVE_BLEND );
    renderer->setDepthState( DepthState::DISABLED );
    renderScreen( renderer, glow->getRenderTargets()->get( "color" )->getTexture() );
    renderer->setAlphaState( AlphaState::DISABLED );
    renderer->setDepthState( DepthState::ENABLED );
}

