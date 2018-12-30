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

#include "LightAccumulationPass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/DepthState.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Programs/ScreenColorShaderProgram.hpp"
#include "Rendering/Programs/PhongDiffuseShaderProgram.hpp"
#include "Rendering/Programs/PhongSpecularShaderProgram.hpp"
#include "Rendering/Programs/UnlitShaderProgram.hpp"
#include "Foundation/Profiler.hpp"
#include "Simulation/AssetManager.hpp"
#include "Primitives/SpherePrimitive.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;

LightAccumulationPass::LightAccumulationPass( RenderGraph *graph, std::string name )
	: RenderGraphPass( graph, name ),
	  _ambientLightProgram( crimild::alloc< ScreenColorShaderProgram >() ),
	  _directionalLightProgram( crimild::alloc< PhongDiffuseShaderProgram >() ),
	  _directionalSpecularProgram( crimild::alloc< PhongSpecularShaderProgram >() ),
	  _pointLightProgram( crimild::alloc< UnlitShaderProgram >() ),
	  _pointLightShape( crimild::alloc< SpherePrimitive >( 1.0f ) )
{
	_ambientDiffuseOutput = graph->createAttachment( getName() + " - Ambient + Diffuse Output", RenderGraphAttachment::Hint::FORMAT_RGBA );
	_specularOutput = graph->createAttachment( getName() + " - Specular Output", RenderGraphAttachment::Hint::FORMAT_RGBA );
}
			
LightAccumulationPass::~LightAccumulationPass( void )
{
	
}
			
void LightAccumulationPass::setup( RenderGraph *graph )
{
	graph->read( this, { _depthInput, _normalInput } );
	graph->write( this, { _ambientDiffuseOutput, _specularOutput } );
}

void LightAccumulationPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	accumAmbientDiffuse( graph, renderer, renderQueue );
	accumSpecular( graph, renderer, renderQueue );
}

void LightAccumulationPass::accumAmbientDiffuse( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	auto gBuffer = graph->createFBO( { _depthInput, _ambientDiffuseOutput } );
	gBuffer->setClearFlags( FrameBufferObject::ClearFlag::COLOR );
	
	renderer->bindFrameBuffer( crimild::get_ptr( gBuffer ) );

	auto projection = renderQueue->getProjectionMatrix();
	auto view = renderQueue->getViewMatrix();
	
	renderer->setAlphaState( AlphaState::ENABLED_ADDITIVE_BLEND );

	renderQueue->each( [ this, renderer, projection, view ]( Light *light, int ) {
		switch ( light->getType() ) {
			case Light::Type::AMBIENT:
				renderAmbientLight( renderer, light );
				break;

			case Light::Type::DIRECTIONAL:
				renderDirectionalLight( renderer, light, view );
				break;

			case Light::Type::POINT:
				renderPointLight( renderer, light, projection, view );
				break;

			default:
				break;
		}
	});

	renderer->setAlphaState( AlphaState::DISABLED );

	renderer->unbindFrameBuffer( crimild::get_ptr( gBuffer ) );
}

void LightAccumulationPass::accumSpecular( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	auto gBuffer = graph->createFBO( { _depthInput, _specularOutput } );
	gBuffer->setClearFlags( FrameBufferObject::ClearFlag::COLOR );
	
	renderer->bindFrameBuffer( crimild::get_ptr( gBuffer ) );

	auto P = renderQueue->getProjectionMatrix();
	auto invP = P.getInverse();
	auto V = renderQueue->getViewMatrix();
	auto invV = V.getInverse();
	
	renderer->setAlphaState( AlphaState::ENABLED_ADDITIVE_BLEND );

	renderQueue->each( [ this, renderer, invP, invV ]( Light *light, int ) {
		switch ( light->getType() ) {
			case Light::Type::DIRECTIONAL: {
				auto program = crimild::get_ptr( _directionalSpecularProgram );
				program->bindLightColor( light->getColor() );

				// compute light direction in view space
				auto d = light->getDirection();
				auto d4 = Vector4f( d.x(), d.y(), d.z(), 0.0f );
				auto vd = invV * d4;
				d = vd.xyz();
				program->bindLightDirection( d );
				program->bindInvProjMatrix( invP );
				program->bindNormalTexture( getNormalInput()->getTexture() );
				program->bindDepthTexture( getDepthInput()->getTexture() );
				
				renderer->setDepthState( DepthState::DISABLED );
				
				renderer->bindProgram( program );
				renderer->drawScreenPrimitive( program );
				renderer->unbindProgram( program );
				
				renderer->setDepthState( DepthState::ENABLED );
				break;
			}

			default:
				break;
		}
	});

	renderer->setAlphaState( AlphaState::DISABLED );

	renderer->unbindFrameBuffer( crimild::get_ptr( gBuffer ) );
}

void LightAccumulationPass::renderAmbientLight( Renderer *renderer, Light *light )
{
	auto program = crimild::get_ptr( _ambientLightProgram );
	program->setColor( light->getAmbient() );

	renderer->setDepthState( DepthState::DISABLED );

	renderer->bindProgram( program );
	renderer->drawScreenPrimitive( program );
	renderer->unbindProgram( program );

	renderer->setDepthState( DepthState::ENABLED );
}

void LightAccumulationPass::renderDirectionalLight( Renderer *renderer, Light *light, const Matrix4f &vMatrix )
{
	auto program = crimild::get_ptr( _directionalLightProgram );
	program->bindLightColor( light->getColor() );

	// compute light direction in view space
	auto d = light->getDirection();
	auto d4 = Vector4f( d.x(), d.y(), d.z(), 0.0f );
	auto vd = vMatrix.getInverse() * d4;
	d = vd.xyz();
	program->bindLightDirection( d );
	program->bindNormals( getNormalInput()->getTexture() );

	renderer->setDepthState( DepthState::DISABLED );

	renderer->bindProgram( program );
	renderer->drawScreenPrimitive( program );
	renderer->unbindProgram( program );

	renderer->setDepthState( DepthState::ENABLED );
}

void LightAccumulationPass::renderPointLight( Renderer *renderer, Light *light, const Matrix4f &pMatrix, const Matrix4f &vMatrix )
{
	auto program = crimild::get_ptr( _pointLightProgram );
	auto p = crimild::get_ptr( _pointLightShape );

	auto mMatrix = light->getWorld().computeModelMatrix();

	renderer->bindProgram( program );

	renderer->bindUniform(
		program->getStandardLocation( ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM ),
		pMatrix
	);
	renderer->bindUniform(
		program->getStandardLocation( ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM ),
		vMatrix
	);
	renderer->bindUniform(
		program->getStandardLocation( ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM ),
		mMatrix
	);
	renderer->bindTexture(
		program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ),
		renderer->getFallbackTexture()
	);
	renderer->bindUniform(
		program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM ),
		light->getColor()
	);

	//program->setPMatrix( pMatrix );
	//program->setVMatrix( vMatrix );
	//program->setLight( light );
	//program->setMMatrix( mMatrix );
	//program->setColor( light->getColor() );

	renderer->bindPrimitive( program, p );
	renderer->drawPrimitive( program, p );
	renderer->unbindPrimitive( program, p );

	renderer->unbindTexture(
		program->getStandardLocation( ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM ),
		renderer->getFallbackTexture()
	);

	renderer->unbindProgram( program );
}

