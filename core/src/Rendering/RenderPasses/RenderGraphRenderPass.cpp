/*
 * Copyright (c) 2013-present, H. Hernan Saez
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

#include "RenderGraphRenderPass.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/RenderGraph/RenderGraph.hpp"
#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;
using namespace rendergraph;

RenderGraphRenderPass::RenderGraphRenderPass( SharedPointer< rendergraph::RenderGraph > const &renderGraph )
	: _renderGraph( renderGraph )
{
	_program = crimild::retain( AssetManager::getInstance()->get< ShaderProgram >( Renderer::SHADER_PROGRAM_SCREEN_TEXTURE ) );
}
	
RenderGraphRenderPass::~RenderGraphRenderPass( void )
{
	
}

void RenderGraphRenderPass::render( Renderer *renderer, RenderQueue *renderQueue, Camera *camera )
{
	_renderGraph->execute( renderer, renderQueue );
	
	auto output = _renderGraph->getOutput();
	if ( output == nullptr ) {
		CRIMILD_LOG_ERROR( "No output provided for render graph" );
		return;
	}
	
	auto texture = output->getTexture();
	if ( texture == nullptr ) {
		CRIMILD_LOG_ERROR( "No valid texture for render graph output" );
		return;
	}
	
	auto program = crimild::get_ptr( _program );
	assert( program && "No valid program to render texture" );
	
	renderer->bindProgram( program );
	
	renderer->bindTexture(
		program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ),
		texture );
	
	renderer->drawScreenPrimitive( program );
	
	renderer->unbindTexture(
		program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ),
		texture );
	
	renderer->unbindProgram( program );
}

