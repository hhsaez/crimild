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

#include "BloomImageEffect.hpp"

#include "Rendering/OpenGLUtils.hpp"

#include <Rendering/Renderer.hpp>
#include <Rendering/RenderPasses/RenderPass.hpp>
#include <Rendering/FrameBufferObject.hpp>

using namespace crimild;
using namespace crimild::opengl;

#define EMISSIVE_BUFFER_NAME ImageEffect::FBO_AUX_2
#define GLOW_BUFFER_NAME ImageEffect::FBO_AUX_1

const char *bloom_emissive_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;
                                                  
    out vec2 vTextureCoord;

    void main ()
    {
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
        vTextureCoord = aTextureCoord;
    }
)};

const char *bloom_emissive_fs = { CRIMILD_TO_STRING(
 
    in vec2 vTextureCoord;

    uniform sampler2D uColorMap;

    uniform float uBrightFilter = 0.5;

    out vec4 vFragColor;

    float computeLuminance( vec3 rgb )
    {   
        float minRGB = min( rgb.r, min( rgb.g, rgb.b ) );
        float maxRGB = max( rgb.r, max( rgb.g, rgb.b ) );
        return ( maxRGB + minRGB ) / 2.0;
    }

    float brightPassValue( float luminance )
    {
        if ( luminance > uBrightFilter ) {
            return max( luminance * 2.5, 1.0 );
        }
        else {
            return max( luminance * 0.0, 0.0 );
        }
    }

    void main( void )
    {
        vec3 color = texture( uColorMap, vTextureCoord ).rgb;
        float luminance = computeLuminance( color );
        luminance = brightPassValue( luminance );
        color *= luminance;
        vFragColor = vec4( color, 1.0 );
    }                      
)};

const char *bloom_compute_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;
                                                  
    out vec2 vTextureCoord;

    void main ()
    {
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
        vTextureCoord = aTextureCoord;
    }
)};

