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

#include "SceneGraph/GeometryNode.hpp"
#include "Components/MaterialComponent.hpp"

using namespace Crimild;

Renderer::Renderer( void )
{

}

Renderer::~Renderer( void )
{

}

void Renderer::render( VisibilitySet *vs )
{
	vs->foreachGeometry( [&]( GeometryNode *geometry ) mutable {
		render( geometry );
	});
}

void Renderer::render( GeometryNode *geometry )
{
	MaterialComponent *materials = geometry->getComponent< MaterialComponent >();
	if ( materials->hasMaterials() ) {
		geometry->foreachPrimitive( [&]( PrimitivePtr &primitive ) mutable {
			materials->foreachMaterial( [&]( MaterialPtr &material ) mutable {
				applyMaterial( geometry, primitive.get(), material.get() );
			});
		});
	}
	else {
		geometry->foreachPrimitive( [&]( PrimitivePtr &primitive ) mutable {
			applyMaterial( geometry, primitive.get(), getDefaultMaterial() );
		});
	}
}

void Renderer::applyMaterial( GeometryNode *geometry, Primitive *primitive, Material *material )
{
	if ( !material || !primitive ) {
		return;
	}

	ShaderProgram *program = material->getProgram() ? material->getProgram() : getDefaultMaterial()->getProgram();
	if ( !program ) {
		return;
	}

	enableShaderProgram( program );
	enableTextures( program, material );
	enableVertexBuffer( program, primitive->getVertexBuffer() );
	enableIndexBuffer( program, primitive->getIndexBuffer() );
	applyTransformations( program, geometry );

	drawPrimitive( program, primitive );

	restoreTransformations( program, geometry );
	disableIndexBuffer( program, primitive->getIndexBuffer() );
	disableVertexBuffer( program, primitive->getVertexBuffer() );
	disableTextures( program, material );
	disableShaderProgram( program );
}

void Renderer::enableShaderProgram( ShaderProgram *program )
{

}

void Renderer::enableTextures( ShaderProgram *program, Material *material )
{

}

void Renderer::enableVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo )
{

}

void Renderer::enableIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo )
{

}

void Renderer::applyTransformations( ShaderProgram *program, GeometryNode *geometry )
{

}

void Renderer::drawPrimitive( ShaderProgram *program, Primitive *primitive )
{

}

void Renderer::restoreTransformations( ShaderProgram *program, GeometryNode *geometry )
{

}

void Renderer::disableIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo )
{

}

void Renderer::disableVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo )
{

}

void Renderer::disableTextures( ShaderProgram *program, Material *material )
{

}

void Renderer::disableShaderProgram( ShaderProgram *program )
{
	
}

