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

#include "NavigationMeshOBJ.hpp"

#include "Crimild_Foundation.hpp"

#include <fstream>
#include <map>
#include <sstream>

using namespace crimild;
using namespace crimild::navigation;

NavigationMeshOBJ::NavigationMeshOBJ( std::string filename )
{
#if 0
	std::ifstream input;
	input.open( filename.c_str() );
	if ( !input.is_open() ) {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot load file ", filename );
		return;
	}

	char buffer[ 1024 ];

	std::vector< Vector3f > positions;

	std::map< std::string, std::pair< NavigationCellPtr, NavigationCellEdgePtr > > edges;

	auto createEdge = [&edges]( NavigationCellPtr const &cell, const Vector3f &p0, crimild::Size i0, const Vector3f &p1, crimild::Size i1 )
	{
		std::stringstream ss;
		ss << "e" << Numerici::min( i0, i1 ) << "-" << Numerici::max( i0, i1 );
		auto name = ss.str();

		auto e = crimild::alloc< NavigationCellEdge >( LineSegment3f( p0, p1 ) );
		auto other = edges[ name ].first;
		if ( other != nullptr ) {
			e->setNeighbor( crimild::get_ptr( other ) );
			edges[ name ].second->setNeighbor( crimild::get_ptr( cell ) );
		}
		else {
			edges[ name ].first = cell;
			edges[ name ].second = e;
		}

		cell->addEdge( e );
	};

	while ( !input.eof() ) {
		input.getline( buffer, 1024 );
		std::stringstream line( buffer );

		std::string what;
		line >> what;

		if ( what == "v" ) {
			float x, y, z;
			line >> x >> y >> z;
			positions.push_back( Vector3f( x, y, z ) );
		}
		else if ( what == "f" ) {
			std::string f0, f1, f2;

			// read lines so vertices are sorted in clockwise order
			line >> f2 >> f1 >> f0;
			auto v0 = StringUtils::split< int >( f0, '/' );
			auto v1 = StringUtils::split< int >( f1, '/' );
			auto v2 = StringUtils::split< int >( f2, '/' );

			auto p0 = positions[ v0[ 0 ] - 1 ];
			auto p1 = positions[ v1[ 0 ] - 1 ];
			auto p2 = positions[ v2[ 0 ] - 1 ];

			auto cell = crimild::alloc< NavigationCell >( p0, p1, p2 );

		    createEdge( cell, p0, v0[ 0 ], p1, v1[ 0 ] );
			createEdge( cell, p1, v1[ 0 ], p2, v2[ 0 ] );
			createEdge( cell, p2, v2[ 0 ], p0, v0[ 0 ] );

			addCell( cell );
		}
	}
#endif
}

NavigationMeshOBJ::~NavigationMeshOBJ( void )
{
}
