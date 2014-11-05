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

#ifndef CRIMILD_RENDERING_RENDERER_
#define CRIMILD_RENDERING_RENDERER_

#include "Foundation/SharedObject.hpp"

#include "FrameBufferObject.hpp"
#include "Material.hpp"
#include "Catalog.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "VertexBufferObject.hpp"
#include "IndexBufferObject.hpp"

#include "primitives/Primitive.hpp"

#include "Mathematics/Vector.hpp"
#include "Mathematics/Matrix.hpp"

namespace crimild {

	class VisibilitySet;
	class Geometry;
	class Camera;
	class RenderStateComponent;
    class RenderQueue;
    class RenderPass;

	class Renderer : public SharedObject {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( Renderer );

	protected:
		Renderer( void );

	public:
		virtual ~Renderer( void );

	public:
		virtual void configure( void ) = 0;

		void setScreenBuffer( FrameBufferObject *screenBuffer ) { _screenBuffer = screenBuffer; }
		FrameBufferObject *getScreenBuffer( void ) { return _screenBuffer.get(); }

	private:
		Pointer< FrameBufferObject > _screenBuffer;

	public:
		virtual void beginRender( void ) = 0;
		
		virtual void clearBuffers( void ) = 0;
        
        virtual void render( RenderQueue *renderQueue, RenderPass *renderPass = nullptr );

		virtual void render( VisibilitySet *vs, RenderPass *renderPass = nullptr );

		virtual void render( Geometry *geometry, Camera *camera, RenderPass *renderPass = nullptr );

		virtual void endRender( void ) = 0;

	public:
		virtual void bindFrameBuffer( FrameBufferObject *fbo );
		virtual void unbindFrameBuffer( FrameBufferObject *fbo );

	public:
		virtual void bindProgram( ShaderProgram *program );
		virtual void unbindProgram( ShaderProgram *program );

		virtual void bindUniform( ShaderLocation *location, bool value ) { bindUniform( location, value ? 1 : 0 ); }
		virtual void bindUniform( ShaderLocation *location, int value ) = 0;
		virtual void bindUniform( ShaderLocation *location, float value ) = 0;
		virtual void bindUniform( ShaderLocation *location, const Vector3f &vector ) = 0;
		virtual void bindUniform( ShaderLocation *location, const RGBAColorf &color ) = 0;
		virtual void bindUniform( ShaderLocation *location, const Matrix4f &matrix ) = 0;

	public:
		virtual void bindMaterial( ShaderProgram *program, Material *material );
		virtual void unbindMaterial( ShaderProgram *program, Material *material );

	public:
		virtual void setDepthState( DepthState *state ) = 0;
		virtual void setAlphaState( AlphaState *state ) = 0;

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
        virtual void applyTransformations( ShaderProgram *program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model, const Matrix4f &normal );
		virtual void restoreTransformations( ShaderProgram *program, Geometry *geometry, Camera *camera );

	public:
		virtual void drawPrimitive( ShaderProgram *program, Primitive *primitive ) = 0;

		/**
			\brief optional
		 */
		virtual void drawBuffers( ShaderProgram *program, Primitive::Type type, VertexBufferObject *vbo, unsigned int count ) { }

	public:
        virtual ShaderProgram *getDepthProgram( void ) { return nullptr; }
        virtual ShaderProgram *getForwardPassProgram( void ) { return nullptr; }
        virtual ShaderProgram *getDeferredPassProgram( void ) { return nullptr; }
        virtual ShaderProgram *getShaderProgram( const char *name ) { return nullptr; }
		virtual ShaderProgram *getFallbackProgram( Material *, Geometry *, Primitive * ) { return nullptr; }

	public:
		Pointer< Material > _defaultMaterial;

	public:
		ShaderProgramCatalog *getShaderProgramCatalog( void ) { return _shaderProgramCatalog.get(); }
		void setShaderProgramCatalog( ShaderProgramCatalog *catalog ) { _shaderProgramCatalog = catalog; }

		TextureCatalog *getTextureCatalog( void ) { return _textureCatalog.get(); }
		void setTextureCatalog( TextureCatalog *catalog ) { _textureCatalog = catalog; }

		VertexBufferObjectCatalog *getVertexBufferObjectCatalog( void ) { return _vertexBufferObjectCatalog.get(); }
		void setVertexBufferObjectCatalog( VertexBufferObjectCatalog *catalog ) { _vertexBufferObjectCatalog = catalog; }

		IndexBufferObjectCatalog *getIndexBufferObjectCatalog( void ) { return _indexBufferObjectCatalog.get(); }
		void setIndexBufferObjectCatalog( IndexBufferObjectCatalog *catalog ) { _indexBufferObjectCatalog = catalog; }

		FrameBufferObjectCatalog *getFrameBufferObjectCatalog( void ) { return _frameBufferObjectCatalog.get(); }
		void setFrameBufferObjectCatalog( FrameBufferObjectCatalog *catalog ) { _frameBufferObjectCatalog = catalog; }

	private:
		Pointer< ShaderProgramCatalog > _shaderProgramCatalog;
		Pointer< TextureCatalog > _textureCatalog;
		Pointer< VertexBufferObjectCatalog > _vertexBufferObjectCatalog;
		Pointer< IndexBufferObjectCatalog > _indexBufferObjectCatalog;
		Pointer< FrameBufferObjectCatalog > _frameBufferObjectCatalog;
	};

}

#endif

