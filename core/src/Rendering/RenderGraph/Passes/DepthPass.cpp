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

#include "DepthPass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Material.hpp"
#include "Foundation/Profiler.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexShaderInputs.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;
using namespace crimild::shadergraph;

class DepthPassProgram : public ShaderProgram {
public:
	DepthPassProgram( void )
	{
		createVertexShader();
		createFragmentShader();
		
		registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
		registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::NORMAL_ATTRIBUTE, "aNormal" );
		
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM, "uPMatrix" );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM, "uVMatrix" );
		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM, "uMMatrix" );
	}

	virtual ~DepthPassProgram( void )
	{

	}

private:
	void createVertexShader( void )
	{
		auto graph = Renderer::getInstance()->createShaderGraph();

		auto P = csl::projectedPosition();
		auto N = csl::viewNormal();
		csl::vertexOutput( "vViewNormal", N );
		csl::vertexPosition( P );

		auto src = graph->build();
		auto shader = crimild::alloc< VertexShader >( src );
		setVertexShader( shader );
	}

	void createFragmentShader( void )
	{
		auto graph = Renderer::getInstance()->createShaderGraph();

		auto N = csl::vec3_in( "vViewNormal" );
		csl::fragColor( csl::vec4( N, csl::scalar( 1.0f ) ) );

		auto src = graph->build();
		auto shader = crimild::alloc< FragmentShader >( src );
		setFragmentShader( shader );
	}
};

DepthPass::DepthPass( RenderGraph *graph )
	: RenderGraphPass( graph, "Depth Pass" )
{
	_depthOutput = graph->createAttachment( getName() + " - Depth", RenderGraphAttachment::Hint::FORMAT_DEPTH_HDR );
	_normalOutput = graph->createAttachment( getName() + " - Normal", RenderGraphAttachment::Hint::FORMAT_RGBA_HDR );
}
			
DepthPass::~DepthPass( void )
{
	
}
			
void DepthPass::setup( RenderGraph *graph )
{
	graph->write( this, { _depthOutput, _normalOutput } );

	_program = crimild::alloc< DepthPassProgram >();
}

void DepthPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	CRIMILD_PROFILE( getName() )
	
	auto fbo = graph->createFBO( { _depthOutput, _normalOutput } );
	
	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );
	
	renderer->setColorMaskState( ColorMaskState::DISABLED );
	renderObjects( renderer, renderQueue, RenderQueue::RenderableType::OCCLUDER );
	renderer->setColorMaskState( ColorMaskState::ENABLED );
	
	renderObjects( renderer, renderQueue, RenderQueue::RenderableType::OPAQUE );
	renderObjects( renderer, renderQueue, RenderQueue::RenderableType::OPAQUE_CUSTOM );
	
	renderer->unbindFrameBuffer( crimild::get_ptr( fbo ) );	
}

void DepthPass::renderObjects( Renderer *renderer, RenderQueue *renderQueue, RenderQueue::RenderableType renderableType )
{
	auto renderables = renderQueue->getRenderables( renderableType );
	if ( renderables->size() == 0 ) {
		return;
	}
	
	auto program = crimild::get_ptr( _program );
	auto pMatrix = renderQueue->getProjectionMatrix();
	auto vMatrix = renderQueue->getViewMatrix();
	
	renderQueue->each( renderables, [this, renderer, renderQueue, program, &pMatrix, &vMatrix ]( RenderQueue::Renderable *renderable ) {
		auto material = crimild::get_ptr( renderable->material );
		
		renderer->bindProgram( program );
		
		renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ), pMatrix );
		renderer->bindUniform( program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ), vMatrix );
		
		renderer->drawGeometry(
			crimild::get_ptr( renderable->geometry ),
			program,
			renderable->modelTransform );
		
		renderer->unbindProgram( program );
	});
}

