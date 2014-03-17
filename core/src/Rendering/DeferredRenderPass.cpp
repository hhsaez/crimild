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

#include "Rendering/DeferredRenderPass.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/VisibilitySet.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Components/SkinComponent.hpp"
#include "Components/JointComponent.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "Foundation/Log.hpp"

using namespace crimild;

DeferredRenderPass::DeferredRenderPass( void )
{
    
}

DeferredRenderPass::~DeferredRenderPass( void )
{
    
}

void DeferredRenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
#if 0
    RenderPass::render( renderer, renderQueue, camera );
#else
    renderToGBuffer( renderer, renderQueue, camera );
    composeFrame( renderer, renderQueue, camera );
//    RenderPass::render( renderer, _frameBufferOutput.get(), nullptr );
    
    computeSSAO( renderer );
//    RenderPass::render( renderer, _ssaoBufferOutput.get(), nullptr );
    applySSAO( renderer );
    
    RenderPass::render( renderer, _accumBufferOutput.get(), nullptr );
    
#endif
}

void DeferredRenderPass::buildAccumBuffer( int width, int height )
{
    _accumBuffer.set( new FrameBufferObject( width, height ) );
    _accumBuffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::DEPTH_16, RenderTarget::Output::RENDER, width, height ) );
    
    RenderTarget *colorTarget = new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height );
    _accumBufferOutput = colorTarget->getTexture();
    _accumBuffer->getRenderTargets().add( colorTarget );
}

void DeferredRenderPass::buildGBuffer( int width, int height )
{
    _gBuffer.set( new FrameBufferObject( width, height ) );
    RenderTarget *depthTarget = new RenderTarget( RenderTarget::Type::DEPTH_16, RenderTarget::Output::RENDER, width, height );
    _gBufferDepthOutput = depthTarget->getTexture();
    _gBuffer->getRenderTargets().add( depthTarget );
    
    RenderTarget *colorTarget = new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height );
    colorTarget->setUseFloatTexture( true );
    _gBufferColorOutput = colorTarget->getTexture();
    _gBuffer->getRenderTargets().add( colorTarget );
    
    RenderTarget *positionTarget = new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height );
    positionTarget->setUseFloatTexture( true );
    _gBufferPositionOutput = positionTarget->getTexture();
    _gBuffer->getRenderTargets().add( positionTarget );
    
    RenderTarget *normalTarget = new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height );
    normalTarget->setUseFloatTexture( true );
    _gBufferNormalOutput = normalTarget->getTexture();
    _gBuffer->getRenderTargets().add( normalTarget );
}

void DeferredRenderPass::renderToGBuffer( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    if ( _gBuffer == nullptr ) {
        int width = renderer->getScreenBuffer()->getWidth();
        int height = renderer->getScreenBuffer()->getHeight();
        buildGBuffer( width, height );
    }
    
    renderer->bindFrameBuffer( _gBuffer.get() );
    
    renderQueue->getOpaqueObjects().each( [&]( Geometry *geometry, int ) {
        
        RenderStateComponent *renderState = geometry->getComponent< RenderStateComponent >();
        renderState->foreachMaterial( [&]( Material *material ) {
            geometry->foreachPrimitive( [&]( Primitive *primitive ) mutable {
                ShaderProgram *program = renderer->getDeferredPassProgram();
                if ( program == nullptr ) {
                    Log::Error << "Deferred rendering not supported in your platform" << Log::End;
                    exit( 1 );
                    return;
                }
                
                renderer->bindProgram( program );
                
                // bind material properties
                renderer->bindMaterial( program, material );
                
                // bind joints and other skinning information
                SkinComponent *skinning = geometry->getComponent< SkinComponent >();
                if ( skinning != nullptr && skinning->hasJoints() ) {
                    skinning->foreachJoint( [&]( Node *node, unsigned int index ) {
                        JointComponent *joint = node->getComponent< JointComponent >();
                        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::JOINT_WORLD_MATRIX_UNIFORM + index ), joint->getWorldMatrix() );
                        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::JOINT_INVERSE_BIND_MATRIX_UNIFORM + index ), joint->getInverseBindMatrix() );
                    });
                }
                
                // bind vertex and index buffers
                renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
                renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
                
                renderer->applyTransformations( program, geometry, camera );
                
                // draw primitive
                renderer->drawPrimitive( program, primitive );
                
                renderer->restoreTransformations( program, geometry, camera );
                
                // unbind primitive buffers
                renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
                renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
                
                // unbind material properties
                renderer->unbindMaterial( program, material );
                
                renderer->unbindProgram( program );
            });
        });
    });
    
    renderer->unbindFrameBuffer( _gBuffer.get() );
}

void DeferredRenderPass::buildFrameBuffer(int width, int height )
{
    _frameBuffer.set( new FrameBufferObject( width, height ) );
    _frameBuffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::DEPTH_16, RenderTarget::Output::RENDER, width, height ) );
    
    RenderTarget *colorTarget = new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height );
    _frameBufferOutput = colorTarget->getTexture();
    _frameBuffer->getRenderTargets().add( colorTarget );
}

