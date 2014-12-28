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

#include "Rendering/ForwardRenderPass.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/VisibilitySet.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/ImageEffect.hpp"

#include "SceneGraph/Geometry.hpp"

#include "Components/RenderStateComponent.hpp"
#include "Components/SkinComponent.hpp"
#include "Components/JointComponent.hpp"

#include "Primitives/QuadPrimitive.hpp"

#include "Foundation/Log.hpp"

using namespace crimild;

ForwardRenderPass::ForwardRenderPass( void )
{
    
}

ForwardRenderPass::~ForwardRenderPass( void )
{
    
}

void ForwardRenderPass::render( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera )
{
    computeShadowMaps( renderer, renderQueue, camera );
    
    if ( _forwardPassBuffer == nullptr ) {
        int width = renderer->getScreenBuffer()->getWidth();
        int height = renderer->getScreenBuffer()->getHeight();
        _forwardPassBuffer = std::make_shared< FrameBufferObject >( width, height );
        auto result = std::make_shared< RenderTarget >( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height );
        _forwardPassResult = result->getTexture();
        _forwardPassBuffer->getRenderTargets().add( result );
        _forwardPassBuffer->getRenderTargets().add( std::make_shared< RenderTarget >( RenderTarget::Type::DEPTH_24, RenderTarget::Output::RENDER, width, height ) );
        
        buildAccumBuffer( width, height );
    }
    
#if 1
    renderer->bindFrameBuffer( _forwardPassBuffer );
    
    renderShadedObjects( renderer, renderQueue, camera );
    renderTranslucentObjects( renderer, renderQueue, camera );
    
    renderer->unbindFrameBuffer( _forwardPassBuffer );
    
    if ( getImageEffects().isEmpty() ) {
        RenderPass::render( renderer, _forwardPassResult, nullptr );
    }
    else {
        Texture *inputs[] = {
            _forwardPassResult.get(),
        };
        
        getImageEffects().each( [&]( ImageEffectPtr const &effect, int ) {
            effect->apply( renderer, 4, inputs, getScreenPrimitive(), _accumBuffer );
        });
        
        RenderPass::render( renderer, _accumBufferOutput, nullptr );
    }

    // UI elements need to be render on top of any image effect
    renderScreenObjects( renderer, renderQueue, camera );

#else
    for ( auto it : _shadowMaps ) {
        if ( it.second != nullptr ) {
            RenderPass::render( renderer, it.second->getTexture(), nullptr );
        }
    }
#endif
}

void ForwardRenderPass::buildAccumBuffer( int width, int height )
{
    _accumBuffer = std::make_shared< FrameBufferObject >( width, height );
    _accumBuffer->getRenderTargets().add( std::make_shared< RenderTarget >( RenderTarget::Type::DEPTH_16, RenderTarget::Output::RENDER, width, height ) );
    
    auto colorTarget = std::make_shared< RenderTarget >( RenderTarget::Type::COLOR_RGBA, RenderTarget::Output::TEXTURE, width, height );
    _accumBufferOutput = colorTarget->getTexture();
    _accumBuffer->getRenderTargets().add( colorTarget );
}

void ForwardRenderPass::computeShadowMaps( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera )
{
    auto program = renderer->getDepthProgram();
    if ( program == nullptr ) {
        return;
    }
    
    // bind shader program first
    renderer->bindProgram( program );
    
    renderQueue->each( [&]( LightPtr const &light, int ) {
        
        if ( !light->shouldCastShadows() ) {
            return;
        }
        
        auto map = _shadowMaps[ light ];
        if ( map == nullptr ) {
            map = std::make_shared< ShadowMap >( light );
            map->getBuffer()->setClearColor( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
            map->setLightProjectionMatrix( light->computeProjectionMatrix() );
            _shadowMaps[ light ] = map;
        }
        
        map->setLightViewMatrix( light->computeViewMatrix() );
        
        renderer->bindFrameBuffer( map->getBuffer() );
        
        renderer->setAlphaState( AlphaState::DISABLED );
        renderer->setDepthState( DepthState::ENABLED );

        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LINEAR_DEPTH_CONSTANT_UNIFORM ), map->getLinearDepthConstant() );

        renderQueue->each( renderQueue->getOpaqueObjects(), [&]( MaterialPtr const &material, RenderQueue::PrimitiveMap const &primitives ) {
            for ( auto it : primitives ) {
                auto primitive = it.first;

                renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
                renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );

                for ( auto geometryIt : it.second ) {
                    renderer->applyTransformations( program, map->getLightProjectionMatrix(), map->getLightViewMatrix(), geometryIt.second.computeModelMatrix(), geometryIt.second.computeNormalMatrix() );
                    renderer->drawPrimitive( program, primitive );
                }

                // unbind primitive buffers
                renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
                renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
            }
        });
        
        renderer->unbindFrameBuffer( map->getBuffer() );
    });
    
    // unbind the shader program
    renderer->unbindProgram( program );
}

void ForwardRenderPass::renderShadedObjects( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera )
{
    auto program = renderer->getForwardPassProgram();

    // bind program
    renderer->bindProgram( program );

    const Matrix4f &projection = camera->getProjectionMatrix();
    const Matrix4f &view = camera->getViewMatrix();
    
    // bind shadow maps
    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::USE_SHADOW_MAP_UNIFORM ), false );
    for ( auto it : _shadowMaps ) {
        if ( it.second != nullptr ) {
            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_SOURCE_PROJECTION_MATRIX_UNIFORM ), it.second->getLightProjectionMatrix() );
            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_SOURCE_VIEW_MATRIX_UNIFORM ), it.second->getLightViewMatrix() );
            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::USE_SHADOW_MAP_UNIFORM ), true );
            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LINEAR_DEPTH_CONSTANT_UNIFORM ), it.second->getLinearDepthConstant() );
            renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SHADOW_MAP_UNIFORM ), it.second->getTexture() );
        }
    }
    
    // bind lights
    renderQueue->each( [&]( LightPtr const &light, int ) {
        renderer->bindLight( program, light );
    });
    
    renderQueue->each( renderQueue->getOpaqueObjects(), [&]( MaterialPtr const &material, RenderQueue::PrimitiveMap const &primitives ) {
        // bind material properties
        renderer->bindMaterial( program, material );

        for ( auto it : primitives ) {
            auto primitive = it.first;

            // bind vertex and index buffers
            renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
            renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );

            for ( auto geometryIt : it.second ) {
                auto world = geometryIt.second;
                renderer->applyTransformations( program, projection, view, world.computeModelMatrix(), world.computeNormalMatrix() );
                renderer->drawPrimitive( program, primitive );
            }
            
            // unbind primitive buffers
            renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
            renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
        }

        // unbind material properties
        renderer->unbindMaterial( program, material );

    });

    // unbind lights
    renderQueue->each( [&]( LightPtr const &light, int ) {
        renderer->unbindLight( program, light );
    });
    
    // unbind shadow maps
    for ( auto it : _shadowMaps ) {
        if ( it.second != nullptr ) {
            renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SHADOW_MAP_UNIFORM ), it.second->getTexture() );
        }
    }

    // unbind program
    renderer->unbindProgram( program );
}

