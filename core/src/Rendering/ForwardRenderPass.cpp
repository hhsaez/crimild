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
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/ShaderProgram.hpp"

#include "Rendering/ImageEffects/ImageEffect.hpp"

#include "SceneGraph/Geometry.hpp"

#include "Components/RenderStateComponent.hpp"
#include "Components/SkinComponent.hpp"
#include "Components/JointComponent.hpp"

#include "Primitives/QuadPrimitive.hpp"

#include "Foundation/Log.hpp"
#include "Foundation/Profiler.hpp"

#include "Simulation/AssetManager.hpp"

using namespace crimild;

ForwardRenderPass::ForwardRenderPass( void )
{
    
}

ForwardRenderPass::~ForwardRenderPass( void )
{
    
}

void ForwardRenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Forward Render Pass" )

#ifdef CRIMILD_PLATFORM_DESKTOP
    computeShadowMaps( renderer, renderQueue, camera );

    auto sBuffer = getSBuffer( renderer );

    renderer->bindFrameBuffer( sBuffer );
#endif

    renderShadedObjects( renderer, renderQueue, camera );
    renderNonShadedObjects( renderer, renderQueue, camera );
    renderTranslucentObjects( renderer, renderQueue, camera );

#ifdef CRIMILD_PLATFORM_DESKTOP
    renderer->unbindFrameBuffer( sBuffer );

	applyImageEffects( renderer, camera );
#endif

    // UI elements need to be render on top of any image effect
    renderScreenObjects( renderer, renderQueue, camera );
}

ShaderProgram *ForwardRenderPass::getForwardProgram( void )
{
    if ( _forwardProgram == nullptr ) {
        _forwardProgram = AssetManager::getInstance()->get< ShaderProgram >( Renderer::SHADER_PROGRAM_RENDER_PASS_FORWARD );
    }
    
    assert( _forwardProgram != nullptr );
    return _forwardProgram;
}

void ForwardRenderPass::computeShadowMaps( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Compute Shadows" )

    auto program = renderer->getShaderProgram( Renderer::SHADER_PROGRAM_DEPTH );
    if ( program == nullptr ) {
		Log::Error << "No shader program with name '" << Renderer::SHADER_PROGRAM_DEPTH << "'" << Log::End;
        return;
    }
    
    // bind shader program first
    renderer->bindProgram( program );
    
    renderQueue->each( [&]( Light *light, int ) {
        
        if ( !light->shouldCastShadows() ) {
            return;
        }
        
        auto map = _shadowMaps[ light ];
        if ( map == nullptr ) {
            map = crimild::alloc< ShadowMap >( light );
            map->getBuffer()->setClearColor( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
            map->setLightProjectionMatrix( light->computeProjectionMatrix() );
            _shadowMaps[ light ] = map;
        }
        
        map->setLightViewMatrix( light->computeViewMatrix() );
        
        renderer->bindFrameBuffer( map->getBuffer() );
        
        renderer->setAlphaState( AlphaState::DISABLED );
        renderer->setDepthState( DepthState::ENABLED );

        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LINEAR_DEPTH_CONSTANT_UNIFORM ), map->getLinearDepthConstant() );

        renderQueue->each( renderQueue->getShadowCasters(), [&]( Material *material, RenderQueue::PrimitiveMap const &primitives ) {
            for ( auto it : primitives ) {
                auto primitive = it.first;

                renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
                renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );

                for ( auto geometryIt : it.second ) {
                    renderer->applyTransformations( program, map->getLightProjectionMatrix(), map->getLightViewMatrix(), geometryIt.second );
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

void ForwardRenderPass::renderShadedObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Render Shaded Objects" )
    
    auto program = getForwardProgram();

    // bind program
    renderer->bindProgram( program );

    auto projection = renderQueue->getProjectionMatrix();
    auto view = renderQueue->getViewMatrix();
    
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
    renderQueue->each( [&]( Light *light, int ) {
        renderer->bindLight( program, light );
    });
    
    renderQueue->each( renderQueue->getShadedObjects(), [&]( Material *material, RenderQueue::PrimitiveMap const &primitives ) {
        CRIMILD_PROFILE( "Apply Materials" )

        // bind material properties
        renderer->bindMaterial( program, material );

        for ( auto it : primitives ) {
            CRIMILD_PROFILE( "Bind Primitive" )

            auto primitive = it.first;

            // bind vertex and index buffers
            renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
            renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );

            for ( auto geometryIt : it.second ) {
                CRIMILD_PROFILE( "Draw Primitive" )
                renderer->applyTransformations( program, projection, view, geometryIt.second );
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
    renderQueue->each( [&]( Light *light, int ) {
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

void ForwardRenderPass::renderNonShadedObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Render Non-Shaded Objects" )
    
    auto program = getForwardProgram();
    
    // bind program
    renderer->bindProgram( program );
    
    auto projection = renderQueue->getProjectionMatrix();
    auto view = renderQueue->getViewMatrix();
    
    // disable shadow maps
    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::USE_SHADOW_MAP_UNIFORM ), false );
    
    // bind lights
    renderQueue->each( [&]( Light *light, int ) {
        renderer->bindLight( program, light );
    });
    
    renderQueue->each( renderQueue->getOpaqueObjects(), [&]( Material *material, RenderQueue::PrimitiveMap const &primitives ) {
        CRIMILD_PROFILE( "Apply Materials" )
        
        // bind material properties
        renderer->bindMaterial( program, material );
        
        for ( auto it : primitives ) {
            CRIMILD_PROFILE( "Bind Primitive" )
            
            auto primitive = it.first;
            
            // bind vertex and index buffers
            renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
            renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
            
            for ( auto geometryIt : it.second ) {
                CRIMILD_PROFILE( "Draw Primitive" )
                renderer->applyTransformations( program, projection, view, geometryIt.second );
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
    renderQueue->each( [&]( Light *light, int ) {
        renderer->unbindLight( program, light );
    });
    
    // unbind program
    renderer->unbindProgram( program );
}

