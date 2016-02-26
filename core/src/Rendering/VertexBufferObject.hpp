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

#ifndef CRIMILD_RENDERING_VERTEX_BUFFER_OBJECT_
#define CRIMILD_RENDERING_VERTEX_BUFFER_OBJECT_

#include "BufferObject.hpp"
#include "VertexFormat.hpp"
#include "Catalog.hpp"
#include "Mathematics/Vector.hpp"

namespace crimild {

	using VertexPrecision = float;

	class VertexBufferObject : 
		public BufferObject< VertexPrecision >, 
		public Catalog< VertexBufferObject >::Resource {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( VertexBufferObject )

	private:
		using Vector2Impl = Vector< 2, VertexPrecision >;
		using Vector3Impl = Vector< 3, VertexPrecision >;
		using RGBAColorImpl = Vector< 4, VertexPrecision >;
		using RGBColorImpl = Vector< 3, VertexPrecision >;
		
	public:
		VertexBufferObject( const VertexFormat &vf, unsigned int vertexCount );

		VertexBufferObject( const VertexFormat &vf, unsigned int vertexCount, const VertexPrecision *vertexData );

		virtual ~VertexBufferObject( void );

		const VertexFormat &getVertexFormat( void ) const { return _vertexFormat; }
		unsigned int getVertexCount( void ) const { return _vertexCount; }

		const Vector3Impl &getPositionAt( unsigned int vIdx ) const
		{
			return *( ( Vector3Impl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getPositionsOffset() ] ) );
		}

		void setPositionAt( unsigned int vIdx, const Vector3Impl &value )
		{
			auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getPositionsOffset() ];
			memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getPositionComponents() );
		}

		RGBAColorImpl getRGBAColorAt( unsigned int vIdx ) const
        {
			return *( ( RGBAColorImpl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getColorsOffset() ] ) );
        }

		void setRGBAColorAt( unsigned int vIdx, const RGBAColorImpl &value )
		{
			auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getColorsOffset() ];
			memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getColorComponents() );
		}

		RGBColorImpl getRGBColorAt( unsigned int vIdx ) const
        {
			return *( ( RGBColorImpl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getColorsOffset() ] ) );
        }

		void setRGBColorAt( unsigned int vIdx, const RGBColorImpl &value )
		{
			auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getColorsOffset() ];
			memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getColorComponents() );
		}

        Vector3Impl getNormalAt( unsigned int vIdx ) const
        {
			return *( ( Vector3Impl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getNormalsOffset() ] ) );
        }

        void setNormalAt( unsigned int vIdx, const Vector3Impl &value )
		{
			auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getNormalsOffset() ];
			memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getNormalComponents() );
		}
        
        Vector3Impl getTangentAt( unsigned int vIdx ) const
        {
			return *( ( Vector3Impl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getTangentsOffset() ] ) );
        }

        void setTangentAt( unsigned int vIdx, const Vector3Impl &value )
		{
			auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getTangentsOffset() ];
			memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getTangentComponents() );
		}
        
        Vector2Impl getTextureCoordAt( unsigned int vIdx ) const
        {
			return *( ( Vector2Impl * ) &( getData()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getTextureCoordsOffset() ] ) );
        }

        void setTextureCoordAt( unsigned int vIdx, const Vector2Impl &value )
		{
			auto d = &data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getTextureCoordsOffset() ];
			memcpy( d, static_cast< const VertexPrecision * >( value ), sizeof( VertexPrecision ) * getVertexFormat().getTextureCoordComponents() );
		}
        
        VertexPrecision getBoneIdAt( unsigned int vIdx, unsigned int bone ) const
        {
        	return getData()[ vIdx + getVertexFormat().getVertexSize() + getVertexFormat().getBoneIdsOffset() + bone ];
        }

        void setBoneIdAt( unsigned int vIdx, unsigned int bone, VertexPrecision value )
		{
			data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getBoneIdsOffset() + bone ] = value;
		}
        
        VertexPrecision getBoneWeightAt( unsigned int vIdx, unsigned int bone ) const
        {
        	return getData()[ vIdx + getVertexFormat().getVertexSize() + getVertexFormat().getBoneWeightsOffset() + bone ];
        }

        void setBoneWeightAt( unsigned int vIdx, unsigned int bone, VertexPrecision value )
		{
			data()[ vIdx * getVertexFormat().getVertexSize() + getVertexFormat().getBoneWeightsOffset() + bone ] = value;
		}
        
	private:
		VertexFormat _vertexFormat;
		unsigned int _vertexCount;
	};

}

#endif

