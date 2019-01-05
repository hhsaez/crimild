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

#include "OpaquePass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/DepthState.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Programs/UnlitShaderProgram.hpp"
#include "Foundation/Profiler.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;

OpaquePass::OpaquePass( RenderGraph *graph, std::string name )
	: RenderGraphPass( graph, name )
{
	_colorOutput = graph->createAttachment( getName() + " - Color Output", RenderGraphAttachment::Hint::FORMAT_RGBA );
}
			
OpaquePass::~OpaquePass( void )
{
	
}
			
void OpaquePass::setup( RenderGraph *graph )
{
	_clearFlags = FrameBufferObject::ClearFlag::COLOR;
	_depthState = crimild::alloc< DepthState >( true, DepthState::CompareFunc::LEQUAL, false );

	if ( _depthInput == nullptr ) {
		_depthInput = graph->createAttachment(
			"Aux Depth Buffer",
			RenderGraphAttachment::Hint::FORMAT_DEPTH |
			RenderGraphAttachment::Hint::RENDER_ONLY );
		_clearFlags = FrameBufferObject::ClearFlag::ALL;
		_depthState = DepthState::ENABLED;
	}
	
	graph->read( this, { _depthInput } );
	graph->write( this, { _colorOutput } );
	
	_program = crimild::alloc< UnlitShaderProgram >();
}

void OpaquePass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	auto fbo = graph->createFBO( { _depthInput, _colorOutput } );
	fbo->setClearFlags( _clearFlags );
	
	CRIMILD_PROFILE( "Render Opaque Objects" )
				
	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );

	auto renderables = renderQueue->getRenderables( RenderQueue::RenderableType::OPAQUE );
	if ( renderables->size() == 0 ) {
		return;
	}

	auto program = crimild::get_ptr( _program );

	renderer->bindProgram( program );
		
	auto projection = renderQueue->getProjectionMatrix();
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), projection );
		
	auto view = renderQueue->getViewMatrix();
	renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), view );
		
	renderQueue->each( renderables, [ this, renderer, renderQueue, &program ]( RenderQueue::Renderable *renderable ) {
		auto material = crimild::get_ptr( renderable->material );
		
		if ( material != nullptr ) {
			renderer->bindMaterial( program, material );
		}
		
		renderer->setDepthState( _depthState );
		
		renderer->drawGeometry(
			crimild::get_ptr( renderable->geometry ),
			program,
			renderable->modelTransform
		);
		
		if ( material != nullptr ) {
			renderer->unbindMaterial( program, material );
		}
		
		renderer->setDepthState( DepthState::ENABLED );
	});
	
	renderer->unbindProgram( program );
	
	renderer->unbindFrameBuffer( crimild::get_ptr( fbo ) );
}

