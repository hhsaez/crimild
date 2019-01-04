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

#include "DeferredLightingPass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/DepthState.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Programs/PhongDeferredLightingShaderProgram.hpp"
#include "Foundation/Profiler.hpp"
#include "Simulation/AssetManager.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;

DeferredLightingPass::DeferredLightingPass( RenderGraph *graph, std::string name )
	: RenderGraphPass( graph, name ),
	  _program( crimild::alloc< PhongDeferredLightingShaderProgram >() )
{
	_colorOutput = graph->createAttachment( getName() + " - Color Output", RenderGraphAttachment::Hint::FORMAT_RGBA );
}
			
DeferredLightingPass::~DeferredLightingPass( void )
{
	
}
			
void DeferredLightingPass::setup( RenderGraph *graph )
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
	
	graph->read( this, { _depthInput, _ambientAccumInput, _diffuseAccumInput, _specularAccumInput } );
	graph->write( this, { _colorOutput } );
	
}

void DeferredLightingPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	auto fbo = graph->createFBO( { _depthInput, _colorOutput } );
	fbo->setClearFlags( _clearFlags );
	
	CRIMILD_PROFILE( getName() )
				
	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );

	auto renderables = renderQueue->getRenderables( RenderQueue::RenderableType::OPAQUE );
	if ( renderables->size() == 0 ) {
		return;
	}

	auto program = crimild::get_ptr( _program );

	auto pMatrix = renderQueue->getProjectionMatrix();
	program->bindProjMatrix( pMatrix );
	
	auto vMatrix = renderQueue->getViewMatrix();
	program->bindViewMatrix( vMatrix );

	program->bindLightAmbientTexture( _ambientAccumInput != nullptr ? _ambientAccumInput->getTexture() : crimild::get_ptr( Texture::ZERO ) );
	program->bindLightDiffuseTexture( _diffuseAccumInput != nullptr ? _diffuseAccumInput->getTexture() : crimild::get_ptr( Texture::ZERO ) );
	program->bindLightSpecularTexture( _specularAccumInput != nullptr ? _specularAccumInput->getTexture() : crimild::get_ptr( Texture::ZERO ) );

	auto size = Vector2f(
		renderer->getScreenBuffer()->getWidth(),
		renderer->getScreenBuffer()->getHeight()
	);
	program->bindScreenSize( size );

	renderer->setDepthState( _depthState );

	renderQueue->each( renderables, [ this, renderer, program ]( RenderQueue::Renderable *renderable ) {
		auto material = renderable->material;
		if ( material != nullptr ) {
			program->bindMaterialAmbient( material->getAmbient() );
			program->bindMaterialDiffuse( material->getDiffuse() );
			program->bindMaterialSpecular( material->getSpecular() );

			program->bindMaterialColorMap( material->getColorMap() != nullptr ? material->getColorMap() : crimild::get_ptr( Texture::ONE ) );
			program->bindMaterialSpecularMap( material->getSpecularMap() != nullptr ? material->getSpecularMap() : crimild::get_ptr( Texture::ONE ) );
		} else {
			program->bindMaterialAmbient( RGBAColorf::ZERO );
			program->bindMaterialDiffuse( RGBAColorf::ZERO );
			program->bindMaterialSpecular( RGBAColorf::ZERO );
			program->bindMaterialColorMap( crimild::get_ptr( Texture::ONE ) );
			program->bindMaterialSpecularMap( crimild::get_ptr( Texture::ONE ) );
		}

		const auto &mMatrix = renderable->modelTransform;
		program->bindModelMatrix( mMatrix );

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

