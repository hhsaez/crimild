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

#include "SSAOImageEffect.hpp"

#include "Rendering/GL3/Utils.hpp"

using namespace crimild;
using namespace crimild::gl3;

const char *ssao_compute_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;
                                                  
    out vec2 vTextureCoord;

    void main ()
    {
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
        vTextureCoord = aTextureCoord;
    }
)};

const char *ssao_compute_fs = { CRIMILD_TO_STRING(
 
    in vec2 vTextureCoord;

    uniform sampler2D uPositionMap;
    uniform sampler2D uNormalMap;
    uniform sampler2D uNoiseMap;
                                                  
    uniform float uRadius;
    uniform float uOccluderBias;
    uniform vec2 uAttenuation;

    uniform mat4 uVMatrix;

    out vec4 vFragColor;
                                                  
    float samplePixels ( vec3 position, vec3 normal, vec2 uv )
    {
        vec3 dstPosition = ( uVMatrix * vec4( texture( uPositionMap, uv ).xyz, 1.0 ) ).xyz;
        
        vec3 positionVec = dstPosition - position;
        float intensity = max( dot( normalize( positionVec ), normal ) - uOccluderBias, 0.0);
        
        float dist = length( positionVec );
        float attenuation = 1.0 / ( uAttenuation.x + ( uAttenuation.y * dist ) );
        
        return intensity * attenuation;
    }
                                                  
    void main( void )
    {
        vec3 position = ( uVMatrix * vec4( texture( uPositionMap, vTextureCoord ).xyz, 1.0 ) ).xyz;
        vec3 normal = texture( uNormalMap, vTextureCoord ).xyz;
        vec2 randVec = normalize( texture( uNoiseMap, vTextureCoord ).xy * 2.0 - 1.0 );
        float depth = texture( uPositionMap, vTextureCoord ).w;
        
        float kernelRadius = uRadius * ( 1.0 - depth );
        
        vec2 kernel[ 4 ];
        kernel[ 0 ] = vec2( 0.0, 1.0 );
        kernel[ 1 ] = vec2( 1.0, 0.0 );
        kernel[ 2 ] = vec2( 0.0, -1.0 );
        kernel[ 3 ] = vec2( -1.0, 0.0 );
        
        const float sin45 = 0.707107;
        
        float texelSize = 1.0 / textureSize( uPositionMap, 0 ).r;
        
        float occlusion = 0.0;
        for ( int i = 0; i < 4; i++ ) {
            vec2 k1 = reflect( kernel[ i ], randVec );
            vec2 k2 = vec2( k1.x * sin45 - k1.y * sin45,
                            k1.x * sin45 + k1.y * sin45 );
            k1 *= texelSize;
            k2 *= texelSize;
            
            occlusion += samplePixels( position, normal, vTextureCoord + k1 * kernelRadius );
            occlusion += samplePixels( position, normal, vTextureCoord + k2 * kernelRadius * 0.75 );
            occlusion += samplePixels( position, normal, vTextureCoord + k1 * kernelRadius * 0.5 );
            occlusion += samplePixels( position, normal, vTextureCoord + k2 * kernelRadius * 0.25 );
        }
        
        occlusion /= 16.0;
        occlusion = clamp( occlusion, 0.0, 1.0 );
        
        vFragColor = vec4( occlusion );
    }
                      
)};

const char *ssao_apply_vs = { CRIMILD_TO_STRING(
    in vec3 aPosition;
    in vec2 aTextureCoord;

    out vec2 vTextureCoord;

    void main ()
    {
        gl_Position = vec4( aPosition.x, aPosition.y, 0.0, 1.0 );
        vTextureCoord = aTextureCoord;
    }
)};

const char *ssao_apply_fs = { CRIMILD_TO_STRING(
    in vec2 vTextureCoord;

    uniform sampler2D uSSAOMap;
                                                
    uniform int uBlurSize = 8;

    out vec4 vFragColor;

    void main( void )
    {
        vec2 texelSize = 1.0 / vec2( textureSize( uSSAOMap, 0 ) );
        
        vec4 ao = vec4( 0.0 );
        vec2 hlim = vec2( float( -uBlurSize ) * 0.5 + 0.5 );
        for ( int x = 0; x < uBlurSize; ++x ) {
            for ( int y = 0; y < uBlurSize; ++y ) {
                vec2 offset = vec2( float( x ), float( y ) );
                offset += hlim;
                offset *= texelSize;
                ao += texture( uSSAOMap, vTextureCoord + offset );
            }
        }
        
        ao = ao / ( uBlurSize * uBlurSize );

        vFragColor = pow( 1.0 - ( ao ), vec4( 1.0 / 2.2 ) );
    }
)};

