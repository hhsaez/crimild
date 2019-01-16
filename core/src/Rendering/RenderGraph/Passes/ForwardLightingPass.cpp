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
	CRIMILD_PROFILE( "Forward Lighting Pass" )
	
	auto fbo = graph->createFBO( { _depthInput, _colorOutput } );
	fbo->setClearFlags( _clearFlags );
	
	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );

	_renderableTypes.each( [ this, renderer, renderQueue ]( RenderQueue::RenderableType const &type ) {
		render( renderer, renderQueue, type );
	});
	
	renderer->unbindFrameBuffer( crimild::get_ptr( fbo ) );
}

void ForwardLightingPass::render( Renderer *renderer, RenderQueue *renderQueue, RenderQueue::RenderableType renderableType )
{
	auto renderables = renderQueue->getRenderables( renderableType );
	if ( renderables->size() == 0 ) {
		return;
	}

	const auto pMatrix = renderQueue->getProjectionMatrix();
	const auto vMatrix = renderQueue->getViewMatrix();

	auto skybox = crimild::get_ptr( Texture::CUBE_ONE );
	renderQueue->each(
		renderQueue->getRenderables( RenderQueue::RenderableType::SKYBOX ),
		[ &skybox ]( RenderQueue::Renderable *renderable ) {
			if ( auto material = renderable->material ) {
				skybox = material->getColorMap();
			}
		}
	);

	auto shadowAtlas = crimild::get_ptr( Texture::ONE );
	if ( auto input = getShadowInput() ) {
		if ( auto texture = input->getTexture() ) {
			shadowAtlas = texture;
		}
	}

	renderer->setDepthState( _depthState );

	renderQueue->each( renderables, [ this, renderer, renderQueue, pMatrix, vMatrix, skybox, shadowAtlas ]( RenderQueue::Renderable *renderable ) {
		auto program = crimild::get_ptr( _program );

		auto color = RGBAColorf::ONE;
		auto colorMap = crimild::get_ptr( Texture::ONE );
		auto specular = RGBAColorf::ONE;
		auto specularMap = crimild::get_ptr( Texture::ONE );
		auto shininess = 1.0f;
		auto alphaState = crimild::get_ptr( AlphaState::DISABLED );
		auto reflection = 0.0f;
		auto reflectionMap = crimild::get_ptr( Texture::ONE );
		auto refraction = 1.0f;
		auto refractionMap = crimild::get_ptr( Texture::ONE );
		auto environmentMap = skybox;

		if ( auto material = renderable->material ) {
			if ( material->getProgram() ) program = material->getProgram();

			color = material->getDiffuse();
			if ( material->getColorMap() ) colorMap = material->getColorMap();

			specular = material->getSpecular();
			if ( material->getSpecularMap() ) specularMap = material->getSpecularMap();

			shininess = material->getShininess();

			reflection = material->getReflection();
			if ( material->getReflectionMap() ) reflectionMap = material->getReflectionMap();

			refraction = material->getRefraction();
			if ( material->getRefractionMap() ) reflectionMap = material->getRefractionMap();

			if ( material->getAlphaState() ) alphaState = material->getAlphaState();
		}

		program->bindUniform( PROJECTION_MATRIX_UNIFORM, pMatrix );
		program->bindUniform( VIEW_MATRIX_UNIFORM, vMatrix );
		const auto &mMatrix = renderable->modelTransform;
		program->bindUniform( MODEL_MATRIX_UNIFORM, mMatrix );
		const auto nMatrix = Matrix3f( mMatrix ).getInverse().getTranspose();
		program->bindUniform( NORMAL_MATRIX_UNIFORM, nMatrix );

		program->bindUniform( LIGHT_ARRAY_COUNT_UNIFORM, ( crimild::Int32 ) renderQueue->getLightCount() );
		renderQueue->each( [ program ]( Light *light, int index ) {
			if ( light == nullptr ) {
				return;
			}

			std::stringstream ss;
			ss << LIGHT_UNIFORM << "_" << index;
			program->bindUniform( ss.str(), light );
		});

		program->bindUniform( COLOR_UNIFORM, color );
		program->bindUniform( COLOR_MAP_UNIFORM, colorMap );
		program->bindUniform( SPECULAR_UNIFORM, specular );
		program->bindUniform( SPECULAR_MAP_UNIFORM, specularMap );
		program->bindUniform( SHININESS_UNIFORM, shininess );
		program->bindUniform( REFLECTION_UNIFORM, reflection );
		program->bindUniform( REFLECTION_MAP_UNIFORM, reflectionMap );
		program->bindUniform( REFRACTION_UNIFORM, refraction );
		program->bindUniform( REFRACTION_MAP_UNIFORM, refractionMap );
		program->bindUniform( ENVIRONMENT_MAP_UNIFORM, environmentMap );
		program->bindUniform( SHADOW_ATLAS_UNIFORM, shadowAtlas );

		renderer->bindProgram( program );
		renderer->setAlphaState( alphaState );

		renderable->geometry->forEachPrimitive( [ renderer ]( Primitive *primitive ) {
			renderer->bindPrimitive( nullptr, primitive );
			renderer->drawPrimitive( nullptr, primitive );
			renderer->unbindPrimitive( nullptr, primitive );
		});

		renderer->setAlphaState( AlphaState::DISABLED );
		renderer->unbindProgram( program );
	});

	renderer->setDepthState( DepthState::ENABLED );
}

