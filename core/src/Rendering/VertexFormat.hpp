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
		static const VertexFormat VF_P3_N3;
		static const VertexFormat VF_P3_N3_UV2;

	public:
		VertexFormat( unsigned int positions, unsigned int colors, unsigned int normals, unsigned int textureCoords );
		VertexFormat( const VertexFormat &vf );
		~VertexFormat( void );

		VertexFormat &operator=( const VertexFormat &vf );

		bool operator==( const VertexFormat &vf ) const;
		bool operator!=( const VertexFormat &vf ) const;

		bool hasPositions( void ) const { return _positions > 0; }
		unsigned int getPositionComponents( void ) const { return _positions; }
		unsigned int getPositionsOffset( void ) const { return _positionsOffset; }

		bool hasColors( void ) const { return _colors > 0; }
		unsigned int getColorComponents( void ) const { return _colors; }
		unsigned int getColorsOffset( void ) const { return _colorsOffset; }

		bool hasNormals( void ) const { return _normals > 0; }
		unsigned int getNormalComponents( void ) const { return _normals; }
		unsigned int getNormalsOffset( void ) const { return _normalsOffset; }

		bool hasTextureCoords( void ) const { return _textureCoords > 0; }
		unsigned int getTextureCoordComponents( void ) const { return _textureCoords; }
		unsigned int getTextureCoordsOffset( void ) const { return _textureCoordsOffset; }

		unsigned int getVertexSize( void ) const { return _vertexSize; }

		size_t getVertexSizeInBytes( void ) const { return _vertexSizeInBytes; }

		std::ostream &operator<<( std::ostream &out );

	private:
		unsigned int _positions;
		unsigned int _positionsOffset;
		unsigned int _colors;
		unsigned int _colorsOffset;
		unsigned int _normals;
		unsigned int _normalsOffset;
		unsigned int _textureCoords;
		unsigned int _textureCoordsOffset;
		unsigned int _vertexSize;
		size_t _vertexSizeInBytes;
	};

}

#endif