SSAOImageEffect::SSAOImageEffect( void )
    : SSAOImageEffect( 64, 20.0f, 0.01f, Vector2f( 0.5f, 2.5f ), 4 )
{
    
}

SSAOImageEffect::SSAOImageEffect( int noiseSize, float radius, float occluderBias, const Vector2f &attenuation, int blurSize )
    : _ssaoRadius( radius ),
      _ssaoOccluderBias( occluderBias ),
      _ssaoAttenuation( attenuation ),
      _ssaoBlurSize( blurSize )
{
    std::vector< unsigned char > noiseData( noiseSize * noiseSize * 3 );
    for ( int z = 0; z < noiseSize; z++ ) {
        for ( int x = 0; x < noiseSize; x++ ) {
            Vector3f noise( Numericf::random( -1.0f, 1.0f ), Numericf::random( -1.0f, 1.0f ), 0.0f );
            noise.normalize();

            noiseData[ z * ( noiseSize * 3 ) + x * 3 + 0 ] = static_cast< unsigned char >( 255.0f * ( 0.5f * noise[ 0 ] + 0.5f ) );
            noiseData[ z * ( noiseSize * 3 ) + x * 3 + 1 ] = static_cast< unsigned char >( 255.0f * ( 0.5f * noise[ 1 ] + 0.5f ) );
            noiseData[ z * ( noiseSize * 3 ) + x * 3 + 2 ] = static_cast< unsigned char >( 255.0f * ( 0.5f * noise[ 2 ] + 0.5f ) );
        }
    }
    auto noiseImage = crimild::alloc< Image >( noiseSize, noiseSize, 3, &noiseData[ 0 ], Image::PixelFormat::RGB );
    _noiseTexture = crimild::alloc< Texture >( noiseImage );
    
    _blendState = crimild::alloc< AlphaState >();
    _blendState->setEnabled( true );
    _blendState->setSrcBlendFunc( AlphaState::SrcBlendFunc::DST_COLOR );
    _blendState->setDstBlendFunc( AlphaState::DstBlendFunc::ZERO );
}

SSAOImageEffect::~SSAOImageEffect( void )
{
    
}

void SSAOImageEffect::compute( RendererPtr const &renderer, CameraPtr const &camera )
{
    computeSSAO( renderer, camera );
    applySSAO( renderer );
}

void SSAOImageEffect::apply( crimild::RendererPtr const &renderer, CameraPtr const &camera )
{
    auto scene = renderer->getFrameBuffer( "scene" );
    renderScreen( renderer, scene->getRenderTargets()->get( "color" )->getTexture() );
    
    auto ssaoBlur = getSSAOBlurBuffer( renderer );
    
    renderer->setDepthState( DepthState::DISABLED );
    renderer->setAlphaState( _blendState );
    
    renderScreen( renderer, ssaoBlur->getRenderTargets()->get( "color" )->getTexture() );

    renderer->setAlphaState( AlphaState::DISABLED );
    renderer->setDepthState( DepthState::ENABLED );
}

FrameBufferObjectPtr SSAOImageEffect::getSSAOBuffer( crimild::RendererPtr const &renderer )
{
    return getFrameBuffer( renderer, ImageEffect::FBO_AUX_1 );
}

FrameBufferObjectPtr SSAOImageEffect::getSSAOBlurBuffer( crimild::RendererPtr const &renderer )
{
    return getFrameBuffer( renderer, ImageEffect::FBO_AUX_2 );
}

