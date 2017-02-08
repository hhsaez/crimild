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

#include "StandardRenderPass.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/SkinnedMesh.hpp"
#include "Rendering/ImageEffects/ImageEffect.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/Profiler.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;

StandardRenderPass::StandardRenderPass( void )
{
    // TODO: fix this
    setShadowMappingEnabled( false );
}

StandardRenderPass::~StandardRenderPass( void )
{
    
}

void StandardRenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Standard Render Pass" )

    if ( isShadowMappingEnabled() ) {
        computeShadowMaps( renderer, renderQueue, camera );
    }

    renderOccluders( renderer, renderQueue, camera );
    renderOpaqueObjects( renderer, renderQueue, camera );
    renderTranslucentObjects( renderer, renderQueue, camera );
}

ShaderProgram *StandardRenderPass::getStandardProgram( void )
{
    if ( _standardProgram == nullptr ) {
        _standardProgram = AssetManager::getInstance()->get< ShaderProgram >( Renderer::SHADER_PROGRAM_RENDER_PASS_STANDARD );
    }
    
    assert( _standardProgram != nullptr );
    return _standardProgram;
}

void StandardRenderPass::computeShadowMaps( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Compute Shadows" )

    auto program = renderer->getShaderProgram( Renderer::SHADER_PROGRAM_DEPTH );
    if ( program == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No shader program with name '", Renderer::SHADER_PROGRAM_DEPTH, "'" );
        return;
    }
    
    // bind shader program first
    renderer->bindProgram( program );
    
    renderer->setAlphaState( AlphaState::DISABLED );
    renderer->setDepthState( DepthState::ENABLED );
    
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
        
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LINEAR_DEPTH_CONSTANT_UNIFORM ), map->getLinearDepthConstant() );
        
        auto renderables = renderQueue->getRenderables( RenderQueue::RenderableType::SHADOW_CASTER );
        
        renderQueue->each( renderables, [this, renderer, program]( RenderQueue::Renderable *renderable ) {
            renderStandardGeometry(
                renderer,
                crimild::get_ptr( renderable->geometry ),
                program,
                nullptr, // no material
                renderable->modelTransform );
        });

        renderer->unbindFrameBuffer( map->getBuffer() );
    });
    
    // unbind the shader program
    renderer->unbindProgram( program );
}

void StandardRenderPass::renderOccluders( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Render Translucent Objects" )
    
    auto renderables = renderQueue->getRenderables( RenderQueue::RenderableType::OCCLUDER );
    
    if ( renderables->size() == 0 ) {
        return;
    }
    
    renderQueue->each( renderables, [this, renderer, renderQueue]( RenderQueue::Renderable *renderable ) {
        auto material = crimild::get_ptr( renderable->material );
        auto program = material->getProgram();
        if ( program == nullptr ) {
            program = getStandardProgram();
        }
        
        renderer->bindProgram( program );
        
        auto projection = renderQueue->getProjectionMatrix();
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), projection );
        
        auto view = renderQueue->getViewMatrix();
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), view );
        
        renderStandardGeometry( renderer, crimild::get_ptr( renderable->geometry ), program, material, renderable->modelTransform );
        
        renderer->unbindProgram( program );
    });
}

void StandardRenderPass::renderOpaqueObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Render Opaque Objects" )
    
    auto renderables = renderQueue->getRenderables( RenderQueue::RenderableType::OPAQUE );
    
    if ( renderables->size() == 0 ) {
        return;
    }
    
    renderQueue->each( renderables, [this, renderer, renderQueue]( RenderQueue::Renderable *renderable ) {
        auto material = crimild::get_ptr( renderable->material );
        auto program = material->getProgram();
        if ( program == nullptr ) {
            program = getStandardProgram();
        }
        
        renderer->bindProgram( program );
        
        auto projection = renderQueue->getProjectionMatrix();
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), projection );
        
        auto view = renderQueue->getViewMatrix();
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), view );
        
        if ( isShadowMappingEnabled() ) {
            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::USE_SHADOW_MAP_UNIFORM ), _shadowMaps.size() > 0 );
            for ( auto it : _shadowMaps ) {
                if ( it.second != nullptr ) {
                    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_SOURCE_PROJECTION_MATRIX_UNIFORM ), it.second->getLightProjectionMatrix() );
                    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_SOURCE_VIEW_MATRIX_UNIFORM ), it.second->getLightViewMatrix() );
                    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LINEAR_DEPTH_CONSTANT_UNIFORM ), it.second->getLinearDepthConstant() );
                    renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SHADOW_MAP_UNIFORM ), it.second->getTexture() );
                }
            }
        }
        
        if ( isLightingEnabled() ) {
            renderQueue->each( [renderer, program]( Light *light, int ) {
                renderer->bindLight( program, light );
            });
        }
        
        renderStandardGeometry( renderer, crimild::get_ptr( renderable->geometry ), program, material, renderable->modelTransform );
        
        if ( isLightingEnabled() ) {
            renderQueue->each( [ renderer, program ]( Light *light, int ) {
                renderer->unbindLight( program, light );
            });
        }
        
        if ( isShadowMappingEnabled() ) {
            for ( auto it : _shadowMaps ) {
                if ( it.second != nullptr ) {
                    renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SHADOW_MAP_UNIFORM ), it.second->getTexture() );
                }
            }
        }
        
        renderer->unbindProgram( program );
    });
}

void StandardRenderPass::renderTranslucentObjects( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Render Translucent Objects" )
    
    auto renderables = renderQueue->getRenderables( RenderQueue::RenderableType::TRANSLUCENT );
    
    if ( renderables->size() == 0 ) {
        return;
    }
    
    renderQueue->each( renderables, [this, renderer, renderQueue]( RenderQueue::Renderable *renderable ) {
        auto material = crimild::get_ptr( renderable->material );
        auto program = material->getProgram();
        if ( program == nullptr ) {
            program = getStandardProgram();
        }
        
        renderer->bindProgram( program );
        
        auto projection = renderQueue->getProjectionMatrix();
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), projection );
        
        auto view = renderQueue->getViewMatrix();
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), view );
        
        renderStandardGeometry( renderer, crimild::get_ptr( renderable->geometry ), program, material, renderable->modelTransform );
        
        renderer->unbindProgram( program );
    });
}

void StandardRenderPass::renderStandardGeometry( Renderer *renderer, Geometry *geometry, ShaderProgram *program, Material *material, const Matrix4f &modelTransform )
{
    if ( material != nullptr ) {
        renderer->bindMaterial( program, material );
    }
    
    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), modelTransform );
    
    auto rc = geometry->getComponent< RenderStateComponent >();
    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_COUNT_UNIFORM ), 0 );
    if ( rc->getSkinnedMesh() != nullptr && rc->getSkinnedMesh()->getAnimationState() != nullptr ) {
        auto animationState = rc->getSkinnedMesh()->getAnimationState();
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_COUNT_UNIFORM ), ( int ) animationState->getJointPoses().size() );
        for ( int i = 0; i < animationState->getJointPoses().size(); i++ ) {
            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_POSE_UNIFORM + i ), animationState->getJointPoses()[ i ] );
        }
    }
    
    geometry->forEachPrimitive( [renderer, program]( Primitive *primitive ) {
        renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
        renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );
        
        renderer->drawPrimitive( program, primitive );
        
        renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
        renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );
    });
    
    if ( material != nullptr ) {
        renderer->unbindMaterial( program, material );
    }
}

