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

#include "Renderer.hpp"
#include "VisibilitySet.hpp"

#include "SceneGraph/Geometry.hpp"
#include "Components/MaterialComponent.hpp"

using namespace Crimild;

Renderer::Renderer( void )
	: _shaderProgramCatalog( new Catalog< ShaderProgram >() ),
	  _textureCatalog( new Catalog< Texture >() ),
	  _vertexBufferObjectCatalog( new Catalog< VertexBufferObject >() ),
	  _indexBufferObjectCatalog( new Catalog< IndexBufferObject >() )
{
}

Renderer::~Renderer( void )
{
}

void Renderer::render( VisibilitySet *vs )
{
	vs->foreachGeometry( [&]( Geometry *geometry ) mutable {
		render( geometry, vs->getCamera() );
	});
}

void Renderer::render( Geometry *geometry, Camera *camera )
{
	MaterialComponent *materials = geometry->getComponent< MaterialComponent >();
	if ( materials->hasMaterials() ) {
		geometry->foreachPrimitive( [&]( PrimitivePtr &primitive ) mutable {
			materials->foreachMaterial( [&]( MaterialPtr &material ) mutable {
				applyMaterial( geometry, primitive.get(), material.get(), camera );
			});
		});
	}
	else {
		geometry->foreachPrimitive( [&]( PrimitivePtr &primitive ) mutable {
			applyMaterial( geometry, primitive.get(), getDefaultMaterial(), camera );
		});
	}
}

void Renderer::applyMaterial( Geometry *geometry, Primitive *primitive, Material *material, Camera *camera )
{
	if ( !material || !primitive ) {
		return;
	}

	ShaderProgram *program = material->getProgram() ? material->getProgram() : getFallbackProgram( material, primitive );
	if ( !program ) {
		return;
	}

	bindResources( program, primitive, material );
	enableMaterialProperties( program, material );
	applyTransformations( program, geometry, camera );
	drawPrimitive( program, primitive );
	restoreTransformations( program, geometry, camera );
	disableMaterialProperties( program, material );
	unbindResources( program, primitive, material );
}

void Renderer::bindResources( ShaderProgram *program, Primitive *primitive, Material *material )
{
	getShaderProgramCatalog()->bind( program );
	
	if ( material->getColorMap() ) {
		getTextureCatalog()->bind( program, material->getColorMap() );
	}
	
	getVertexBufferObjectCatalog()->bind( program, primitive->getVertexBuffer() );
	getIndexBufferObjectCatalog()->bind( program, primitive->getIndexBuffer() );
}

void Renderer::unbindResources( ShaderProgram *program, Primitive *primitive, Material *material )
{
	getIndexBufferObjectCatalog()->unbind( program, primitive->getIndexBuffer() );
	getVertexBufferObjectCatalog()->unbind( program, primitive->getVertexBuffer() );

	if ( material->getColorMap() ) {
		getTextureCatalog()->unbind( program, material->getColorMap() );
	}
	
	getShaderProgramCatalog()->unbind( program );
}

