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

#include "ShadowPass.hpp"

#include "Rendering/RenderGraph/RenderGraphAttachment.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Foundation/Profiler.hpp"
#include "Rendering/Programs/DepthShaderProgram.hpp"
#include "Rendering/ShaderGraph/Constants.hpp"
#include "Rendering/ShadowMap.hpp"

using namespace crimild;
using namespace crimild::rendergraph;
using namespace crimild::rendergraph::passes;
using namespace crimild::shadergraph;
using namespace crimild::shadergraph::locations;

ShadowPass::ShadowPass( RenderGraph *graph )
	: RenderGraphPass( graph, "Shadow Pass" )
{
#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
	_depthAttachment = graph->createAttachment(
		getName() + " - Depth",
		RenderGraphAttachment::Hint::FORMAT_DEPTH |
        RenderGraphAttachment::Hint::SIZE_2048 |
		RenderGraphAttachment::Hint::RENDER_ONLY );
	_shadowOutput = graph->createAttachment(
		getName() + " - Shadow",
		RenderGraphAttachment::Hint::FORMAT_RGBA |
		RenderGraphAttachment::Hint::WRAP_REPEAT |
        RenderGraphAttachment::Hint::SIZE_2048 |
		RenderGraphAttachment::Hint::PERSISTENT );
#else
    _shadowOutput = graph->createAttachment(
        getName() + " - Shadow",
        RenderGraphAttachment::Hint::FORMAT_DEPTH_HDR |
        RenderGraphAttachment::Hint::WRAP_CLAMP_TO_EDGES |
        RenderGraphAttachment::Hint::SIZE_4096 |
        RenderGraphAttachment::Hint::BORDER_ONE |
        RenderGraphAttachment::Hint::PERSISTENT );
#endif
}

ShadowPass::~ShadowPass( void )
{
	
}

void ShadowPass::setup( RenderGraph *graph )
{
	graph->write( this, { _depthAttachment, _shadowOutput } );
	
	_program = crimild::alloc< DepthShaderProgram >();
}

void ShadowPass::execute( RenderGraph *graph, Renderer *renderer, RenderQueue *renderQueue )
{
	CRIMILD_PROFILE( getName() )

	static const auto VIEWPORTS = containers::Array< Vector4f > {
		Vector4f( 0.0f, 0.0f, 0.5f, 0.5f ),
		Vector4f( 0.5f, 0.0f, 0.5f, 0.5f ),
		Vector4f( 0.5f, 0.5f, 0.5f, 0.5f ),
		Vector4f( 0.0f, 0.5f, 0.5f, 0.5f ),
	};

	crimild::Size lightCount = 0;
	
	auto fbo = graph->createFBO( { _depthAttachment, _shadowOutput } );
	
	renderer->bindFrameBuffer( crimild::get_ptr( fbo ) );

    const auto SHADOW_ATLAS_SIZE = Vector2f( fbo->getWidth(), fbo->getHeight() );

	renderQueue->each( [ this, renderer, renderQueue, &lightCount, SHADOW_ATLAS_SIZE ]( Light *light, int ) {
		if ( light != nullptr && light->castShadows() && lightCount < VIEWPORTS.size() ) {
			auto vp = VIEWPORTS[ lightCount++ ];
			renderer->setViewport(
                Rectf(
                    SHADOW_ATLAS_SIZE.x() * vp.x(),
                    SHADOW_ATLAS_SIZE.y() * vp.y(),
                    SHADOW_ATLAS_SIZE.x() * vp.z(),
                    SHADOW_ATLAS_SIZE.y() * vp.w()
                )
            );
			
			renderShadowMap( renderer, renderQueue, light );
			
			if ( auto shadowMap = light->getShadowMap() ) {
				shadowMap->setViewport( vp );
			}
		}
	});
	
	renderer->unbindFrameBuffer( crimild::get_ptr( fbo ) );	
}

void ShadowPass::renderShadowMap( Renderer *renderer, RenderQueue *renderQueue, Light *light )
{
	auto renderables = renderQueue->getRenderables( RenderQueue::RenderableType::SHADOW_CASTER );
	if ( renderables->size() == 0 ) {
		return;
	}

	auto program = crimild::get_ptr( _program );

	// TODO: compute a frustrum based on what the camera is looking at
    Frustumf f( -50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 100.0f );
    const auto pMatrix = f.computeOrthographicMatrix().getTranspose();
    program->bindUniform( PROJECTION_MATRIX_UNIFORM, pMatrix );

	// TODO: for diretional lights, get only the rotation of light's transform
	// and apply and offset to get a valid posistion
	Transformation lightTransform;
	lightTransform.setRotate( light->getWorld().getRotate() );
    lightTransform.setTranslate( -50.0f * lightTransform.computeDirection() );
    const auto vMatrix = lightTransform.computeModelMatrix().getInverse();
    program->bindUniform( VIEW_MATRIX_UNIFORM, vMatrix );

	if ( auto shadowMap = light->getShadowMap() ) {
		shadowMap->setLightProjectionMatrix( pMatrix );
        shadowMap->setLightViewMatrix( vMatrix );

        renderer->setCullFaceState( shadowMap->getCullFaceState() );
	}

	renderQueue->each( renderables, [ renderer, program ]( RenderQueue::Renderable *renderable ) {

		const auto &mMatrix = renderable->modelTransform;
        program->bindUniform( MODEL_MATRIX_UNIFORM, mMatrix );

		renderer->bindProgram( program );
		
		renderable->geometry->forEachPrimitive( [ renderer ]( Primitive *primitive ) {
			renderer->bindPrimitive( nullptr, primitive );
			renderer->drawPrimitive( nullptr, primitive );
			renderer->unbindPrimitive( nullptr, primitive );
		});
		
		renderer->unbindProgram( program );
	});
	
    renderer->setCullFaceState( CullFaceState::ENABLED_BACK );

}

