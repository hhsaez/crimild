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

#include "VertexBufferObject.hpp"

using namespace crimild;

VertexBufferObject::VertexBufferObject( const VertexFormat &vf, unsigned int vertexCount, const float *vertexData )
	: BufferObject( vertexCount * vf.getVertexSize(), vertexData ),
	  _vertexFormat( vf ),
	  _vertexCount( vertexCount )
{
}

VertexBufferObject::~VertexBufferObject( void )
{
}

Vector3f VertexBufferObject::getPositionAt( unsigned int index ) const
{
	const float *data = &( getData()[ index * _vertexFormat.getVertexSize() + _vertexFormat.getPositionsOffset() ] );
	return Vector3f( data[ 0 ], data[ 1 ], data[ 2 ] );
}

void VertexBufferObject::setPositionAt( unsigned int index, const Vector3f &position )
{
	float *data = &( getData()[ index * _vertexFormat.getVertexSize() + _vertexFormat.getPositionsOffset() ] );
	data[ 0 ] = position[ 0 ];
	data[ 1 ] = position[ 1 ];
	data[ 2 ] = position[ 2 ];
}

RGBAColorf VertexBufferObject::getRGBAColorAt( unsigned int index ) const
{
	const float *data = &( getData()[ index * _vertexFormat.getVertexSize() + _vertexFormat.getColorsOffset() ] );
	return RGBAColorf( data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ] );
}

RGBColorf VertexBufferObject::getRGBColorAt( unsigned int index ) const
{
	const float *data = &( getData()[ index * _vertexFormat.getVertexSize() + _vertexFormat.getColorsOffset() ] );
	return RGBColorf( data[ 0 ], data[ 1 ], data[ 2 ] );
}

Vector3f VertexBufferObject::getNormalAt( unsigned int index ) const
{
	const float *data = &( getData()[ index * _vertexFormat.getVertexSize() + _vertexFormat.getNormalsOffset() ] );
	return Vector3f( data[ 0 ], data[ 1 ], data[ 2 ] );
}

Vector3f VertexBufferObject::getTangentAt( unsigned int index ) const
{
	const float *data = &( getData()[ index * _vertexFormat.getVertexSize() + _vertexFormat.getTangentsOffset() ] );
	return Vector3f( data[ 0 ], data[ 1 ], data[ 2 ] );
}

Vector2f VertexBufferObject::getTextureCoordAt( unsigned int index ) const
{
	const float *data = &( getData()[ index * _vertexFormat.getVertexSize() + _vertexFormat.getTextureCoordsOffset() ] );
	return Vector2f( data[ 0 ], data[ 1 ] );
}