void SSAOImageEffect::computeSSAO( crimild::RendererPtr const &renderer, CameraPtr const &camera )
{
    auto ssaoBuffer = getSSAOBuffer( renderer );
    
    renderer->bindFrameBuffer( ssaoBuffer );
    
    auto ssaoProgram = renderer->getShaderProgram( "ssao_compute" );
    if ( ssaoProgram == nullptr ) {
        ssaoProgram = crimild::alloc< ShaderProgram >( Utils::getVertexShaderInstance( ssao_compute_vs ), Utils::getFragmentShaderInstance( ssao_compute_fs ) );
        ssaoProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
        ssaoProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
        
        ssaoProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM, "uPositionMap" );
        ssaoProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM, "uNormalMap" );

        ssaoProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uVMatrix" ) );
        ssaoProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uNoiseMap" ) );
        ssaoProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uRadius" ) );
        ssaoProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uOccluderBias" ) );
        ssaoProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uAttenuation" ) );
        
        renderer->addShaderProgram( "ssao", ssaoProgram );
    }
    
    auto gBuffer = renderer->getFrameBuffer( DeferredRenderPass::G_BUFFER_NAME );
    auto positions = gBuffer->getRenderTargets()->get( DeferredRenderPass::G_BUFFER_POSITION_TARGET_NAME );
    auto normals = gBuffer->getRenderTargets()->get( DeferredRenderPass::G_BUFFER_VIEW_SPACE_NORMAL_TARGET_NAME );
    
    renderer->bindProgram( ssaoProgram );
    
    renderer->bindUniform( ssaoProgram->getLocation( "uRadius" ), _ssaoRadius );
    renderer->bindUniform( ssaoProgram->getLocation( "uOccluderBias" ), _ssaoOccluderBias );
    renderer->bindUniform( ssaoProgram->getLocation( "uAttenuation" ), _ssaoAttenuation );
    
    renderer->bindUniform( ssaoProgram->getLocation( "uVMatrix" ), camera->getViewMatrix() );
    
    renderer->bindTexture( ssaoProgram->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM ), positions->getTexture() );
    renderer->bindTexture( ssaoProgram->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM ), normals->getTexture() );
    renderer->bindTexture( ssaoProgram->getLocation( "uNoiseMap" ), _noiseTexture );
    
    renderer->drawScreenPrimitive( ssaoProgram );
    
    renderer->unbindTexture( ssaoProgram->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM ), positions->getTexture() );
    renderer->unbindTexture( ssaoProgram->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM ), normals->getTexture() );
    renderer->unbindTexture( ssaoProgram->getLocation( "uNoiseMap" ), _noiseTexture );
    
    renderer->unbindProgram( ssaoProgram );
    
    renderer->unbindFrameBuffer( ssaoBuffer );
}

void SSAOImageEffect::applySSAO( crimild::RendererPtr const &renderer )
{
    auto ssaoBlurFBO = getSSAOBlurBuffer( renderer );
    
    auto ssaoApplyProgram = renderer->getShaderProgram( "ssao_apply" );
    if ( ssaoApplyProgram == nullptr ) {
        ssaoApplyProgram = crimild::alloc< ShaderProgram >( Utils::getVertexShaderInstance( ssao_apply_vs ), Utils::getFragmentShaderInstance( ssao_apply_fs ) );
        ssaoApplyProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
        ssaoApplyProgram->registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );
        
        ssaoApplyProgram->registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::SSAO_MAP_UNIFORM, "uSSAOMap" );
        
        ssaoApplyProgram->registerLocation( crimild::alloc< ShaderLocation >( ShaderLocation::Type::UNIFORM, "uBlurSize" ) );

        renderer->addShaderProgram( "ssao_apply", ssaoApplyProgram );
    }
    
    auto ssaoBuffer = getSSAOBuffer( renderer );
    
    renderer->bindFrameBuffer( ssaoBlurFBO );
    renderer->bindProgram( ssaoApplyProgram );
    renderer->bindTexture( ssaoApplyProgram->getStandardLocation( ShaderProgram::StandardLocation::SSAO_MAP_UNIFORM ), ssaoBuffer->getRenderTargets()->get( "color" )->getTexture() );

    renderer->bindUniform( ssaoApplyProgram->getLocation( "uBlurSize" ), _ssaoBlurSize );

    renderer->drawScreenPrimitive( ssaoApplyProgram );

    renderer->unbindTexture( ssaoApplyProgram->getStandardLocation( ShaderProgram::StandardLocation::SSAO_MAP_UNIFORM ), ssaoBuffer->getRenderTargets()->get( "color" )->getTexture() );
    renderer->unbindProgram( ssaoApplyProgram );
    renderer->unbindFrameBuffer( ssaoBlurFBO );
}

