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

#ifndef CRIMILD_RENDERER_RENDERER_
#define CRIMILD_RENDERER_RENDERER_

#include "FrameBufferObject.hpp"
#include "Material.hpp"
#include "Catalog.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "VertexBufferObject.hpp"
#include "IndexBufferObject.hpp"

#include "Mathematics/Vector.hpp"
#include "Mathematics/Matrix.hpp"

namespace Crimild {

	class VisibilitySet;
	class Geometry;
	class Primitive;
	class Camera;
	class RenderStateComponent;

	class Renderer {
	protected:
		Renderer( void );

	public:
		virtual ~Renderer( void );

	public:
		virtual void configure( void ) = 0;

		void setScreenBuffer( FrameBufferObjectPtr screenBuffer ) { _screenBuffer = screenBuffer; }
		FrameBufferObject *getScreenBuffer( void ) { return _screenBuffer.get(); }

	private:
		FrameBufferObjectPtr _screenBuffer;

	public:
		virtual void beginRender( void ) = 0;
		
		virtual void clearBuffers( void ) = 0;

		virtual void render( VisibilitySet *vs );

		virtual void render( Geometry *geometry, Camera *camera );

		virtual void endRender( void ) = 0;

	public:
		virtual void bindFrameBuffer( FrameBufferObject *fbo );
		virtual void unbindFrameBuffer( FrameBufferObject *fbo );

	public:
		virtual void bindProgram( ShaderProgram *program );
		virtual void unbindProgram( ShaderProgram *program );

		virtual void bindUniform( ShaderLocation *location, int value ) = 0;
		virtual void bindUniform( ShaderLocation *location, float value ) = 0;
		virtual void bindUniform( ShaderLocation *location, const Vector3f &vector ) = 0;
		virtual void bindUniform( ShaderLocation *location, const RGBAColorf &color ) = 0;
		virtual void bindUniform( ShaderLocation *location, const Matrix4f &matrix ) = 0;

	public:
		virtual void bindMaterial( ShaderProgram *program, Material *material );
		virtual void unbindMaterial( ShaderProgram *program, Material *material );

	public:
		virtual void bindTexture( ShaderLocation *location, Texture *texture );
		virtual void unbindTexture( ShaderLocation *location, Texture *texture );

	public:
		virtual void bindLight( ShaderProgram *program, Light *light );
		virtual void unbindLight( ShaderProgram *program, Light *light );

	private:
		int _lightCount;

	public:
		virtual void bindVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo );
		virtual void unbindVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo );
		virtual void bindIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo );
		virtual void unbindIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo );

	public:
		virtual void applyTransformations( ShaderProgram *program, Geometry *geometry, Camera *camera );
		virtual void restoreTransformations( ShaderProgram *program, Geometry *geometry, Camera *camera );

	public:
		virtual void drawPrimitive( ShaderProgram *program, Primitive *primitive ) = 0;

	public:
		virtual ShaderProgram *getFallbackProgram( Material *, Geometry *, Primitive * ) { return nullptr; }

	public:
		MaterialPtr _defaultMaterial;

	public:
		ShaderProgramCatalog *getShaderProgramCatalog( void ) { return _shaderProgramCatalog.get(); }
		void setShaderProgramCatalog( ShaderProgramCatalogPtr catalog ) { _shaderProgramCatalog = catalog; }

		TextureCatalog *getTextureCatalog( void ) { return _textureCatalog.get(); }
		void setTextureCatalog( TextureCatalogPtr catalog ) { _textureCatalog = catalog; }

		VertexBufferObjectCatalog *getVertexBufferObjectCatalog( void ) { return _vertexBufferObjectCatalog.get(); }
		void setVertexBufferObjectCatalog( VertexBufferObjectCatalogPtr catalog ) { _vertexBufferObjectCatalog = catalog; }

		IndexBufferObjectCatalog *getIndexBufferObjectCatalog( void ) { return _indexBufferObjectCatalog.get(); }
		void setIndexBufferObjectCatalog( IndexBufferObjectCatalogPtr catalog ) { _indexBufferObjectCatalog = catalog; }

		FrameBufferObjectCatalog *getFrameBufferObjectCatalog( void ) { return _frameBufferObjectCatalog.get(); }
		void setFrameBufferObjectCatalog( FrameBufferObjectCatalogPtr catalog ) { _frameBufferObjectCatalog = catalog; }

	private:
		ShaderProgramCatalogPtr _shaderProgramCatalog;
		TextureCatalogPtr _textureCatalog;
		VertexBufferObjectCatalogPtr _vertexBufferObjectCatalog;
		IndexBufferObjectCatalogPtr _indexBufferObjectCatalog;
		FrameBufferObjectCatalogPtr _frameBufferObjectCatalog;

	private:
		Renderer( const Renderer &renderer ) { }
		Renderer &operator=( const Renderer & ) { return *this; }
	};

	typedef std::shared_ptr< Renderer > RendererPtr;

}

#endif

