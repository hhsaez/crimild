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

#include "LinearizeDepthPass.hpp"

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
#include "Rendering/ShaderUniformImpl.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;
using namespace crimild::shadergraph;

class LinearizeDepthPassProgram : public ShaderProgram {
public:
	LinearizeDepthPassProgram( void )
		: _uNear( crimild::alloc< FloatUniform >( "uNear", 0.1f ) ),
		  _uFar( crimild::alloc< FloatUniform >( "uFar", 1024.0f ) )
	{
		createVertexShader();
		createFragmentShader();
		
		registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
		registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE, "aTextureCoord" );		

		registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM, "uDepthMap" );

		attachUniform( _uNear );
		attachUniform( _uFar );
	}

	virtual ~LinearizeDepthPassProgram( void )
	{

	}

private:
	void createVertexShader( void )
	{
		auto graph = Renderer::getInstance()->createShaderGraph();

		auto p = csl::screenPosition();
		auto uv = csl::modelTextureCoords();

		csl::vertexOutput( "vTextureCoord", uv );
		csl::vertexPosition( p );

		auto src = graph->build();
		auto shader = crimild::alloc< VertexShader >( src );
		setVertexShader( shader );
	}

	void createFragmentShader( void )
	{
		auto graph = Renderer::getInstance()->createShaderGraph();

		auto uv = csl::vec2_in( "vTextureCoord" );
		auto texture = csl::texture2D_uniform( "uDepthMap" );
		auto near = csl::scalar_uniform( _uNear->getName() );
		auto far = csl::scalar_uniform( _uFar->getName() );

		auto d = csl::red( csl::textureColor( texture, uv ) );
		d = csl::linearizeDepth( d, near, far );
		d = csl::div( d, far );

		csl::fragColor( csl::vec4( d, d, d, csl::scalar_constant( 1 ) ) );

		auto src = graph->build();
		auto shader = crimild::alloc< FragmentShader >( src );
		setFragmentShader( shader );
	}

private:
	SharedPointer< FloatUniform > _uNear;
	SharedPointer< FloatUniform > _uFar;
};

LinearizeDepthPass::LinearizeDepthPass( RenderGraph *graph )
	: RenderGraphPass( graph, "Convert Depth to linear values" )
{

}
			
LinearizeDepthPass::~LinearizeDepthPass( void )
{
	
}
			
void LinearizeDepthPass::setup( RenderGraph *graph )
{
	graph->read( this, { _input } );
	graph->write( this, { _output } );

	_program = crimild::alloc< LinearizeDepthPassProgram >();
}

void LinearizeDepthPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	CRIMILD_PROFILE( getName() );

	if ( _input == nullptr || _input->getTexture() == nullptr ) {
		return;
	}

    auto fbo = graph->createFBO( { _output } );
	
	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );
	
	auto program = crimild::get_ptr( _program );
	assert( program && "No valid program to render texture" );
	
	renderer->bindProgram( program );

	renderer->bindTexture(
		program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ),
		getInput()->getTexture() );
	
	renderer->drawScreenPrimitive( program );
	
	renderer->unbindTexture(
		program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ),
		getInput()->getTexture() );
	
	renderer->unbindProgram( program );
	
	renderer->unbindFrameBuffer( crimild::get_ptr( fbo ) );
}

