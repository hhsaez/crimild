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

#include "ShadowRenderPass.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/SkinnedMesh.hpp"
#include "Rendering/ShadowMap.hpp"
#include "Rendering/ImageEffects/ImageEffect.hpp"

#include "Components/RenderStateComponent.hpp"

#include "Foundation/Profiler.hpp"

#include "Simulation/Simulation.hpp"

using namespace crimild;

ShadowRenderPass::ShadowRenderPass( void )
{

}

ShadowRenderPass::~ShadowRenderPass( void )
{
    
}

crimild::Bool ShadowRenderPass::isShadowMappingEnabled( void ) const
{
    // TODO: shadows only work on desktop for the moment
#if defined( CRIMILD_PLATFORM_DESKTOP )
	// shadows are enabled by default
	return Simulation::getInstance()->getSettings()->get< crimild::Bool >( Settings::SETTINGS_RENDERING_SHADOWS_ENABLED, true );
#else
	return false;
#endif
}

void ShadowRenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    if ( isShadowMappingEnabled() ) {
        computeShadowMaps( renderer, renderQueue, camera );

#if 0
        auto debugShader = renderer->getShaderProgram( Renderer::SHADER_PROGRAM_DEBUG_DEPTH );

        renderQueue->each( [ this, renderer, debugShader ]( Light *light, int ) {
        	if ( light->castShadows() ) {
        		auto shadowMap = light->getShadowMap();
        		RenderPass::render( renderer, shadowMap->getTexture(), debugShader );
        	}
        });
#endif

    }
}

void ShadowRenderPass::computeShadowMaps( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
    CRIMILD_PROFILE( "Compute Shadows" )

    auto program = renderer->getShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_DIFFUSE );
    if ( program == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No shader program with name '", Renderer::SHADER_PROGRAM_UNLIT_DIFFUSE, "'" );
        return;
    }
    
    // bind shader program first
    renderer->bindProgram( program );
    
    renderer->setAlphaState( AlphaState::DISABLED );
    renderer->setDepthState( DepthState::ENABLED );
    
    renderQueue->each( [ this, renderer, program, renderQueue ]( Light *light, int ) {
        
        if ( !light->castShadows() ) {
            return;
        }

        auto map = light->getShadowMap();

		if ( light->getType() == Light::Type::DIRECTIONAL ) {
	        // TODO: near and far should be calculated automatically based
	        // on camera frustum and the scene
#if 0
            const auto NEAR = 100.0f;
            const auto FAR = 100.0f;
            
	    	const float top = 50;
			const float bottom = -50;
			const float left = -50;
			const float right = 50;
			const float far = FAR;
			const float near = -NEAR;

			const auto pMatrix = Matrix4f(
				2.0f / ( right - left ), 0.0f, 0.0f, - ( right + left ) / ( right - left ),
				0.0f, 2.0f / ( top - bottom ), 0.0f, - ( top + bottom ) / ( top - bottom ),
				0.0f, 0.0f, -2.0f / ( far - near ), - ( far + near ) / ( far - near ),
				0.0f, 0.0f, 0.0f, 1.0f
			);
#else
			Frustumf f( 45.0f, 1.0f, 0.1f, 100.0f );
			const auto pMatrix = f.computeProjectionMatrix();
#endif

        	map->setLightProjectionMatrix( pMatrix );

        	// for directional lights, we only care for the rotation
        	Transformation t = light->getWorld();
        	//t.setRotate( light->getWorld().getRotate() );
			const auto vMatrix = t.computeModelMatrix().getInverse();
        	map->setLightViewMatrix( vMatrix );
		}
		else {
	        // TODO: near and far should be calculated automatically based
	        // on camera frustum and the scene
	        const auto NEAR = 1.0f;
	        const auto FAR = 100.0f;

	        // TODO: light's frustum should not be hardcoded, right?
	        Frustumf frustum( 90.0f, 4.0f / 3.0f, NEAR, FAR );
			const auto pMatrix = frustum.computeProjectionMatrix();

        	map->setLightProjectionMatrix( pMatrix );			

			const auto vMatrix = light->getWorld().computeModelMatrix().getInverse();
	        map->setLightViewMatrix( vMatrix );
		}

        renderer->bindFrameBuffer( map->getBuffer() );
        
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), map->getLightProjectionMatrix() );
        renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), map->getLightViewMatrix() );
        
        auto renderables = renderQueue->getRenderables( RenderQueue::RenderableType::SHADOW_CASTER );
        
        renderQueue->each( renderables, [ this, renderer, program ]( RenderQueue::Renderable *renderable ) {
            renderStandardGeometry(
                renderer,
                crimild::get_ptr( renderable->geometry ),
                program,
                nullptr, // no material
                renderable->modelTransform );
        });

        renderer->unbindFrameBuffer( map->getBuffer() );
    });
    
    renderer->unbindProgram( program );
}

void ShadowRenderPass::renderStandardGeometry( Renderer *renderer, Geometry *geometry, ShaderProgram *program, Material *material, const Matrix4f &modelTransform )
{
    if ( material != nullptr ) {
        renderer->bindMaterial( program, material );
    }
    
    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ), modelTransform );
    
    renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::SKINNED_MESH_JOINT_COUNT_UNIFORM ), 0 );
    
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

		renderer->bindPrimitive( program, primitive );
        renderer->drawPrimitive( program, primitive );
		renderer->unbindPrimitive( program, primitive );
    });
    
    if ( material != nullptr ) {
        renderer->unbindMaterial( program, material );
    }
}

