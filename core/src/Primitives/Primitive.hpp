/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holders nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_PRIMITIVES_PRIMITIVE_
#define CRIMILD_PRIMITIVES_PRIMITIVE_

#include "Coding/Codable.hpp"
#include "Streaming/Stream.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/IndexBufferObject.hpp"
#include "Rendering/InstancedBufferObject.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "Rendering/VertexBufferObject.hpp"

#include <functional>

namespace crimild {

    class Primitive :
		public coding::Codable,
		public Catalog< Primitive >::Resource,
		public StreamObject {
		CRIMILD_IMPLEMENT_RTTI( crimild::Primitive )

	public:
		enum class Type : uint8_t {
			POINTS,
			LINES,
			LINE_LOOP,
			LINE_STRIP,
			TRIANGLES,
			TRIANGLE_STRIP,
			TRIANGLE_FAN
		};

	public:
		explicit Primitive( Primitive::Type type = Primitive::Type::TRIANGLES );
		virtual ~Primitive( void );

		Primitive::Type getType( void ) const { return _type; }

		containers::Array< SharedPointer< VertexBuffer >> vertexBuffers;
		SharedPointer< IndexBuffer > indexBuffer;

        //void setVertices( SharedPointer< VertexBuffer > &vertices ) noexcept { setVertices( { vertexData } ); }
        //void setVertices( containers::Array< SharedPointer< VertexBuffer >> const &vertices ) noexcept { _vertices; }

        //void setIndices( SharedPointer< IndexBuffer > const &indexData ) noexcept { _indexData = indexData; }

        void setVertexBuffer( VertexBufferObject *vbo ) { _vertexBuffer = crimild::retain( vbo ) ; }
        void setVertexBuffer( SharedPointer< VertexBufferObject > const &vbo ) { _vertexBuffer = vbo; }
        VertexBufferObject *getVertexBuffer( void ) { return crimild::get_ptr( _vertexBuffer ); }

        void setIndexBuffer( IndexBufferObject *ibo ) { _indexBuffer = crimild::retain( ibo ) ; }
        void setIndexBuffer( SharedPointer< IndexBufferObject > const &ibo ) { _indexBuffer = ibo; }
        IndexBufferObject *getIndexBuffer( void ) { return crimild::get_ptr( _indexBuffer ); }

		void setInstancedBuffer( InstancedBufferObject *buffer ) { _instancedBuffer = crimild::retain( buffer ); }
		void setInstancedBuffer( SharedPointer< InstancedBufferObject > const &buffer ) { _instancedBuffer = buffer; }
		InstancedBufferObject *getInstancedBuffer( void ) { return crimild::get_ptr( _instancedBuffer ); }

	private:
		Primitive::Type _type;
		SharedPointer< VertexBufferObject > _vertexBuffer;
		SharedPointer< IndexBufferObject > _indexBuffer;
		SharedPointer< InstancedBufferObject > _instancedBuffer;
        
        /**
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
		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;

		//@}
	};

	using PrimitivePtr = SharedPointer< Primitive >;

}

#endif

