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
#include "Simulation/AssetManager.hpp"
#include "Foundation/Profiler.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;

ForwardLightingPass::ForwardLightingPass( RenderGraph *graph, crimild::Size maxLights )
	: ForwardLightingPass(
		graph,
		{
			RenderQueue::RenderableType::OPAQUE,
			RenderQueue::RenderableType::TRANSLUCENT,
		},
		maxLights )
{
	
}

ForwardLightingPass::ForwardLightingPass( RenderGraph *graph, ForwardLightingPass::RenderableTypeArray const &renderableTypes, crimild::Size maxLights )
	: RenderGraphPass( graph, "Forward Lighting" ),
	  _renderableTypes( renderableTypes ),
	  _programs( maxLights + 1 )
{
    _colorOutput = graph->createAttachment(
		getName() + " - Color",
		RenderGraphAttachment::Hint::FORMAT_RGBA );
	_clearFlags = FrameBufferObject::ClearFlag::COLOR;
	_depthState = crimild::alloc< DepthState >( true, DepthState::CompareFunc::LEQUAL, false );

	for ( crimild::Size i = 0; i <= maxLights; i++ ) {
		_programs[ i ] = crimild::alloc< ForwardShadingShaderProgram >( i );
	}
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
	
	graph->read( this, { _depthInput } );
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

	auto lightCount = Numerici::min( _programs.size() - 1, renderQueue->getLightCount() );
	auto program = crimild::get_ptr( _programs[ lightCount ] );

	program->bindProjMatrix( pMatrix );
	program->bindViewMatrix( vMatrix );

	renderQueue->each( [ program ]( Light *light, int index ) {
		program->bindLight( light, index );
	});

	renderQueue->each( renderables, [ this, renderer, program ]( RenderQueue::Renderable *renderable ) {
		const auto &mMatrix = renderable->modelTransform;
		program->bindModelMatrix( mMatrix );

		const auto nMatrix = Matrix3f( mMatrix ).getInverse().getTranspose();
		program->bindNormalMatrix( mMatrix );

		if ( auto material = crimild::get_ptr( renderable->material ) ) {
			program->bindMaterial( material );

			renderer->setAlphaState( material->getAlphaState() );
			renderer->setDepthState( material->getDepthState() );
		}

		renderer->bindProgram( program );
		
		renderable->geometry->forEachPrimitive( [ renderer ]( Primitive *primitive ) {
			renderer->bindPrimitive( nullptr, primitive );
			renderer->drawPrimitive( nullptr, primitive );
			renderer->unbindPrimitive( nullptr, primitive );
		});

		renderer->setAlphaState( AlphaState::DISABLED );
		renderer->setDepthState( DepthState::ENABLED );

		renderer->unbindProgram( program );
	});
}

