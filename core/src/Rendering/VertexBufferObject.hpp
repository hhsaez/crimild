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

	class VertexBufferObject : public BufferObject< float >, public Catalog< VertexBufferObject >::Resource {
	public:
		VertexBufferObject( const VertexFormat &vf, unsigned int vertexCount, const float *vertexData );
		virtual ~VertexBufferObject( void );

		const VertexFormat &getVertexFormat( void ) const { return _vertexFormat; }
		unsigned int getVertexCount( void ) const { return _vertexCount; }

		Vector3f getPositionAt( unsigned int index ) const;
		RGBAColorf getRGBAColorAt( unsigned int index ) const;
		RGBColorf getRGBColorAt( unsigned int index ) const;
		Vector3f getNormalAt( unsigned int index ) const;
		Vector3f getTangentAt( unsigned int index ) const; 
		Vector2f getTextureCoordAt( unsigned int index ) const;

	private:
		VertexFormat _vertexFormat;
		unsigned int _vertexCount;
	};

	typedef std::shared_ptr< VertexBufferObject > VertexBufferObjectPtr;
	typedef Catalog< VertexBufferObject > VertexBufferObjectCatalog;
	typedef std::shared_ptr< VertexBufferObjectCatalog > VertexBufferObjectCatalogPtr;

}

#endif

