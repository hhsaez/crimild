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

#include "TextureColorPass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Material.hpp"
#include "Foundation/Profiler.hpp"
#include "Rendering/Programs/ScreenTextureShaderProgram.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;
using namespace crimild::shadergraph;

TextureColorPass::TextureColorPass( RenderGraph *graph )
	: RenderGraphPass( graph, "Show texture color" ),
	  _program( crimild::alloc< ScreenTextureShaderProgram >( ScreenTextureShaderProgram::Mode::ALPHA ) )
{

}
			
TextureColorPass::~TextureColorPass( void )
{
	
}
			
void TextureColorPass::setup( RenderGraph *graph )
{
	graph->read( this, { _input } );
	graph->write( this, { _output } );
}

void TextureColorPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	CRIMILD_PROFILE( getName() );

	if ( _input == nullptr || _input->getTexture() == nullptr ) {
		return;
	}

    auto fbo = graph->createFBO( { _output } );
	
	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );
	
	auto program = crimild::get_ptr( _program );
	auto texture = _input->getTexture();

	renderer->bindProgram( program );
	
	renderer->bindTexture(
		program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ),
		texture );
	
	renderer->drawScreenPrimitive( program );
	
	renderer->unbindTexture(
		program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_MAP_UNIFORM ),
		texture );
	
	renderer->unbindProgram( program );				
	
	renderer->unbindFrameBuffer( crimild::get_ptr( fbo ) );
}

