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

#include "NavigationCell.hpp"

using namespace crimild;
using namespace crimild::navigation;

NavigationCell::NavigationCell( const Vector3f &v0, const Vector3f &v1, const Vector3f &v2 )
{
	_vertices[ 0 ] = v0;
	_vertices[ 1 ] = v1;
	_vertices[ 2 ] = v2;

	_center = ( _vertices[ 0 ] + _vertices[ 1 ] + _vertices[ 2 ] ) / 3.0f;
	
	_normal = ( ( v2 - v0 ) ^ ( v1 - v0 ) ).getNormalized();

	_plane = Plane3f( v0, v1, v2 );
}

NavigationCell::~NavigationCell( void )
{

}

bool NavigationCell::containsPoint( const Vector3f &p ) const 
{
	auto sameSide = []( const Vector3f &p1, const Vector3f &p2, const Vector3f &a, const Vector3f &b ) -> bool
	{
		auto cp1 = ( b - a ) ^ ( p1 - a );
		auto cp2 = ( b - a ) ^ ( p2 - a );
		return ( cp1 * cp2 ) >= 0;
	};

	auto a = _vertices[ 0 ];
	auto b = _vertices[ 1 ];
	auto c = _vertices[ 2 ];
	return sameSide( p, a, b, c ) && sameSide( p, b, a, c ) && sameSide( p, c, a, b );
}

