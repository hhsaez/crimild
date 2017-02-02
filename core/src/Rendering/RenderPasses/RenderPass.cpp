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

#include "RenderPass.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/ImageEffects/ImageEffect.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/Profiler.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;

constexpr const char *RenderPass::G_BUFFER_NAME;
constexpr const char *RenderPass::G_BUFFER_DIFFUSE_TARGET_NAME;
constexpr const char *RenderPass::G_BUFFER_POSITION_TARGET_NAME;
constexpr const char *RenderPass::G_BUFFER_NORMAL_TARGET_NAME;
constexpr const char *RenderPass::G_BUFFER_VIEW_SPACE_NORMAL_TARGET_NAME;
constexpr const char *RenderPass::G_BUFFER_DEPTH_TARGET_NAME;
constexpr const char *RenderPass::S_BUFFER_NAME;
constexpr const char *RenderPass::S_BUFFER_COLOR_TARGET_NAME;
constexpr const char *RenderPass::S_BUFFER_DEPTH_TARGET_NAME;
constexpr const char *RenderPass::D_BUFFER_NAME;
constexpr const char *RenderPass::D_BUFFER_COLOR_TARGET_NAME;
constexpr const char *RenderPass::D_BUFFER_DEPTH_TARGET_NAME;

RenderPass::RenderPass( void )
    : _screen( crimild::alloc< QuadPrimitive >( 2.0f, 2.0f, VertexFormat::VF_P3_UV2, Vector2f( 0.0f, 1.0f ), Vector2f( 1.0f, -1.0f ) ) )
{

}

RenderPass::~RenderPass( void )
{

}

void RenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{

}

void RenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera, RenderQueue::Renderables const &objects )
{

}

void RenderPass::render( Renderer *renderer, Texture *texture, ShaderProgram *defaultProgram )
{
    auto program = defaultProgram;
    if ( program == nullptr ) {
        program = renderer->getShaderProgram( Renderer::SHADER_PROGRAM_SCREEN_TEXTURE );
    }
    
    assert( program && "No valid program to render texture" );
    
    // bind shader program first
    renderer->bindProgram( program );
    
    // bind framebuffer texture
    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ), texture );
     
    // bind vertex and index buffers
    renderer->bindVertexBuffer( program, _screen->getVertexBuffer() );
    renderer->bindIndexBuffer( program, _screen->getIndexBuffer() );

    Matrix4f mMatrix;
    mMatrix.makeIdentity();
    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), mMatrix );
     
    // draw primitive
    renderer->drawPrimitive( program, crimild::get_ptr( _screen ) );
     
    // unbind primitive buffers
    renderer->unbindVertexBuffer( program, _screen->getVertexBuffer() );
    renderer->unbindIndexBuffer( program, _screen->getIndexBuffer() );
     
    // unbind framebuffer texture
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ), texture );
     
    // lastly, unbind the shader program
    renderer->unbindProgram( program );
}

FrameBufferObject *RenderPass::getSBuffer( Renderer *renderer )
{
    // do not cache this value since it probably changes when applying image effects
    auto fbo = renderer->getFrameBuffer( S_BUFFER_NAME );
    if ( fbo != nullptr ) {
        return fbo;
    }
    
    int width = renderer->getScreenBuffer()->getWidth();
    int height = renderer->getScreenBuffer()->getHeight();
    
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Creating S_BUFFER with size ", width, "x", height );
    
    auto sBuffer = crimild::alloc< FrameBufferObject >( width, height );
#ifdef CRIMILD_PLATFORM_DESKTOP
    sBuffer->getRenderTargets().add( S_BUFFER_DEPTH_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER_AND_TEXTURE, width, height, true ) );
#else
    sBuffer->getRenderTargets().add( S_BUFFER_DEPTH_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER, width, height ) );
#endif
    sBuffer->getRenderTargets().add( S_BUFFER_COLOR_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height ) );
    renderer->setFrameBuffer( S_BUFFER_NAME, sBuffer );
    
    return crimild::get_ptr( sBuffer );
}

FrameBufferObject *RenderPass::getDBuffer( Renderer *renderer )
{
    // do not cache this value since it probably changes when applying image effects
    auto fbo = renderer->getFrameBuffer( D_BUFFER_NAME );
    if ( fbo != nullptr ) {
        return fbo;
    }
    
    int width = renderer->getScreenBuffer()->getWidth();
    int height = renderer->getScreenBuffer()->getHeight();
    
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Creating D_BUFFER with size ", width, "x", height );
    
    auto dBuffer = crimild::alloc< FrameBufferObject >( width, height );
#ifdef CRIMILD_PLATFORM_DESKTOP
    dBuffer->getRenderTargets().add( D_BUFFER_DEPTH_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER_AND_TEXTURE, width, height, true ) );
#else
    dBuffer->getRenderTargets().add( D_BUFFER_DEPTH_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER, width, height ) );
#endif
    dBuffer->getRenderTargets().add( D_BUFFER_COLOR_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height ) );
    renderer->setFrameBuffer( D_BUFFER_NAME, dBuffer );
    
    return crimild::get_ptr( dBuffer );
}

void RenderPass::applyImageEffects( Renderer *renderer, Camera *camera )
{
	auto self = this;
	getImageEffects().forEach( [self, renderer, camera]( ImageEffect *effect, int ) {
		if ( effect->isEnabled() ) {
            auto destBuffer = self->getDBuffer( renderer );

			effect->compute( renderer, camera );
			
			renderer->bindFrameBuffer( destBuffer );
			effect->apply( renderer, camera );
			renderer->unbindFrameBuffer( destBuffer );

			self->swapSDBuffers( renderer );
		}
	});
}

void RenderPass::swapSDBuffers( Renderer *renderer )
{
	auto source = crimild::retain( getSBuffer( renderer ) );
	auto destination = crimild::retain( getDBuffer( renderer ) );

	renderer->setFrameBuffer( RenderPass::S_BUFFER_NAME, destination );
	renderer->setFrameBuffer( RenderPass::D_BUFFER_NAME, source );
}

