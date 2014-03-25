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

#include "GlowImageEffect.hpp"
#include "Rendering/GL3/Programs/BlurShaderProgram.hpp"

using namespace crimild;
using namespace crimild::gl3;

GlowImageEffect::GlowImageEffect( void )
{
    _alphaState.set( new AlphaState( true ) );
    _depthState.set( new DepthState( false ) );
    
    _amount = 10;
    _glowMapSize = 128;
}

GlowImageEffect::~GlowImageEffect( void )
{
    
}

void GlowImageEffect::apply( crimild::Renderer *renderer, int inputCount, Texture **inputs, Primitive *primitive, FrameBufferObject *output )
{
    if ( _glowMapBuffer == nullptr ) {
        buildGlowBuffer( getGlowMapSize(), getGlowMapSize() );
    }
    
    computeGlow( renderer, inputs[ 0 ], primitive );
    applyGlow( renderer, inputs[ 0 ], _glowMap.get(), primitive, output );
}

void GlowImageEffect::buildGlowBuffer( int width, int height )
{
    _glowMapBuffer.set( new FrameBufferObject( width, height ) );
    _glowMapBuffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::DEPTH_16, RenderTarget::Output::RENDER, width, height ) );
    
    RenderTarget *glowTarget = new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height );
    _glowMap = glowTarget->getTexture();
    _glowMapBuffer->getRenderTargets().add( glowTarget );
}

void GlowImageEffect::computeGlow( crimild::Renderer *renderer, Texture *srcImage, Primitive *primitive )
{
    renderer->bindFrameBuffer( _glowMapBuffer.get() );
    
    ShaderProgram *program = renderer->getShaderProgram( "blur" );
    if ( program == nullptr ) {
        Log::Error << "Cannot find shader program for glow effect" << Log::End;
        exit( 1 );
        return;
    }
    
    renderer->setAlphaState( _alphaState.get() );
    renderer->setDepthState( _depthState.get() );
    
    renderer->bindProgram( program );
    renderer->bindTexture( program->getLocation( BlurShaderProgram::UNIFORM_BLUR_INPUT ), srcImage );
    
    renderer->bindUniform( program->getLocation( BlurShaderProgram::UNIFORM_BLUR_AMOUNT ), getAmount() );
    renderer->bindUniform( program->getLocation( BlurShaderProgram::UNIFORM_BLUR_SCALE ), 1.0f );
    renderer->bindUniform( program->getLocation( BlurShaderProgram::UNIFORM_BLUR_STRENTH ), 0.5f );
    
    renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
    renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
    
    for ( int i = 0; i < 2; i++ ) {
        renderer->bindUniform( program->getLocation( BlurShaderProgram::UNIFORM_BLUR_ORIENTATION ), i );
        
        renderer->drawPrimitive( program, primitive );
    }
    
    renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
    renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
    
    renderer->unbindTexture( program->getLocation( BlurShaderProgram::UNIFORM_BLUR_INPUT ), srcImage );
    renderer->unbindProgram( program );
    
    renderer->unbindFrameBuffer( _glowMapBuffer.get() );
}

void GlowImageEffect::applyGlow( Renderer *renderer, Texture *srcImage, Texture *dstImage, Primitive *primitive, FrameBufferObject *output )
{
    // bind buffer for ssao output
    renderer->bindFrameBuffer( output );
    
    ShaderProgram *program = renderer->getShaderProgram( "blend" );
    if ( program == nullptr ) {
        Log::Error << "Cannot find shader program for blending" << Log::End;
        exit( 1 );
        return;
    }
    
    // bind shader program first
    renderer->bindProgram( program );
    
    // bind framebuffer texture
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::BLEND_SRC_MAP_UNIFORM ), srcImage );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::BLEND_DST_MAP_UNIFORM ), dstImage );
    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::BLEND_MODE_UNIFORM ), 1 );
    
    // bind vertex and index buffers
    renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
    renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
    
    // draw primitive
    renderer->drawPrimitive( program, primitive );
    
    // unbind primitive buffers
    renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
    renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
    
    // unbind framebuffer texture
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::BLEND_SRC_MAP_UNIFORM ), srcImage );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::BLEND_DST_MAP_UNIFORM ), dstImage );
    
    // lastly, unbind the shader program
    renderer->unbindProgram( program );
    
    // unbind buffer for ssao output
    renderer->unbindFrameBuffer( output );
}


