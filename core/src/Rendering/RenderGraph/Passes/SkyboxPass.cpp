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

#include "SkyboxPass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/ShaderGraph/Constants.hpp"
#include "Rendering/Programs/SkyboxShaderProgram.hpp"
#include "Simulation/AssetManager.hpp"
#include "Foundation/Profiler.hpp"

#include "Rendering/Texture.hpp"
#include "Rendering/Image.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;
using namespace crimild::shadergraph::locations;

SkyboxPass::SkyboxPass( RenderGraph *graph )
	: RenderGraphPass( graph, "Skybox" )
{
    _colorOutput = graph->createAttachment(
		getName() + " - Color",
		RenderGraphAttachment::Hint::FORMAT_RGBA );
	
	_clearFlags = FrameBufferObject::ClearFlag::COLOR;

	_depthState = crimild::alloc< DepthState >( true, DepthState::CompareFunc::LEQUAL, false );

	_program = crimild::alloc< SkyboxShaderProgram >();
}

SkyboxPass::~SkyboxPass( void )
{
	
}

void SkyboxPass::setup( rendergraph::RenderGraph *graph )
{
	if ( _depthInput == nullptr ) {
		_depthInput = graph->createAttachment(
			getName() + " - Depth",
			RenderGraphAttachment::Hint::FORMAT_DEPTH |
			RenderGraphAttachment::Hint::RENDER_ONLY );
		_clearFlags = FrameBufferObject::ClearFlag::ALL;
	}
	
	graph->read( this, { _depthInput } );
	graph->write( this, { _colorOutput } );
}

void SkyboxPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	CRIMILD_PROFILE( getName() )

	auto fbo = graph->createFBO( { _depthInput, _colorOutput } );
	fbo->setClearFlags( _clearFlags );
	
	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );

	auto renderables = renderQueue->getRenderables( RenderQueue::RenderableType::SKYBOX );
	if ( renderables->size() == 0 ) {
		return;
	}
	
	const auto pMatrix = renderQueue->getProjectionMatrix();
	const auto vMatrix = renderQueue->getViewMatrix();

	renderer->setDepthState( _depthState );

	renderQueue->each( renderables, [ this, renderer, pMatrix, vMatrix ]( RenderQueue::Renderable *renderable ) {
		auto color = RGBAColorf::ONE;
		auto colorMap = crimild::get_ptr( Texture::CUBE_ONE );
		auto program = crimild::get_ptr( _program );

		if ( auto material = crimild::get_ptr( renderable->material ) ) {
			color = material->getDiffuse();
			
			if ( material->getProgram() ) program = material->getProgram();
			if ( material->getColorMap() ) colorMap = material->getColorMap();
		}

		program->bindUniform( PROJECTION_MATRIX_UNIFORM, pMatrix );
		program->bindUniform( VIEW_MATRIX_UNIFORM, vMatrix );

		program->bindUniform( COLOR_UNIFORM, color );
		program->bindUniform( COLOR_MAP_UNIFORM, colorMap );

		renderer->bindProgram( program );
		
		renderable->geometry->forEachPrimitive( [ renderer ]( Primitive *primitive ) {
			renderer->bindPrimitive( nullptr, primitive );
			renderer->drawPrimitive( nullptr, primitive );
			renderer->unbindPrimitive( nullptr, primitive );
		});

		renderer->unbindProgram( program );
	});

	renderer->setDepthState( DepthState::ENABLED );
	
	renderer->unbindFrameBuffer( crimild::get_ptr( fbo ) );
}