const char *bloom_compute_fs = { CRIMILD_TO_STRING(
 
    in vec2 vTextureCoord;

    uniform sampler2D uEmissiveMap;

    uniform int uBlurSize = 16;
    uniform float uBlurScale = 1.0;
    uniform float uBlurStrength = 0.1;
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

BloomImageEffect::BloomImageEffect( void )
    : BloomImageEffect( 0.5, 16.0, 0.25, 1.0f )
{

}

BloomImageEffect::BloomImageEffect( float brightFilter, float glowSize, float glowStrength, float glowScale )
    : _brightFilter( brightFilter ),
      _glowSize( glowSize ),
      _glowStrength( glowStrength ),
      _glowScale( glowScale )
{
    
}

BloomImageEffect::~BloomImageEffect( void )
{
    
}

void BloomImageEffect::compute( crimild::Renderer *renderer, Camera *camera )
{
    computeBrightPassFilter( renderer, camera );
    computeGlowMap( renderer, camera );
}

void BloomImageEffect::apply( crimild::Renderer *renderer, crimild::Camera *camera )
{
#if 0
    auto emissiveBuffer = getFrameBuffer( renderer, EMISSIVE_BUFFER_NAME );
    renderScreen( renderer, emissiveBuffer->getRenderTargets()->get( "color" )->getTexture() );
#else
    auto scene = renderer->getFrameBuffer( RenderPass::S_BUFFER_NAME );
    renderScreen( renderer, scene->getRenderTargets()[ RenderPass::S_BUFFER_COLOR_TARGET_NAME ]->getTexture() );

    auto glow = renderer->getFrameBuffer( GLOW_BUFFER_NAME );
    renderer->setAlphaState( AlphaState::ENABLED_ADDITIVE_BLEND );
    renderer->setDepthState( DepthState::DISABLED );
    renderScreen( renderer, glow->getRenderTargets()[ "color" ]->getTexture() );
    renderer->setAlphaState( AlphaState::DISABLED );
    renderer->setDepthState( DepthState::ENABLED );
#endif
}

void BloomImageEffect::computeBrightPassFilter( crimild::Renderer *renderer, Camera *camera )
{
    auto emissiveBuffer = getFrameBuffer( renderer, EMISSIVE_BUFFER_NAME );

    auto program = renderer->getShaderProgram( "bloom_emissive" );
    if ( program == nullptr ) {
        auto tmp = crimild::alloc< ShaderProgram >( OpenGLUtils::getVertexShaderInstance( bloom_emissive_vs ), OpenGLUtils::getFragmentShaderInstance( bloom_emissive_fs ) ) ;
        renderer->setShaderProgram( "bloom_emissive", tmp );
        program = crimild::get_ptr( tmp );
        
        program->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
        program->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
        program->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uColorMap" ) );
        program->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uBrightFilter" ) );
    }

    auto scene = renderer->getFrameBuffer( RenderPass::S_BUFFER_NAME );
    auto color = scene->getRenderTargets()[ RenderPass::S_BUFFER_COLOR_TARGET_NAME ];

    renderer->bindFrameBuffer( emissiveBuffer );
    renderer->bindProgram( program );
    renderer->bindTexture( program->getLocation( "uColorMap" ), color->getTexture() );
    renderer->bindUniform( program->getLocation( "uBrightFilter" ), _brightFilter );

    renderer->drawScreenPrimitive( program );

    renderer->unbindTexture( program->getLocation( "uColorMap" ), color->getTexture() );
    renderer->unbindProgram( program );
    renderer->unbindFrameBuffer( emissiveBuffer );
}

void BloomImageEffect::computeGlowMap( crimild::Renderer *renderer, Camera *camera )
{
    // compute glow map
    auto emissiveBuffer = getFrameBuffer( renderer, EMISSIVE_BUFFER_NAME );
    auto glowBuffer = getFrameBuffer( renderer, GLOW_BUFFER_NAME );
    renderer->bindFrameBuffer( glowBuffer );

    auto glowProgram = renderer->getShaderProgram( "bloom_compute" );
    if ( glowProgram == nullptr ) {
        auto tmp = crimild::alloc< ShaderProgram >( OpenGLUtils::getVertexShaderInstance( bloom_compute_vs ), OpenGLUtils::getFragmentShaderInstance( bloom_compute_fs ) );
        renderer->setShaderProgram( "bloom_compute", tmp );
        glowProgram = crimild::get_ptr( tmp );
        
        glowProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
        glowProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
        
        glowProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uEmissiveMap" ) );
        glowProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uOrientation" ) );
        glowProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uBlurSize" ) );
        glowProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uBlurStrength" ) );
        glowProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uBlurScale" ) );
    }

    renderer->bindProgram( glowProgram );

    renderer->bindTexture( glowProgram->getLocation( "uEmissiveMap" ), emissiveBuffer->getRenderTargets()[ "color" ]->getTexture() );

    renderer->bindUniform( glowProgram->getLocation( "uBlurSize" ), _glowSize );
    renderer->bindUniform( glowProgram->getLocation( "uBlurStrength" ), _glowStrength );
    renderer->bindUniform( glowProgram->getLocation( "uBlurScale" ), _glowScale );

    renderer->bindUniform( glowProgram->getLocation( "uOrientation" ), 0 );
    renderer->drawScreenPrimitive( glowProgram );

    renderer->bindUniform( glowProgram->getLocation( "uOrientation" ), 1 );
    renderer->setAlphaState( AlphaState::ENABLED_ADDITIVE_BLEND );
    renderer->setDepthState( DepthState::DISABLED );
    renderer->drawScreenPrimitive( glowProgram );
    renderer->setAlphaState( AlphaState::DISABLED );
    renderer->setDepthState( DepthState::ENABLED );

    renderer->unbindTexture( glowProgram->getLocation( "uEmissiveMap" ), emissiveBuffer->getRenderTargets()[ "color" ]->getTexture() );

    renderer->unbindProgram( glowProgram );

    renderer->unbindFrameBuffer( glowBuffer );
}

