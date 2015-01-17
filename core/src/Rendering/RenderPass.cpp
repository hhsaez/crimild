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

#include "SceneGraph/Geometry.hpp"

#include "Components/RenderStateComponent.hpp"

#include "Primitives/QuadPrimitive.hpp"

#include "Foundation/Log.hpp"

using namespace crimild;

RenderPass::RenderPass( void )
    : _screen( crimild::alloc< QuadPrimitive >( 2.0f, 2.0f, VertexFormat::VF_P3_UV2, Vector2f( 0.0f, 1.0f ), Vector2f( 1.0f, -1.0f ) ) ),
      _imageEffects( crimild::alloc< SharedObjectList< ImageEffect >>() )
{

}

RenderPass::~RenderPass( void )
{

}

void RenderPass::render( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera )
{
    renderOpaqueObjects( renderer, renderQueue, camera );
    renderTranslucentObjects( renderer, renderQueue, camera );
    renderScreenObjects( renderer, renderQueue, camera );
}

void RenderPass::render( SharedPointer< Renderer > const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera, RenderQueue::MaterialMap const &objects )
{
    const Matrix4f &projection = camera->getProjectionMatrix();
    const Matrix4f &view = camera->getViewMatrix();
    
    renderQueue->each( objects, [&]( MaterialPtr const &material, RenderQueue::PrimitiveMap const &primitives ) {
        auto program = material->getProgram() ? material->getProgram() : renderer->getShaderProgram( "phong" );
        if ( program == nullptr ) {
            Log::Error << "No valid program for batch" << Log::End;
            return;
        }
        
        // bind program
        renderer->bindProgram( program );
        
        // bind lights
        renderQueue->each( [&]( LightPtr const &light, int ) {
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
                Matrix4f model = geometryIt.second.computeModelMatrix();
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
        renderQueue->each( [&]( LightPtr const &light, int ) {
            renderer->unbindLight( program, light );
        });
        
        // unbind program
        renderer->unbindProgram( program );
    });
}

void RenderPass::render( RendererPtr const &renderer, TexturePtr const &texture, ShaderProgramPtr const &defaultProgram )
{
    auto program = defaultProgram;
    if ( program == nullptr ) {
        program = renderer->getFallbackProgram( nullptr, nullptr, nullptr );
        if ( program == nullptr ) {
            return;
        }
    }
     
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
    renderer->drawPrimitive( program, _screen );
     
    // unbind primitive buffers
    renderer->unbindVertexBuffer( program, _screen->getVertexBuffer() );
    renderer->unbindIndexBuffer( program, _screen->getIndexBuffer() );
     
    // unbind framebuffer texture
    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ), texture );
     
    // lastly, unbind the shader program
    renderer->unbindProgram( program );
}

void RenderPass::renderOpaqueObjects( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera )
{
    render( renderer, renderQueue, camera, renderQueue->getOpaqueObjects() );
}

void RenderPass::renderTranslucentObjects( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera )
{
    render( renderer, renderQueue, camera, renderQueue->getTranslucentObjects() );
}

void RenderPass::renderScreenObjects( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera )
{
    const Matrix4f &projection = camera->getOrthographicMatrix();
    Matrix4f view;
    view.makeIdentity();
    
    renderQueue->each( renderQueue->getScreenObjects(), [&]( MaterialPtr const &material, RenderQueue::PrimitiveMap const &primitives ) {
        auto program = material->getProgram() ? material->getProgram() : renderer->getShaderProgram( "phong" );
        if ( program == nullptr ) {
            Log::Error << "No valid program for batch" << Log::End;
            return;
        }
        
        // bind program
        renderer->bindProgram( program );
        
        // bind lights
        renderQueue->each( [&]( LightPtr const &light, int ) {
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
                auto model = geometryIt.second.computeModelMatrix();
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
        renderQueue->each( [&]( LightPtr const &light, int ) {
            renderer->unbindLight( program, light );
        });
        
        // unbind program
        renderer->unbindProgram( program );
    });
    
}