void DeferredRenderPass::composeFrame( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    if ( _frameBuffer == nullptr ) {
        int width = renderer->getScreenBuffer()->getWidth();
        int height = renderer->getScreenBuffer()->getHeight();
        buildFrameBuffer( width, height );
    }
    
    // bind buffer for ssao output
    renderer->bindFrameBuffer( _frameBuffer.get() );
    
    ShaderProgram *program = renderer->getShaderProgram( "deferredCompose" );
    if ( program == nullptr ) {
        Log::Error << "Cannot find shader program for composite deferred scene" << Log::End;
        exit( 1 );
        return;
    }
    
    // bind shader program first
    renderer->bindProgram( program );
    
    // bind lights
    renderQueue->getLights().each( [&]( Light *light, int ) {
        renderer->bindLight( program, light );
    });
    
    // bind framebuffer texture
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_COLOR_MAP_UNIFORM ), _gBufferColorOutput.get() );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM ), _gBufferPositionOutput.get() );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM ), _gBufferNormalOutput.get() );
    
    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), camera->getViewMatrix() );
    
    // bind vertex and index buffers
    renderer->bindVertexBuffer( program, getScreenPrimitive()->getVertexBuffer() );
    renderer->bindIndexBuffer( program, getScreenPrimitive()->getIndexBuffer() );
    
    // draw primitive
    renderer->drawPrimitive( program, getScreenPrimitive() );
    
    // unbind primitive buffers
    renderer->unbindVertexBuffer( program, getScreenPrimitive()->getVertexBuffer() );
    renderer->unbindIndexBuffer( program, getScreenPrimitive()->getIndexBuffer() );
    
    // unbind framebuffer texture
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_COLOR_MAP_UNIFORM ), _gBufferColorOutput.get() );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM ), _gBufferPositionOutput.get() );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM ), _gBufferNormalOutput.get() );
    
    // unbind lights
    renderQueue->getLights().each( [&]( Light *light, int ) {
        renderer->unbindLight( program, light );
    });
    
    // lastly, unbind the shader program
    renderer->unbindProgram( program );
    
    // unbind buffer for ssao output
    renderer->unbindFrameBuffer( _frameBuffer.get() );
}

void DeferredRenderPass::buildSSAOBuffer( int width, int height )
{
    _ssaoBuffer.set( new FrameBufferObject( width, height ) );
    _ssaoBuffer->getRenderTargets().add( new RenderTarget( RenderTarget::Type::DEPTH_16, RenderTarget::Output::RENDER, width, height ) );
    
    RenderTarget *colorTarget = new RenderTarget( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height );
    _ssaoBufferOutput = colorTarget->getTexture();
    _ssaoBuffer->getRenderTargets().add( colorTarget );
}

void DeferredRenderPass::computeSSAO( Renderer *renderer )
{
    if ( _ssaoBuffer == nullptr ) {
        int width = renderer->getScreenBuffer()->getWidth();
        int height = renderer->getScreenBuffer()->getHeight();
        buildSSAOBuffer( width, height );
    }
    
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
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM ), _gBufferPositionOutput.get() );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM ), _gBufferNormalOutput.get() );
    
    // bind vertex and index buffers
    renderer->bindVertexBuffer( program, getScreenPrimitive()->getVertexBuffer() );
    renderer->bindIndexBuffer( program, getScreenPrimitive()->getIndexBuffer() );
    
    // draw primitive
    renderer->drawPrimitive( program, getScreenPrimitive() );
    
    // unbind primitive buffers
    renderer->unbindVertexBuffer( program, getScreenPrimitive()->getVertexBuffer() );
    renderer->unbindIndexBuffer( program, getScreenPrimitive()->getIndexBuffer() );
    
    // unbind framebuffer texture
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_POSITION_MAP_UNIFORM ), _gBufferPositionOutput.get() );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::G_BUFFER_NORMAL_MAP_UNIFORM ), _gBufferNormalOutput.get() );
    
    // lastly, unbind the shader program
    renderer->unbindProgram( program );
    
    // unbind buffer for ssao output
    renderer->unbindFrameBuffer( _ssaoBuffer.get() );
}

void DeferredRenderPass::applySSAO( Renderer *renderer )
{
    if ( _accumBuffer == nullptr ) {
        int width = renderer->getScreenBuffer()->getWidth();
        int height = renderer->getScreenBuffer()->getHeight();
        buildAccumBuffer( width, height );
    }
    
    // bind buffer for ssao output
    renderer->bindFrameBuffer( _accumBuffer.get() );
    
    ShaderProgram *program = renderer->getShaderProgram( "ssaoBlend" );
    if ( program == nullptr ) {
        Log::Error << "Cannot find shader program for blending SSAO" << Log::End;
        exit( 1 );
        return;
    }
    
    // bind shader program first
    renderer->bindProgram( program );
    
    // bind framebuffer texture
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), _frameBufferOutput.get() );
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SSAO_MAP_UNIFORM ), _ssaoBufferOutput.get() );
    
    // bind vertex and index buffers
    renderer->bindVertexBuffer( program, getScreenPrimitive()->getVertexBuffer() );
    renderer->bindIndexBuffer( program, getScreenPrimitive()->getIndexBuffer() );
    
    // draw primitive
    renderer->drawPrimitive( program, getScreenPrimitive() );
    
    // unbind primitive buffers
    renderer->unbindVertexBuffer( program, getScreenPrimitive()->getVertexBuffer() );
    renderer->unbindIndexBuffer( program, getScreenPrimitive()->getIndexBuffer() );
    
    // unbind framebuffer texture
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ), _frameBufferOutput.get() );
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SSAO_MAP_UNIFORM ), _ssaoBufferOutput.get() );
    
    // lastly, unbind the shader program
    renderer->unbindProgram( program );
    
    // unbind buffer for ssao output
    renderer->unbindFrameBuffer( _accumBuffer.get() );
}

