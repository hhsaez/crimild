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

#ifndef CRIMILD_RENDERING_VERTEX_FORMAT_
#define CRIMILD_RENDERING_VERTEX_FORMAT_

#include <memory>
#include <iostream>

namespace crimild {

	class VertexFormat {
	public:
		static const VertexFormat VF_P3;
		static const VertexFormat VF_P3_C4;
		static const VertexFormat VF_P3_UV2;
		static const VertexFormat VF_P3_C4_UV2;
		static const VertexFormat VF_P3_N3;
		static const VertexFormat VF_P3_N3_TG3;
		static const VertexFormat VF_P3_N3_UV2;
		static const VertexFormat VF_P3_N3_TG3_UV2;

	public:
		VertexFormat( void );
		VertexFormat( unsigned char positions, unsigned char colors, unsigned char normals, unsigned char tangents, unsigned char textureCoords );
		VertexFormat( unsigned char positions, unsigned char colors, unsigned char normals, unsigned char tangents, unsigned char textureCoords, unsigned char boneIds, unsigned char boneWeights );
		VertexFormat( const VertexFormat &vf );
		~VertexFormat( void );

		VertexFormat &operator=( const VertexFormat &vf );

		bool operator==( const VertexFormat &vf ) const;
		bool operator!=( const VertexFormat &vf ) const;

		bool hasPositions( void ) const { return _positions > 0; }
		unsigned char getPositionComponents( void ) const { return _positions; }
		unsigned char getPositionsOffset( void ) const { return _positionsOffset; }

		bool hasColors( void ) const { return _colors > 0; }
		unsigned char getColorComponents( void ) const { return _colors; }
		unsigned char getColorsOffset( void ) const { return _colorsOffset; }

		bool hasNormals( void ) const { return _normals > 0; }
		unsigned char getNormalComponents( void ) const { return _normals; }
		unsigned char getNormalsOffset( void ) const { return _normalsOffset; }

		bool hasTangents( void ) const { return _tangents > 0; }
		unsigned char getTangentComponents( void ) const { return _tangents; }
		unsigned char getTangentsOffset( void ) const { return _tangentsOffset; }

		bool hasTextureCoords( void ) const { return _textureCoords > 0; }
		unsigned char getTextureCoordComponents( void ) const { return _textureCoords; }
		unsigned char getTextureCoordsOffset( void ) const { return _textureCoordsOffset; }

		bool hasBoneIds( void ) const { return _boneIds > 0; }
		unsigned char getBoneIdComponents( void ) const { return _boneIds; }
		unsigned char getBoneIdsOffset( void ) const { return _boneIdsOffset; }

		bool hasBoneWeights( void ) const { return _boneWeights > 0; }
		unsigned char getBoneWeightComponents( void ) const { return _boneWeights; }
		unsigned char getBoneWeightsOffset( void ) const { return _boneWeightsOffset; }

		unsigned char getVertexSize( void ) const { return _vertexSize; }

		unsigned int getVertexSizeInBytes( void ) const { return _vertexSizeInBytes; }

	private:
		unsigned char _positions;
		unsigned char _positionsOffset;
		unsigned char _colors;
		unsigned char _colorsOffset;
		unsigned char _normals;
		unsigned char _normalsOffset;
		unsigned char _tangents;
		unsigned char _tangentsOffset;
		unsigned char _textureCoords;
		unsigned char _textureCoordsOffset;
		unsigned char _boneIds;
		unsigned char _boneIdsOffset;
		unsigned char _boneWeights;
		unsigned char _boneWeightsOffset;
		unsigned char _vertexSize;
		unsigned int _vertexSizeInBytes;
	};

	std::ostream &operator<<( std::ostream &out, const crimild::VertexFormat &vf );

}

#endif

