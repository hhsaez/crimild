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

#include "NavigationMeshContainer.hpp"

#include "Crimild_Coding.hpp"
#include "Debug/DebugRenderHelper.hpp"
#include "Navigation/NavigationMeshOBJ.hpp"
#include "Primitives/Primitive.hpp"
#include "SceneGraph/Node.hpp"
#include "Simulation/FileSystem.hpp"

using namespace crimild;
using namespace crimild::coding;
using namespace crimild::navigation;

NavigationMeshContainer::NavigationMeshContainer( void )
{
}

NavigationMeshContainer::NavigationMeshContainer( NavigationMeshPtr const &mesh )
    : _navigationMesh( mesh )
{
}

NavigationMeshContainer::~NavigationMeshContainer( void )
{
}

void NavigationMeshContainer::renderDebugInfo( Renderer *renderer, Camera *camera )
{
#if 0
	std::vector< Vector3f > exteriorEdges;
	std::vector< Vector3f > interiorEdges;
	std::vector< Vector3f > normals;
	std::vector< Vector3f > centers;

	Array< Vector3f > cells;

	static const auto OFFSET = Vector3f( 0.0f, 0.0f, 0.0f );

	getNavigationMesh()->foreachCell( [ &exteriorEdges, &interiorEdges, &cells, &normals, &centers ]( NavigationCellPtr const &cell ) {
		for ( int i = 0; i < 3; i++ ) {
			cells.add( cell->getVertex( 2 - i ) );
		}

		cell->foreachEdge( [&exteriorEdges, &interiorEdges]( NavigationCellEdgePtr const &e ) {
			if ( e->getNeighbor() != nullptr ) {
				auto l = e->getLine();
				interiorEdges.push_back( OFFSET + l.getOrigin() );
				interiorEdges.push_back( OFFSET + l.getDestination() );
			}
			else {
				auto l = e->getLine();
				exteriorEdges.push_back( OFFSET + l.getOrigin() );
				exteriorEdges.push_back( OFFSET + l.getDestination() );
			}
		});

		auto n = cell->getNormal();
		auto c = cell->getCenter();
		n *= 2.0f;
		normals.push_back( c );
		normals.push_back( c + n );

		centers.push_back( c + 0.1f * Vector3f( 1.0f, 0.0, 0.0 ) );
		centers.push_back( c + 0.1f * Vector3f( -1.0f, 0.0, 0.0 ) );
		centers.push_back( c + 0.1f * Vector3f( 0.0f, 0.0, 1.0 ) );
		centers.push_back( c + 0.1f * Vector3f( 0.0f, 0.0, -1.0 ) );
	});

	if ( exteriorEdges.size() > 0 ) {
		DebugRenderHelper::renderLines( renderer, camera, &exteriorEdges[ 0 ], exteriorEdges.size(), RGBAColorf( 0.0f, 0.0f, 0.5f, 1.0f ) );
	}

	if ( interiorEdges.size() > 0 ) {
		DebugRenderHelper::renderLines( renderer, camera, &interiorEdges[ 0 ], interiorEdges.size(), RGBAColorf( 0.75f, 0.75f, 1.0f, 1.0f ) );
	}

	if ( normals.size() > 0 ) {
		DebugRenderHelper::renderLines( renderer, camera, &normals[ 0 ], normals.size(), RGBAColorf( 0.0f, 1.0f, 0.0f, 1.0f ) );
	}

	if ( centers.size() > 0 ) {
		DebugRenderHelper::renderLines( renderer, camera, &centers[ 0 ], centers.size(), RGBAColorf( 1.0f, 1.0f, 0.0f, 1.0f ) );
	}

	if ( cells.size() > 0 ) {
		auto primitive = crimild::alloc< Primitive >();
        primitive->setVertexData(
            {
                crimild::alloc< VertexBuffer >(
                    VertexLayout::P3,
                    cells
                ),
            }
        );

        primitive->setIndices(
            [&] {
                auto indices = cells.map( [ i = 0 ]( const auto & ) mutable { return i++; } );
                return crimild::alloc< IndexBuffer >( Format::INDEX_32_UINT, indices );
            }()
        );

		Transformation t = getNode()->getWorld();
		t.translate() += OFFSET;
		DebugRenderHelper::render( renderer, camera, crimild::get_ptr( primitive ), t, RGBAColorf( 0.5f, 0.5f, 1.0f, 0.5f ) );
	}
#endif
}

void NavigationMeshContainer::encode( Encoder &encoder )
{
    NodeComponent::encode( encoder );

    // TODO
}

void NavigationMeshContainer::decode( Decoder &decoder )
{
    NodeComponent::decode( decoder );

    std::string navMeshFile;
    decoder.decode( "navMeshFile", navMeshFile );

    _navigationMesh = crimild::alloc< NavigationMeshOBJ >( FileSystem::getInstance().pathForResource( navMeshFile ) );
}
