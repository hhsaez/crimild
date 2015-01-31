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

#include "Rendering/BasicRenderPass.hpp"

#include "Foundation/Log.hpp"

using namespace crimild;

BasicRenderPass::BasicRenderPass( void )
{

}

BasicRenderPass::~BasicRenderPass( void )
{

}

void BasicRenderPass::render( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera )
{
    render( renderer, renderQueue, camera, renderQueue->getOpaqueObjects() );
    render( renderer, renderQueue, camera, renderQueue->getTranslucentObjects() );
}

void BasicRenderPass::render( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera, RenderQueue::MaterialMap const &objects )
{
    const Matrix4f &projection = camera->getProjectionMatrix();
    const Matrix4f &view = camera->getViewMatrix();
    
    renderQueue->each( objects, [&]( MaterialPtr const &material, RenderQueue::PrimitiveMap const &primitives ) {
        auto program = material->getProgram() ? material->getProgram() : renderer->getShaderProgram( "basic" );
        if ( program == nullptr ) {
            Log::Error << "No valid program for batch" << Log::End;
            return;
        }
        
        // bind program
        renderer->bindProgram( program );
        
        // bind material properties
        renderer->bindMaterial( program, material );
        
        for ( auto primitiveIt : primitives ) {
            auto primitive = primitiveIt.first;
            
            // bind vertex and index buffers
            renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
            renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );

            auto &vf = primitive->getVertexBuffer()->getVertexFormat();
            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::USE_NORMAL_ATTRIBUTE ), vf.hasNormals() );
            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::USE_TEXTURE_COORD_ATTRIBUTE ), vf.hasTextureCoords() );
            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::USE_COLOR_ATTRIBUTE ), vf.hasColors() );
            
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
        
        // unbind program
        renderer->unbindProgram( program );
    });
}

