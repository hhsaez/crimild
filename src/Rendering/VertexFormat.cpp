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

#include "VertexFormat.hpp"

using namespace Crimild;

const VertexFormat VertexFormat::VF_P3( 3, 0, 0 );
const VertexFormat VertexFormat::VF_P3_UV2( 3, 0, 2 );
const VertexFormat VertexFormat::VF_P3_N3( 3, 3, 0 );
const VertexFormat VertexFormat::VF_P3_N3_UV2( 3, 3, 2 );

VertexFormat::VertexFormat( unsigned int positions, unsigned int normals, unsigned int textureCoords )
{
	_positions = positions;
	_positionsOffset = 0;

	_normals = normals;
	_normalsOffset = _positionsOffset + _positions;

	_textureCoords = textureCoords;
	_textureCoordsOffset = _normalsOffset + _normals;

	_vertexSize = _positions + _normals + _textureCoords;
	_vertexSizeInBytes = sizeof( float ) * _vertexSize;
}

VertexFormat::VertexFormat( const VertexFormat &vf )
{
	_positions = vf._positions;
	_positionsOffset = vf._positionsOffset;

	_normals = vf._normals;
	_normalsOffset = vf._normalsOffset;

	_textureCoords = vf._textureCoords;
	_textureCoordsOffset = vf._textureCoordsOffset;

	_vertexSize = vf._vertexSize;
	_vertexSizeInBytes = vf._vertexSizeInBytes;
}

VertexFormat::~VertexFormat( void )
{

}

VertexFormat &VertexFormat::operator=( const VertexFormat &vf )
{
	_positions = vf._positions;
	_positionsOffset = vf._positionsOffset;

	_normals = vf._normals;
	_normalsOffset = vf._normalsOffset;

	_textureCoords = vf._textureCoords;
	_textureCoordsOffset = vf._textureCoordsOffset;

	_vertexSize = vf._vertexSize;
	_vertexSizeInBytes = vf._vertexSizeInBytes;

	return *this;
}

bool VertexFormat::operator==( const VertexFormat &vf ) const
{
	return ( _positions == vf._positions ) &&
		   ( _positionsOffset == vf._positionsOffset ) &&
		   ( _normals == vf._normals ) && 
		   ( _normalsOffset == vf._normalsOffset ) &&
		   ( _textureCoords == vf._textureCoords ) &&
		   ( _textureCoordsOffset == vf._textureCoordsOffset ) &&
		   ( _vertexSize == vf._vertexSize ) &&
		   ( _vertexSizeInBytes == vf._vertexSizeInBytes );

}

bool VertexFormat::operator!=( const VertexFormat &vf ) const
{
	return !( *this == vf );
}

std::ostream &VertexFormat::operator<<( std::ostream &out )
{
	out << "{p: " << getPositionComponents() 
		<< ", n: " << getNormalComponents() 
		<< ", tc: " << getTextureCoordComponents();
	return out;
}

