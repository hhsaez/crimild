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

using namespace crimild;
using namespace crimild::gl3;

SSAOImageEffect::SSAOImageEffect( void )
{
    
}

SSAOImageEffect::~SSAOImageEffect( void )
{
    
}

void SSAOImageEffect::apply( crimild::Renderer *renderer, int inputCount, Texture **inputs, Primitive *primitive, FrameBufferObject *output )
{
    if ( _ssaoBuffer == nullptr ) {
        int width = renderer->getScreenBuffer()->getWidth();
        int height = renderer->getScreenBuffer()->getHeight();
        buildSSAOBuffer( width, height );
    }
    
    if ( inputCount < 4 ) {
        Log::Error << "Not enough input data for SSAO effect" << Log::End;
        return;
    }
    
    computeSSAO( renderer, inputs[ 2 ], inputs[ 3 ], primitive );
    applySSAO( renderer, inputs[ 0 ], _ssaoBufferOutput.get(), primitive, output );
}

void SSAOImageEffect::buildSSAOBuffer( int width, int height )
{
    _ssaoBuffer.set( new FrameBufferObject( width, height ) );
    _ssaoBuffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::DEPTH_16, RenderTarget::Output::RENDER, width, height ) );
    
    RenderTarget *colorTarget = new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height );
    _ssaoBufferOutput = colorTarget->getTexture();
    _ssaoBuffer->getRenderTargets().add( colorTarget );
}

void SSAOImageEffect::computeSSAO( Renderer *renderer, Texture *srcPositions, Texture *srcNormals, Primitive *primitive )
{
    // bind buffer for ssao output
    renderer->bindFrameBuffer( _ssaoBuffer.get() );
    
    ShaderProgram *program = renderer->getShaderProgram( "ssao" );
    if ( program == nullptr ) {
        Log::Error << "Cannot find shader program for SSAO" << Log::End;
        exit( 1 );
        return;
    }
    
    // bind shader program first
    renderer->bindProgram( program );
    
    // bind framebuffer texture
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM ), srcPositions );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM ), srcNormals );
    
    // bind vertex and index buffers
    renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
    renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
    
    // draw primitive
    renderer->drawPrimitive( program, primitive );
    
    // unbind primitive buffers
    renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
    renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
    
    // unbind framebuffer texture
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM ), srcPositions );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM ), srcNormals );
    
    // lastly, unbind the shader program
    renderer->unbindProgram( program );
    
    // unbind buffer for ssao output
    renderer->unbindFrameBuffer( _ssaoBuffer.get() );
}

void SSAOImageEffect::applySSAO( Renderer *renderer, Texture *srcImage, Texture *ssaoMap, Primitive *primitive, FrameBufferObject *output )
{
    // bind buffer for ssao output
    renderer->bindFrameBuffer( output );
    
    ShaderProgram *program = renderer->getShaderProgram( "ssaoBlend" );
    if ( program == nullptr ) {
        Log::Error << "Cannot find shader program for blending SSAO" << Log::End;
        exit( 1 );
        return;
    }
    
    // bind shader program first
    renderer->bindProgram( program );
    
    // bind framebuffer texture
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), srcImage );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SSAO_MAP_UNIFORM ), ssaoMap );
    
    // bind vertex and index buffers
    renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
    renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
    
    // draw primitive
    renderer->drawPrimitive( program, primitive );
    
    // unbind primitive buffers
    renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
    renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
    
    // unbind framebuffer texture
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), srcImage );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SSAO_MAP_UNIFORM ), ssaoMap );
    
    // lastly, unbind the shader program
    renderer->unbindProgram( program );
    
    // unbind buffer for ssao output
    renderer->unbindFrameBuffer( output );
}

