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

#include "ScreenPass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Programs/UnlitShaderProgram.hpp"
#include "Rendering/ShaderGraph/Constants.hpp"
#include "Foundation/Profiler.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;
using namespace crimild::shadergraph::locations;

ScreenPass::ScreenPass( RenderGraph *graph )
	: RenderGraphPass( graph, "Screen Pass" )
{
	_depthOutput = graph->createAttachment( getName() + " - Depth", RenderGraphAttachment::Hint::FORMAT_DEPTH | RenderGraphAttachment::Hint::RENDER_ONLY );
	_output = graph->createAttachment( getName() + " - Color", RenderGraphAttachment::Hint::FORMAT_RGBA );
}
			
ScreenPass::~ScreenPass( void )
{
	
}
			
void ScreenPass::setup( RenderGraph *graph )
{
	graph->write( this, { _depthOutput, _output } );
}

void ScreenPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	CRIMILD_PROFILE( "Render Screen Objects" )
	
	auto fbo = graph->createFBO( { _depthOutput, _output } );
	
	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );
	
	auto renderables = renderQueue->getRenderables( RenderQueue::RenderableType::SCREEN );
	if ( renderables->size() == 0 ) {
		return;
	}
	
	crimild::Real32 width = renderer->getScreenBuffer()->getWidth();
	crimild::Real32 height = renderer->getScreenBuffer()->getHeight();
	auto aspect = width / height;
    auto f = Frustumf( -aspect, aspect, -1.0, 1.0, -100.0, 100.0 );
	auto pMatrix = f.computeOrthographicMatrix();
	auto vMatrix = Matrix4f();
	vMatrix.makeIdentity();
	
	renderQueue->each( renderables, [ renderer, pMatrix, vMatrix ]( RenderQueue::Renderable *renderable ) {
        auto color = RGBAColorf::ONE;
        auto colorMap = crimild::get_ptr( Texture::ONE );
        auto alphaState = crimild::get_ptr( AlphaState::DISABLED );

        auto program = static_cast< ShaderProgram * >( AssetManager::getInstance()->get< UnlitShaderProgram >() );
        if ( auto material = crimild::get_ptr( renderable->material ) ) {
            if ( material->getProgram() ) program = material->getProgram();

            color = material->getDiffuse();
            if ( material->getColorMap() ) colorMap = material->getColorMap();

            if ( material->getAlphaState() ) alphaState = material->getAlphaState();
		}

        program->bindUniform( PROJECTION_MATRIX_UNIFORM, pMatrix );
        program->bindUniform( VIEW_MATRIX_UNIFORM, vMatrix );
        program->bindUniform( MODEL_MATRIX_UNIFORM, renderable->modelTransform );

        program->bindUniform( COLOR_UNIFORM, color );
        program->bindUniform( COLOR_MAP_UNIFORM, colorMap );

		renderer->bindProgram( program );

		renderer->setDepthState( DepthState::DISABLED );
        renderer->setAlphaState( alphaState );

        renderable->geometry->forEachPrimitive( [ renderer ]( Primitive *primitive ) {
            renderer->bindPrimitive( nullptr, primitive );
            renderer->drawPrimitive( nullptr, primitive );
            renderer->unbindPrimitive( nullptr, primitive );
        });

		renderer->setDepthState( DepthState::ENABLED );
        renderer->setAlphaState( AlphaState::DISABLED );

		renderer->unbindProgram( program );
	});
	
	renderer->unbindFrameBuffer( crimild::get_ptr( fbo ) );
}

