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

#include "MeshCollider.hpp"

using namespace crimild;
using namespace crimild::physics;

MeshCollider::MeshCollider( void )
{

}

MeshCollider::~MeshCollider( void )
{

}

SharedPointer< btCollisionShape > MeshCollider::generateShape( void ) 
{
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Generating shape for mesh collider" );

	auto mesh = new btTriangleMesh(); // is this a leak?

	getNode()->perform( ApplyToGeometries( [mesh]( Geometry *geometry ) {
		geometry->forEachPrimitive( [mesh]( Primitive *primitive ) {
			auto ibo = primitive->getIndexBuffer();
			auto vbo = primitive->getVertexBuffer();

			const unsigned short *indices = static_cast< const unsigned short * >( ibo->getData() );
			Vector3f vertices[ 3 ];
			for ( int i = 0; i < ibo->getIndexCount() / 3; i++ ) {
				for ( int j = 0; j < 3; j++ ) {
					vertices[ j ] = vbo->getPositionAt( indices[ i * 3 + j ] );
				}
				mesh->addTriangle( BulletUtils::convert( vertices[ 0 ] ), BulletUtils::convert( vertices[ 1 ] ), BulletUtils::convert( vertices[ 2 ] ) );
			}
		});
	}));

	return crimild::alloc< btBvhTriangleMeshShape >( mesh, true );
}

void MeshCollider::renderDebugInfo( Renderer *renderer, Camera *camera )
{
	DebugRenderHelper::renderSphere( 
		renderer, 
		camera, 
		getNode()->getWorldBound()->getCenter(), 
		getNode()->getWorldBound()->getRadius(), 
		RGBAColorf( 1.0f, 0.0f, 0.0f, 0.5f ) );
}

