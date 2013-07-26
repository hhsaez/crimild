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

#include "DebugRenderPass.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Rendering/VisibilitySet.hpp"

using namespace crimild;

DebugRenderPass::DebugRenderPass( RenderPassPtr actualRenderPass )
	: _actualRenderPass( actualRenderPass ),
	  _debugMaterial( new Material() )
{
	_renderBoundings = false;
	_renderNormals = false;
}

DebugRenderPass::~DebugRenderPass( void )
{

}

void DebugRenderPass::render( Renderer *renderer, VisibilitySet *vs, Camera *camera ) 
{
	if ( _actualRenderPass != nullptr ) {
		_actualRenderPass->render( renderer, vs, camera );
	}

	vs->foreachGeometry( [&]( Geometry *geometry ) {
		if ( _renderBoundings ) {
			renderBoundings( renderer, geometry, _debugMaterial.get(), camera );
		}

		if ( _renderNormals ) {
			renderNormalsAndTangents( renderer, geometry, _debugMaterial.get(), camera );
		}
	});
}

void DebugRenderPass::renderNormalsAndTangents( Renderer *renderer, Geometry *geometry, Material *material, Camera *camera )
{
	std::vector< float > vertices;

	geometry->foreachPrimitive( [&]( PrimitivePtr primitive ) {
		VertexBufferObject *vbo = primitive->getVertexBuffer();
		const VertexFormat &vf = vbo->getVertexFormat();

		for ( int i = 0; i < vbo->getVertexCount(); i++ ) {
			Vector3f pos = vbo->getPositionAt( i );

			// render normals
			Vector3f normal = vbo->getNormalAt( i );
			vertices.push_back( pos[ 0 ] ); vertices.push_back( pos[ 1 ] ); vertices.push_back( pos[ 2 ] );
			vertices.push_back( 1.0f ); vertices.push_back( 1.0f ); vertices.push_back( 1.0f ); vertices.push_back( 1.0f );
			vertices.push_back( pos[ 0 ] + 0.05 * normal[ 0 ] ); vertices.push_back( pos[ 1 ] + 0.05 * normal[ 1 ] ); vertices.push_back( pos[ 2 ] + 0.05 * normal[ 2 ] );
			vertices.push_back( 0.0f ); vertices.push_back( 1.0f ); vertices.push_back( 0.0f ); vertices.push_back( 1.0f );

			if ( vf.hasTangents() ) {
				Vector3f tangent = vbo->getTangentAt( i );
				vertices.push_back( pos[ 0 ] ); vertices.push_back( pos[ 1 ] ); vertices.push_back( pos[ 2 ] );
				vertices.push_back( 1.0f ); vertices.push_back( 1.0f ); vertices.push_back( 1.0f ); vertices.push_back( 1.0f );
				vertices.push_back( pos[ 0 ] + 0.05 * tangent[ 0 ] ); vertices.push_back( pos[ 1 ] + 0.05 * tangent[ 1 ] ); vertices.push_back( pos[ 2 ] + 0.05 * tangent[ 2 ] );
				vertices.push_back( 1.0f ); vertices.push_back( 0.0f ); vertices.push_back( 1.0f ); vertices.push_back( 1.0f );
			}
		}
	});

	VertexFormat format = VertexFormat::VF_P3_C4;

	int vertexCount = vertices.size() / format.getVertexSize();
	std::vector< unsigned short > indices( vertexCount );
	for ( int i = 0; i < vertexCount; i++ ) {
		indices.push_back( i );
	}

	VertexBufferObjectPtr vbo( new VertexBufferObject( format, vertexCount, &vertices[ 0 ] ) );
	IndexBufferObjectPtr ibo( new IndexBufferObject( indices.size(), &indices[ 0 ] ) );
	PrimitivePtr primitive( new Primitive( Primitive::Type::LINES ) );
	primitive->setVertexBuffer( vbo );
	primitive->setIndexBuffer( ibo );

	RenderPass::render( renderer, geometry, primitive.get(), material, camera );
}

void DebugRenderPass::renderBoundings( Renderer *renderer, Geometry *geometry, Material *material, Camera *camera )
{
	SpherePrimitivePtr primitive( new SpherePrimitive( 
		geometry->getWorldBound()->getRadius(), 
		VertexFormat::VF_P3, 
		Vector2i( 30, 30 ), 
		geometry->getLocalBound()->getCenter() ) );

	material->getAlphaState()->setEnabled( true );
	material->getDepthState()->setEnabled( false );
	material->setDiffuse( RGBAColorf( 0.0f, 0.0f, 1.0f, 0.25f ) );
	
	RenderPass::render( renderer, geometry, primitive.get(), material, camera );	
}

