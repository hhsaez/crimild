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

#include "BlendPass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Programs/ScreenTextureShaderProgram.hpp"
#include "Foundation/Profiler.hpp"
#include "Simulation/AssetManager.hpp"
#include "Rendering/ShaderGraph/Constants.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;
using namespace crimild::shadergraph::locations;

BlendPass::BlendPass( RenderGraph *graph, SharedPointer< AlphaState > const &alphaState )
	: RenderGraphPass( graph, "Blend Pass" ),
	  _alphaState( alphaState )
{
	_output = graph->createAttachment( getName() + " - Output", RenderGraphAttachment::Hint::FORMAT_RGBA );
    _program = crimild::alloc< ScreenTextureShaderProgram >();
}
			
BlendPass::~BlendPass( void )
{
	
}
			
void BlendPass::setup( RenderGraph *graph )
{
	graph->read( this, _inputs );
	graph->write( this, { _output } );
}

void BlendPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	CRIMILD_PROFILE( getName() )

	if ( _inputs.size() == 0 ) {
		return;
	}
	
    auto fbo = graph->createFBO( { _output } );
	
	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );
	
    auto program = crimild::get_ptr( _program );

	_inputs.each( [ this, renderer, program ]( RenderGraphAttachment *input, crimild::Size idx ) {
		auto texture = input->getTexture();

		if ( idx > 0 ) {
			renderer->setAlphaState( _alphaState );
			renderer->setDepthState( DepthState::DISABLED );
		}

        program->bindUniform( COLOR_MAP_UNIFORM, texture );
		
        renderer->bindProgram( program );

		renderer->drawScreenPrimitive( program );

        renderer->unbindProgram( program );

        if ( idx > 0 ) {
			renderer->setAlphaState( AlphaState::DISABLED );
			renderer->setDepthState( DepthState::ENABLED );
		}
	});

	renderer->unbindFrameBuffer( crimild::get_ptr( fbo ) );
}

