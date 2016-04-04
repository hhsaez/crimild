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

VertexBufferObject::VertexBufferObject( const VertexFormat &vf, unsigned int vertexCount )
	: VertexBufferObject( vf, vertexCount, nullptr )
{

}

VertexBufferObject::VertexBufferObject( const VertexFormat &vf, unsigned int vertexCount, const VertexPrecision *vertexData )
	: BufferObject( vertexCount * vf.getVertexSize(), vertexData ),
	  _vertexFormat( vf ),
	  _vertexCount( vertexCount )
{

}

VertexBufferObject::~VertexBufferObject( void )
{

}

const VertexBufferObject::Vector3Impl &VertexBufferObject::getPositionAt( unsigned int vIdx ) const
{
	return *( ( VertexBufferObject::Vector3Impl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getPositionsOffset() ] ) );
}

void VertexBufferObject::setPositionAt( unsigned int vIdx, const VertexBufferObject::Vector3Impl &value )
{
	auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getPositionsOffset() ];
	memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getPositionComponents() );
}

VertexBufferObject::RGBAColorImpl VertexBufferObject::getRGBAColorAt( unsigned int vIdx ) const
{
	return *( ( VertexBufferObject::RGBAColorImpl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getColorsOffset() ] ) );
}

void VertexBufferObject::setRGBAColorAt( unsigned int vIdx, const VertexBufferObject::RGBAColorImpl &value )
{
	auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getColorsOffset() ];
	memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getColorComponents() );
}

VertexBufferObject::RGBColorImpl VertexBufferObject::getRGBColorAt( unsigned int vIdx ) const
{
	return *( ( VertexBufferObject::RGBColorImpl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getColorsOffset() ] ) );
}

void VertexBufferObject::setRGBColorAt( unsigned int vIdx, const VertexBufferObject::RGBColorImpl &value )
{
	auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getColorsOffset() ];
	memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getColorComponents() );
}

VertexBufferObject::Vector3Impl VertexBufferObject::getNormalAt( unsigned int vIdx ) const
{
	return *( ( VertexBufferObject::Vector3Impl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getNormalsOffset() ] ) );
}

void VertexBufferObject::setNormalAt( unsigned int vIdx, const VertexBufferObject::Vector3Impl &value )
{
	auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getNormalsOffset() ];
	memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getNormalComponents() );
}

VertexBufferObject::Vector3Impl VertexBufferObject::getTangentAt( unsigned int vIdx ) const
{
	return *( ( VertexBufferObject::Vector3Impl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getTangentsOffset() ] ) );
}

void VertexBufferObject::setTangentAt( unsigned int vIdx, const VertexBufferObject::Vector3Impl &value )
{
	auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getTangentsOffset() ];
	memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getTangentComponents() );
}

VertexBufferObject::Vector2Impl VertexBufferObject::getTextureCoordAt( unsigned int vIdx ) const
{
	return *( ( VertexBufferObject::Vector2Impl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getTextureCoordsOffset() ] ) );
}

void VertexBufferObject::setTextureCoordAt( unsigned int vIdx, const VertexBufferObject::Vector2Impl &value )
{
	auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getTextureCoordsOffset() ];
	memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getTextureCoordComponents() );
}

VertexPrecision VertexBufferObject::getBoneIdAt( unsigned int vIdx, unsigned int bone ) const
{
	return getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getBoneIdsOffset() + bone ];
}

void VertexBufferObject::setBoneIdAt( unsigned int vIdx, unsigned int bone, VertexPrecision value )
{
	float *d = data();
	d[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getBoneIdsOffset() + bone ] = value;
}

VertexPrecision VertexBufferObject::getBoneWeightAt( unsigned int vIdx, unsigned int bone ) const
{
	return getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getBoneWeightsOffset() + bone ];
}

void VertexBufferObject::setBoneWeightAt( unsigned int vIdx, unsigned int bone, VertexPrecision value )
{
	float *d = data();
	d[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getBoneWeightsOffset() + bone ] = value;
}

