/*
 * Copyright (c) 2013, Hernan Saez
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

#include "RenderPass.hpp"
#include "Renderer.hpp"
#include "VisibilitySet.hpp"

#include "SceneGraph/Geometry.hpp"
#include "Components/RenderStateComponent.hpp"

using namespace Crimild;

RenderPass::RenderPass( void )
{

}

RenderPass::~RenderPass( void )
{

}

void RenderPass::render( Renderer *renderer, VisibilitySet *vs, Camera *camera )
{
	vs->foreachGeometry( [&]( Geometry *geometry ) mutable {
		render( renderer, geometry, camera );
	});
}

void RenderPass::render( Renderer *renderer, Geometry *geometry, Camera *camera )
{
	RenderStateComponent *renderState = geometry->getComponent< RenderStateComponent >();
	if ( renderState->hasMaterials() ) {
		geometry->foreachPrimitive( [&]( PrimitivePtr &primitive ) mutable {
			renderState->foreachMaterial( [&]( Material *material ) mutable {
				render( renderer, geometry, primitive.get(), material, camera );
			});
		});
	}
}

void RenderPass::render( Renderer *renderer, Geometry *geometry, Primitive *primitive, Material *material, Camera *camera )
{
	if ( !material || !primitive ) {
		return;
	}

	ShaderProgram *program = material->getProgram() ? material->getProgram() : renderer->getFallbackProgram( material, geometry, primitive );
	if ( !program ) {
		return;
	}

	RenderStateComponent *renderState = geometry->getComponent< RenderStateComponent >();

	// bind shader program first
	renderer->bindProgram( program );

	// bind material properties
	renderer->bindMaterial( program, material );

	// bind lights
	if ( renderState->hasLights() ) {
		renderState->foreachLight( [&]( Light *light ) {
			renderer->bindLight( program, light );
		});
	}

	// bind vertex and index buffers
	renderer->bindVertexBuffer( program, primitive->getVertexBuffer() );
	renderer->bindIndexBuffer( program, primitive->getIndexBuffer() );

	// apply transformations
	renderer->applyTransformations( program, geometry, camera );

	// draw primitive
	renderer->drawPrimitive( program, primitive );

	// restore transformation stack
	renderer->restoreTransformations( program, geometry, camera );

	// unbind primitive buffers
	renderer->unbindVertexBuffer( program, primitive->getVertexBuffer() );
	renderer->unbindIndexBuffer( program, primitive->getIndexBuffer() );

	// unbind lights
	if ( renderState->hasLights() ) {
		renderState->foreachLight( [&]( Light *light ) {
			renderer->unbindLight( program, light );
		});
	}

	// unbind material properties
	renderer->unbindMaterial( program, material );

	// lastly, unbind the shader program
	renderer->unbindProgram( program );
}

