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
#include "Animation/Skeleton.hpp"
#include "Rendering/ImageEffects/ImageEffect.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Components/RenderStateComponent.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/Profiler.hpp"

#include "Simulation/AssetManager.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::animation;

StandardRenderPass::StandardRenderPass( void )
{

}

StandardRenderPass::~StandardRenderPass( void )
{
    
}

void StandardRenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Standard Render Pass" )

    _shadowMapping = false;
#if defined( CRIMILD_PLATFORM_DESKTOP )
    _shadowMapping = Simulation::getInstance()->getSettings()->get< crimild::Bool >( Settings::SETTINGS_RENDERING_SHADOWS_ENABLED, true );
#endif


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

}

void StandardRenderPass::renderOccluders( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Render Occluder Objects" )
    
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
        
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::USE_SHADOW_MAP_UNIFORM ), false );
        if ( isShadowMappingEnabled() ) {
            renderQueue->each( [ this, renderer, program, renderQueue ]( Light *light, int ) {        
                if ( !light->castShadows() ) {
                    return;
                }

                auto map = light->getShadowMap();
                if ( map == nullptr ) {
                    return;
                }

                if ( map->getTexture() == nullptr || map->getTexture()->getCatalog() == nullptr ) {
                    return;
                }

                renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::USE_SHADOW_MAP_UNIFORM ), true );
                renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_SOURCE_PROJECTION_MATRIX_UNIFORM ), map->getLightProjectionMatrix() );
                renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::LIGHT_SOURCE_VIEW_MATRIX_UNIFORM ), map->getLightViewMatrix() );
                renderer->bindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SHADOW_MAP_UNIFORM ), map->getTexture() );
            });
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
            renderQueue->each( [ this, renderer, program, renderQueue ]( Light *light, int ) {        
                if ( !light->castShadows() ) {
                    return;
                }

                auto map = light->getShadowMap();
                if ( map == nullptr ) {
                    return;
                }

                if ( map->getTexture() == nullptr || map->getTexture()->getCatalog() == nullptr ) {
                    return;
                }

                renderer->unbindTexture( program->getStandardLocation( ShaderProgram::StandardLocation::SHADOW_MAP_UNIFORM ), map->getTexture() );
            });
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
	if ( auto skeleton = rc->getSkeleton() ) {
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_COUNT_UNIFORM ), ( int ) skeleton->getJoints().size() );
		skeleton->getJoints().each( [ renderer, program ]( const std::string &, SharedPointer< Joint > const &joint ) {
			renderer->bindUniform(
				program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_POSE_UNIFORM + joint->getId() ),
				joint->getPoseMatrix()
			);
		});
	}
	/*
    if ( rc->getSkinnedMesh() != nullptr && rc->getSkinnedMesh()->getAnimationState() != nullptr ) {
        auto animationState = rc->getSkinnedMesh()->getAnimationState();
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_COUNT_UNIFORM ), ( int ) animationState->getJointPoses().size() );
        for ( int i = 0; i < animationState->getJointPoses().size(); i++ ) {
            renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_POSE_UNIFORM + i ), animationState->getJointPoses()[ i ] );
        }
    }
	*/
    
    geometry->forEachPrimitive( [renderer, program]( Primitive *primitive ) {
		// TODO: maybe we shound't add a geometry to the queue if it
		// has no valid primitive instead of quering the state of the
		// VBO and IBO while rendering
		
		auto vbo = primitive->getVertexBuffer();
		if ( vbo == nullptr ) {
			return;
		}

		auto ibo = primitive->getIndexBuffer();
		if ( ibo == nullptr ) {
			return;
		}
		
        renderer->bindVertexBuffer( program, vbo );
        renderer->bindIndexBuffer( program, ibo );
        
        renderer->drawPrimitive( program, primitive );
        
        renderer->unbindVertexBuffer( program, vbo );
        renderer->unbindIndexBuffer( program, ibo );
    });
    
    if ( material != nullptr ) {
        renderer->unbindMaterial( program, material );
    }
}

