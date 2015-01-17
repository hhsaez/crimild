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

#include "HierarchyRenderPass.hpp"
#include "Primitives/Primitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Rendering/RenderQueue.hpp"
#include "Visitors/SelectNodes.hpp"

using namespace crimild;

HierarchyRenderPass::HierarchyRenderPass( void )
    : _actualRenderPass( crimild::alloc< RenderPass >() ),
      _debugMaterial( crimild::alloc< Material >() )
{
	_renderBoundings = false;
}

HierarchyRenderPass::HierarchyRenderPass( RenderPassPtr const &actualRenderPass )
	: _actualRenderPass( actualRenderPass ),
      _debugMaterial( crimild::alloc< Material >() )
{
	_renderBoundings = false;
}

HierarchyRenderPass::~HierarchyRenderPass( void )
{

}

void HierarchyRenderPass::render( RendererPtr const &renderer, RenderQueuePtr const &renderQueue, CameraPtr const &camera )
{
    /*
	if ( _actualRenderPass != nullptr ) {
		_actualRenderPass->render( renderer, renderQueue, camera );
	}

    auto primitive = std::make_shared< SpherePrimitive >(
		0.1f, 
		VertexFormat::VF_P3, 
		Vector2i( 30, 30 ) );

    auto geometry = std::make_shared< Geometry >();

	_debugMaterial->getAlphaState()->setEnabled( true );
	_debugMaterial->getDepthState()->setEnabled( false );
	_debugMaterial->setDiffuse( RGBAColorf( 0.0f, 0.0f, 1.0f, 0.25f ) );

	std::vector< float > positions;
	
	if ( _targetScene != nullptr ) {
		_targetScene->perform( SelectNodes( [&]( NodePtr const &node ) {
			if ( node->hasParent() ) {
				positions.push_back( node->getParent()->getWorld().getTranslate()[ 0 ] );
				positions.push_back( node->getParent()->getWorld().getTranslate()[ 1 ] );
				positions.push_back( node->getParent()->getWorld().getTranslate()[ 2 ] );
			}
			else {
				positions.push_back( node->getWorld().getTranslate()[ 0 ] );
				positions.push_back( node->getWorld().getTranslate()[ 1 ] );
				positions.push_back( node->getWorld().getTranslate()[ 2 ] );
			}

			positions.push_back( node->getWorld().getTranslate()[ 0 ] );
			positions.push_back( node->getWorld().getTranslate()[ 1 ] );
			positions.push_back( node->getWorld().getTranslate()[ 2 ] );

			if ( node->getName().length() > 0 ) {
				geometry->setWorld( node->getWorld() );
				RenderPass::render( renderer, geometry, primitive, _debugMaterial, camera );
			}
			return false;
		}));
	}

	std::vector< unsigned short > indices( positions.size() / 3 );
	for ( int i = 0; i < indices.size(); i++ ) {
		indices[ i ] = i;
	}

    auto bones = std::make_shared< Primitive >( Primitive::Type::LINES );
    bones->setVertexBuffer( std::make_shared< VertexBufferObject >( VertexFormat::VF_P3, positions.size() / 3, &positions[ 0 ] ) );
	bones->setIndexBuffer( std::make_shared< IndexBufferObject >( indices.size(), &indices[ 0 ] ) );
	geometry->setWorld( TransformationImpl() );
	_debugMaterial->setDiffuse( RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f ) );
	
	RenderPass::render( renderer, geometry, bones, _debugMaterial, camera );
     */
}


void HierarchyRenderPass::renderBoundings( RendererPtr const &renderer, GeometryPtr const &geometry, MaterialPtr const &material, CameraPtr const &camera )
{
}

