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
		CRIMILD_IMPLEMENT_RTTI( crimild::VertexBufferObject )

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

		const Vector3Impl &getPositionAt( unsigned int vIdx ) const;
		void setPositionAt( unsigned int vIdx, const Vector3Impl &value );

		RGBAColorImpl getRGBAColorAt( unsigned int vIdx ) const;
		void setRGBAColorAt( unsigned int vIdx, const RGBAColorImpl &value );

		RGBColorImpl getRGBColorAt( unsigned int vIdx ) const;
		void setRGBColorAt( unsigned int vIdx, const RGBColorImpl &value );

        Vector3Impl getNormalAt( unsigned int vIdx ) const;
        void setNormalAt( unsigned int vIdx, const Vector3Impl &value );
        
        Vector3Impl getTangentAt( unsigned int vIdx ) const;
        void setTangentAt( unsigned int vIdx, const Vector3Impl &value );
        
        Vector2Impl getTextureCoordAt( unsigned int vIdx ) const;
        void setTextureCoordAt( unsigned int vIdx, const Vector2Impl &value );
        
        VertexPrecision getBoneIdAt( unsigned int vIdx, unsigned int bone ) const;
        void setBoneIdAt( unsigned int vIdx, unsigned int bone, VertexPrecision value );

        VertexPrecision getBoneWeightAt( unsigned int vIdx, unsigned int bone ) const;
        void setBoneWeightAt( unsigned int vIdx, unsigned int bone, VertexPrecision value );
        
	private:
		VertexFormat _vertexFormat;
        crimild::UInt32 _vertexCount;
            
        /**
            \name Coding
         */
        //@{
            
    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;
            
        //@}

        /**
        	\name Streaming
             \deprecated See crimild::coding
        */
        //@{

	public:
    	/**
    		\brief Default constructor

    		\warning Used only for streaming purposes
    	*/
		VertexBufferObject( void );

		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;

		//@}
	};

	using VertexBufferObjectPtr = SharedPointer< VertexBufferObject >;

}

#endif

