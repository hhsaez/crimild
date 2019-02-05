/*
 * Copyright (c) 2002-present, H. Hernán Saez
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

#include "ForwardLightingPass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Programs/ForwardShadingShaderProgram.hpp"
#include "Rendering/Programs/UnlitShaderProgram.hpp"
#include "Simulation/AssetManager.hpp"
#include "Foundation/Profiler.hpp"
#include "Rendering/ShaderGraph/Constants.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;
using namespace crimild::shadergraph;
using namespace crimild::shadergraph::locations;

ForwardLightingPass::ForwardLightingPass( RenderGraph *graph, crimild::Size maxLights )
	: ForwardLightingPass(
		graph,
		{
            RenderQueue::RenderableType::OPAQUE,
			RenderQueue::RenderableType::OPAQUE_CUSTOM,
            RenderQueue::RenderableType::TRANSLUCENT,
            RenderQueue::RenderableType::TRANSLUCENT_CUSTOM,
		},
		maxLights )
{
	
}

ForwardLightingPass::ForwardLightingPass( RenderGraph *graph, ForwardLightingPass::RenderableTypeArray const &renderableTypes, crimild::Size maxLights )
	: RenderGraphPass( graph, "Forward Lighting" ),
	  _renderableTypes( renderableTypes )
{
    _colorOutput = graph->createAttachment(
		getName() + " - Color",
		RenderGraphAttachment::Hint::FORMAT_RGBA );
	_clearFlags = FrameBufferObject::ClearFlag::COLOR;
	_depthState = crimild::alloc< DepthState >( true, DepthState::CompareFunc::LEQUAL, false );

	_program = crimild::alloc< ForwardShadingShaderProgram >(); 
}

ForwardLightingPass::~ForwardLightingPass( void )
{
	
}

void ForwardLightingPass::setup( rendergraph::RenderGraph *graph )
{
	if ( _depthInput == nullptr ) {
		_depthInput = graph->createAttachment(
			getName() + " - Depth",
			RenderGraphAttachment::Hint::FORMAT_DEPTH |
			RenderGraphAttachment::Hint::RENDER_ONLY );
		_clearFlags = FrameBufferObject::ClearFlag::ALL;
		_depthState = DepthState::ENABLED;
	}
	
	graph->read( this, { _depthInput, _shadowInput } );
	graph->write( this, { _colorOutput } );
}

void ForwardLightingPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	CRIMILD_PROFILE( getName() )

	auto fbo = graph->createFBO( { _depthInput, _colorOutput } );
	fbo->setClearFlags( _clearFlags );

	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );

	_renderableTypes.each( [ this, renderer, renderQueue ]( RenderQueue::RenderableType const &renderableType ) {
        render( renderer, renderQueue, renderableType );
	});

	renderer->unbindFrameBuffer( crimild::get_ptr( fbo ) );
}

void ForwardLightingPass::render( Renderer *renderer, RenderQueue *renderQueue, RenderQueue::RenderableType renderableType )
{
    auto renderables = renderQueue->getRenderables( renderableType );
    if ( renderables->size() == 0 ) {
        return;
    }

    containers::Map< ShaderProgram *, containers::List< RenderQueue::Renderable * >> sorted;

    renderQueue->each( renderables, [ this, &sorted ]( RenderQueue::Renderable *renderable ) {
        auto program = crimild::get_ptr( _program );
        if ( auto material = renderable->material ) {
            if ( material->getProgram() != nullptr ) program = material->getProgram();
        }

        sorted[ program ].add( renderable );
    });

    sorted.each( [ this, renderer, renderQueue ]( ShaderProgram *program, containers::List< RenderQueue::Renderable * > &renderables ) {
        render( renderer, renderQueue, program, renderables );
    });
}

void ForwardLightingPass::render( Renderer *renderer, RenderQueue *renderQueue, ShaderProgram *program, containers::List< RenderQueue::Renderable * > &renderables )
{
	renderer->setDepthState( _depthState );
	renderer->setAlphaState( AlphaState::DISABLED );
	renderer->setCullFaceState( CullFaceState::ENABLED_BACK );

	const auto pMatrix = renderQueue->getProjectionMatrix();
	program->bindUniform( PROJECTION_MATRIX_UNIFORM, pMatrix );

	const auto vMatrix = renderQueue->getViewMatrix();
	program->bindUniform( VIEW_MATRIX_UNIFORM, vMatrix );

	program->bindUniform( MODEL_MATRIX_UNIFORM, Matrix4f::IDENTITY );
	program->bindUniform( NORMAL_MATRIX_UNIFORM, Matrix3f::IDENTITY );
	
	program->bindUniform( COLOR_UNIFORM, RGBAColorf::ONE );
	program->bindUniform( COLOR_MAP_UNIFORM, crimild::get_ptr( Texture::ONE ) );
	program->bindUniform( SPECULAR_UNIFORM, RGBAColorf::ONE );
	program->bindUniform( SPECULAR_MAP_UNIFORM, crimild::get_ptr( Texture::ONE ) );
	program->bindUniform( SHININESS_UNIFORM, 1.0f );
	program->bindUniform( REFLECTION_UNIFORM, 0.0f );
	program->bindUniform( REFLECTION_MAP_UNIFORM, crimild::get_ptr( Texture::ONE ) );
	program->bindUniform( REFRACTION_UNIFORM, 1.0f );
	program->bindUniform( REFRACTION_MAP_UNIFORM, crimild::get_ptr( Texture::ONE ) );

	auto skybox = crimild::get_ptr( Texture::CUBE_ONE );
	renderQueue->each(
		renderQueue->getRenderables( RenderQueue::RenderableType::SKYBOX ),
		[ &skybox ]( RenderQueue::Renderable *renderable ) {
			if ( auto material = renderable->material ) {
				skybox = material->getColorMap();
			}
		}
	);
	program->bindUniform( ENVIRONMENT_MAP_UNIFORM, skybox );

	auto shadowAtlas = crimild::get_ptr( Texture::ONE );
	if ( auto input = getShadowInput() ) {
		if ( auto texture = input->getTexture() ) {
			shadowAtlas = texture;
		}
	}
	program->bindUniform( SHADOW_ATLAS_UNIFORM, shadowAtlas );

	program->bindUniform( LIGHT_ARRAY_COUNT_UNIFORM, ( crimild::Int32 ) renderQueue->getLightCount() );
	renderQueue->each( [ program ]( Light *light, int index ) {
		if ( light == nullptr ) {
			return;
		}
		
		std::stringstream ss;
		ss << LIGHT_UNIFORM << "_" << index;
		program->bindUniform( ss.str(), light );
	});

	renderer->bindProgram( program );

    renderables.each( [ renderer, program ]( RenderQueue::Renderable *renderable ) {

		const auto &mMatrix = renderable->modelTransform;
		renderer->bindUniform( program->getLocation( MODEL_MATRIX_UNIFORM ), mMatrix );
		const auto nMatrix = Matrix3f( mMatrix ).getInverse().getTranspose();
        renderer->bindUniform( program->getLocation( NORMAL_MATRIX_UNIFORM ), nMatrix );

		if ( auto material = renderable->material ) {
			renderer->bindUniform( program->getLocation( COLOR_UNIFORM ), material->getDiffuse() );
			if ( material->getColorMap() ) {
				renderer->bindTexture( program->getLocation( COLOR_MAP_UNIFORM ), material->getColorMap() );
			}

			renderer->bindUniform( program->getLocation( SPECULAR_UNIFORM ), material->getSpecular() );
			if ( material->getSpecularMap() ) {
				renderer->bindTexture( program->getLocation( SPECULAR_MAP_UNIFORM ), material->getSpecularMap() );
			}

			renderer->bindUniform( program->getLocation( SHININESS_UNIFORM ), material->getShininess() );

			renderer->bindUniform( program->getLocation( REFLECTION_UNIFORM ), material->getReflection() );
			if ( material->getReflectionMap() ) {
				renderer->bindTexture( program->getLocation( REFLECTION_MAP_UNIFORM ), material->getReflectionMap() );
			}
			
			renderer->bindUniform( program->getLocation( REFRACTION_UNIFORM ), material->getRefraction() );
			if ( material->getRefractionMap() ) {
				renderer->bindTexture( program->getLocation( REFRACTION_MAP_UNIFORM ), material->getRefractionMap() );
			}

			if ( material->getAlphaState() ) renderer->setAlphaState( material->getAlphaState() );
			if ( material->getCullFaceState() ) renderer->setCullFaceState( material->getCullFaceState() );
		}
		
		renderable->geometry->forEachPrimitive( [ renderer ]( Primitive *primitive ) {
			renderer->bindPrimitive( nullptr, primitive );
			renderer->drawPrimitive( nullptr, primitive );
			renderer->unbindPrimitive( nullptr, primitive );
		});
		
		if ( auto material = renderable->material ) {
			if ( material->getColorMap() ) {
				renderer->unbindTexture( program->getLocation( COLOR_MAP_UNIFORM ), material->getColorMap() );
			}
			if ( material->getSpecularMap() ) {
				renderer->unbindTexture( program->getLocation( SPECULAR_MAP_UNIFORM ), material->getSpecularMap() );
			}
			if ( material->getReflectionMap() ) {
				renderer->unbindTexture( program->getLocation( REFLECTION_MAP_UNIFORM ), material->getReflectionMap() );
			}
			if ( material->getRefractionMap() ) {
				renderer->unbindTexture( program->getLocation( REFRACTION_MAP_UNIFORM ), material->getRefractionMap() );
			}

			if ( material->getAlphaState() ) renderer->setAlphaState( AlphaState::DISABLED );
			if ( material->getCullFaceState() ) renderer->setCullFaceState( CullFaceState::ENABLED_BACK );
		}

	});

	renderer->unbindProgram( program );

	renderer->setDepthState( DepthState::ENABLED );
	renderer->setAlphaState( AlphaState::DISABLED );
	renderer->setCullFaceState( CullFaceState::ENABLED_BACK );
}

