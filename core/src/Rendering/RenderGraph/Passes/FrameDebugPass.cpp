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

#include "FrameDebugPass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderGraph/Constants.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/Programs/ScreenTextureShaderProgram.hpp"
#include "Foundation/Profiler.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;
using namespace crimild::shadergraph::locations;

FrameDebugPass::FrameDebugPass( RenderGraph *graph, std::string name )
	: RenderGraphPass( graph, name ),
	  _program( crimild::alloc< ScreenTextureShaderProgram >() )
{
	_output = graph->createAttachment( getName() + " - Output", RenderGraphAttachment::Hint::FORMAT_RGBA );
}
			
FrameDebugPass::~FrameDebugPass( void )
{
	
}
			
void FrameDebugPass::setup( RenderGraph *graph )
{
	graph->read( this, _inputs );
	graph->write( this, { _output } );
}

void FrameDebugPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	const static auto LAYOUT_4 = Array< Rectf > {
		Rectf( 0.0f, 0.5f, 0.5f, 0.5f ),
		Rectf( 0.5f, 0.5f, 0.5f, 0.5f ),
		Rectf( 0.0f, 0.0f, 0.5f, 0.5f ),
		Rectf( 0.5f, 0.0f, 0.5f, 0.5f ),
	};
	
	const static auto LAYOUT_8 = Array< Rectf > {
		Rectf( 0.25f, 0.0f, 0.75f, 0.75f ),
		
		Rectf( 0.005f, 0.755f, 0.24f, 0.24f ),
		Rectf( 0.255f, 0.755f, 0.24f, 0.24f ),
		Rectf( 0.505f, 0.755f, 0.24f, 0.24f ),
		Rectf( 0.755f, 0.755f, 0.24f, 0.24f ),
		
		Rectf( 0.005f, 0.505f, 0.24f, 0.24f ),
		Rectf( 0.005f, 0.255f, 0.24f, 0.24f ),
		Rectf( 0.005f, 0.005f, 0.24f, 0.24f ),
	};
	
	const static auto LAYOUT_12 = Array< Rectf > {
		Rectf( 0.25f, 0.25f, 0.5f, 0.5f ),
		
		Rectf( 0.0f, 0.75f, 0.25f, 0.25f ),
		Rectf( 0.25f, 0.75f, 0.25f, 0.25f ),
		Rectf( 0.5f, 0.75f, 0.25f, 0.25f ),
		Rectf( 0.75f, 0.75f, 0.25f, 0.25f ),
		
		Rectf( 0.0f, 0.5f, 0.25f, 0.25f ),
		Rectf( 0.75f, 0.5f, 0.25f, 0.25f ),
		
		Rectf( 0.0f, 0.25f, 0.25f, 0.25f ),
		Rectf( 0.75f, 0.25f, 0.25f, 0.25f ),
		
		Rectf( 0.0f, 0.0f, 0.25f, 0.25f ),
		Rectf( 0.25f, 0.0f, 0.25f, 0.25f ),
		Rectf( 0.5f, 0.0f, 0.25f, 0.25f ),
		Rectf( 0.75f, 0.0f, 0.25f, 0.25f ),
	};
	
	CRIMILD_PROFILE( getName() )

	if ( _inputs.size() == 0 ) {
		return;
	}
	
	auto gBuffer = graph->createFBO( { _output } );
	gBuffer->setClearColor( RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	renderer->bindFrameBuffer( crimild::get_ptr( gBuffer ) );
	
	auto layout = LAYOUT_12;
	if ( _inputs.size() <= 4 ) layout = LAYOUT_4;
	else if ( _inputs.size() <= 8 ) layout = LAYOUT_8;

    const auto BUFFER_SIZE = Vector2f( gBuffer->getWidth(), gBuffer->getHeight() );
	
	_inputs.each( [ this, renderer, &layout, BUFFER_SIZE, idx = 0 ]( RenderGraphAttachment *input ) mutable {
		if ( idx < layout.size() ) {
            auto viewport = layout[ idx ];
            viewport.x() *= BUFFER_SIZE.x();
            viewport.y() *= BUFFER_SIZE.y();
            viewport.width() *= BUFFER_SIZE.x();
            viewport.height() *= BUFFER_SIZE.y();

			renderer->setViewport( viewport );
			render( renderer, input->getTexture() );
		}
	});
	
	renderer->unbindFrameBuffer( crimild::get_ptr( gBuffer ) );
	
	// reset viewport
    renderer->setScreenViewport();
}

void FrameDebugPass::render( Renderer *renderer, Texture *texture )
{
	auto program = crimild::get_ptr( _program );

    program->bindUniform( COLOR_MAP_UNIFORM, texture );

	renderer->bindProgram( program );

	renderer->drawScreenPrimitive( program );

	renderer->unbindProgram( program );				
}
