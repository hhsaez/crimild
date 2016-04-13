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

#include "Rendering/RenderPass.hpp"
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

RenderPass::RenderPass( void )
    : _screen( crimild::alloc< QuadPrimitive >( 2.0f, 2.0f, VertexFormat::VF_P3_UV2, Vector2f( 0.0f, 1.0f ), Vector2f( 1.0f, -1.0f ) ) )
{

}

RenderPass::~RenderPass( void )
{

}

void RenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    render( renderer, renderQueue, camera, renderQueue->getShadedObjects() );
    renderOpaqueObjects( renderer, renderQueue, camera );
    renderTranslucentObjects( renderer, renderQueue, camera );
    renderScreenObjects( renderer, renderQueue, camera );
}

void RenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera, RenderQueue::Renderables const &objects )
{
    const Matrix4f &projection = renderQueue->getProjectionMatrix();
    const Matrix4f &view = renderQueue->getViewMatrix();
    
    renderQueue->each( objects, [&]( Material *material, RenderQueue::PrimitiveMap const &primitives ) {
        if ( material->getProgram() == nullptr ) {
            material->setProgram( renderer->getShaderProgram( Renderer::SHADER_PROGRAM_LIT_TEXTURE ) );
        }
        auto program = material->getProgram();
        assert( program != nullptr && "No valid program to render batch" );
        
        // bind program
        renderer->bindProgram( program );
        
        // bind lights
        renderQueue->each( [&]( Light *light, int ) {
            renderer->bindLight( program, light );
        });
        
        // bind material properties
        renderer->bindMaterial( program, material );
        
        for ( auto primitiveIt : primitives ) {
            auto primitive = primitiveIt.first;
            
            // bind vertex and index buffers
            renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
            renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
            
            for ( auto geometryIt : primitiveIt.second ) {
                auto &model = geometryIt.second;
                Matrix4f normal = model;
                normal[ 12 ] = 0.0f;
                normal[ 13 ] = 0.0f;
                normal[ 14 ] = 0.0f;
                
                renderer->applyTransformations( program, projection, view, model, normal );
                renderer->drawPrimitive( program, primitive );
            }
            
            // unbind primitive buffers
            renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
            renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
        }
        
        // unbind material properties
        renderer->unbindMaterial( program, material );
        
        // unbind lights
        renderQueue->each( [&]( Light *light, int ) {
            renderer->unbindLight( program, light );
        });
        
        // unbind program
        renderer->unbindProgram( program );
    });
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

void RenderPass::renderOpaqueObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Render Opaque Objects" )

    render( renderer, renderQueue, camera, renderQueue->getOpaqueObjects() );
}

void RenderPass::renderTranslucentObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Render Translucent Objects" )
    
    render( renderer, renderQueue, camera, renderQueue->getTranslucentObjects() );
}

void RenderPass::renderScreenObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Render Screen Objects" )
    
    const Matrix4f &projection = camera->getOrthographicMatrix();
    Matrix4f view;
    view.makeIdentity();
    
    renderQueue->each( renderQueue->getScreenObjects(), [&]( Material *material, RenderQueue::PrimitiveMap const &primitives ) {
        if ( material->getProgram() == nullptr ) {
            material->setProgram( renderer->getShaderProgram( Renderer::SHADER_PROGRAM_LIT_TEXTURE ) );
        }
        auto program = material->getProgram();
        assert( program != nullptr && "No valid program to render batch" );
        
        // bind program
        renderer->bindProgram( program );
        
        // bind lights
        renderQueue->each( [&]( Light *light, int ) {
            renderer->bindLight( program, light );
        });
        
        // bind material properties
        renderer->bindMaterial( program, material );
        
        for ( auto it : primitives ) {
            auto primitive = it.first;
            
            // bind vertex and index buffers
            renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
            renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
            
            for ( auto geometryIt : it.second ) {
                auto &model = geometryIt.second;
                auto normal = model;
                normal[ 12 ] = 0.0f;
                normal[ 13 ] = 0.0f;
                normal[ 14 ] = 0.0f;

                renderer->applyTransformations( program, projection, view, model, normal );
                renderer->drawPrimitive( program, primitive );
            }
            
            // unbind primitive buffers
            renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
            renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
        }
        
        // unbind material properties
        renderer->unbindMaterial( program, material );
        
        // unbind lights
        renderQueue->each( [&]( Light *light, int ) {
            renderer->unbindLight( program, light );
        });
        
        // unbind program
        renderer->unbindProgram( program );
    });
    
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
    
    auto sBuffer = crimild::alloc< FrameBufferObject >( width, height );
    sBuffer->getRenderTargets().add( S_BUFFER_DEPTH_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER, width, height ) );
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
    
    auto dBuffer = crimild::alloc< FrameBufferObject >( width, height );
    dBuffer->getRenderTargets().add( D_BUFFER_DEPTH_TARGET_NAME, crimild::alloc< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER, width, height ) );
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

