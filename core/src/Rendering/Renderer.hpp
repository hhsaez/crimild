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

#include "Primitives/Primitive.hpp"

#include "Mathematics/Vector.hpp"
#include "Mathematics/Matrix.hpp"

namespace crimild {
    
    class AlphaState;
    class Camera;
    class DepthState;
    class FrameBufferObject;
    class Geometry;
    class IndexBufferObject;
    class Light;
    class Material;
    class RenderPass;
    class RenderQueue;
    class ShaderLocation;
    class ShaderProgram;
    class ShaderUniform;
    class Texture;
    class VertexBufferObject;
    class VisibilitySet;

	class Renderer : public SharedObject {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( Renderer );
        
	protected:
		Renderer( void );

	public:
		virtual ~Renderer( void );

	public:
		virtual void configure( void ) = 0;

        void setScreenBuffer( std::shared_ptr< FrameBufferObject > const &screenBuffer ) { _screenBuffer = screenBuffer; }
		std::shared_ptr< FrameBufferObject > &getScreenBuffer( void ) { return _screenBuffer; }

	private:
		std::shared_ptr< FrameBufferObject > _screenBuffer;

	public:
        virtual void beginRender( void );
		
		virtual void clearBuffers( void ) = 0;
        
        virtual void render( std::shared_ptr< RenderQueue > const &renderQueue, std::shared_ptr< RenderPass > const &renderPass );

        virtual void render( std::shared_ptr< VisibilitySet > const &vs, std::shared_ptr< RenderPass > const &renderPass );

        virtual void render( std::shared_ptr< Geometry > const &geometry, std::shared_ptr< Camera > const &camera, std::shared_ptr< RenderPass > const &renderPass );

        virtual void endRender( void );

	public:
		virtual void bindFrameBuffer( std::shared_ptr< FrameBufferObject > const &fbo );
		virtual void unbindFrameBuffer( std::shared_ptr< FrameBufferObject > const &fbo );

	public:
        virtual void bindProgram( std::shared_ptr< ShaderProgram > const &program );
		virtual void unbindProgram( std::shared_ptr< ShaderProgram > const &program );

		virtual void bindUniform( std::shared_ptr< ShaderLocation > const &location, bool value ) { bindUniform( location, value ? 1 : 0 ); }
		virtual void bindUniform( std::shared_ptr< ShaderLocation > const &location, int value ) = 0;
		virtual void bindUniform( std::shared_ptr< ShaderLocation > const &location, float value ) = 0;
		virtual void bindUniform( std::shared_ptr< ShaderLocation > const &location, const Vector3f &vector ) = 0;
		virtual void bindUniform( std::shared_ptr< ShaderLocation > const &location, const RGBAColorf &color ) = 0;
		virtual void bindUniform( std::shared_ptr< ShaderLocation > const &location, const Matrix4f &matrix ) = 0;

	public:
        virtual void bindMaterial( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< Material > const &material );
        virtual void unbindMaterial( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< Material > const &material );

	public:
        virtual void setDepthState( std::shared_ptr< DepthState > const &state ) = 0;
        virtual void setAlphaState( std::shared_ptr< AlphaState > const &state ) = 0;

	public:
        virtual void bindTexture( std::shared_ptr< ShaderLocation > const &location, std::shared_ptr< Texture > const &texture );
		virtual void unbindTexture( std::shared_ptr< ShaderLocation > const &location, std::shared_ptr< Texture > const &texture );

	public:
		virtual void bindLight( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< Light > const &light );
		virtual void unbindLight( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< Light > const &light );

	private:
		int _lightCount;

	public:
		virtual void bindVertexBuffer( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< VertexBufferObject > const &vbo );
		virtual void unbindVertexBuffer( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< VertexBufferObject > const &vbo );

        virtual void bindIndexBuffer( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< IndexBufferObject > const &ibo );
		virtual void unbindIndexBuffer( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< IndexBufferObject > const &ibo );

	public:
		virtual void applyTransformations( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< Geometry > const &geometry, std::shared_ptr< Camera > const &camera );
        virtual void applyTransformations( std::shared_ptr< ShaderProgram > const &program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model, const Matrix4f &normal );
		virtual void restoreTransformations( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< Geometry > const &geometry, std::shared_ptr< Camera > const &camera );

	public:
		virtual void drawPrimitive( std::shared_ptr< ShaderProgram > const &program, std::shared_ptr< Primitive > const &primitive ) = 0;

		/**
			\brief optional
		 */
		virtual void drawBuffers( std::shared_ptr< ShaderProgram > const &program, Primitive::Type type, std::shared_ptr< VertexBufferObject > const &vbo, unsigned int count ) { }

	public:
        virtual std::shared_ptr< ShaderProgram > getDepthProgram( void ) { return std::shared_ptr< ShaderProgram >(); }
        virtual std::shared_ptr< ShaderProgram > getForwardPassProgram( void ) { return std::shared_ptr< ShaderProgram >(); }
        virtual std::shared_ptr< ShaderProgram > getDeferredPassProgram( void ) { return std::shared_ptr< ShaderProgram >(); }
        virtual std::shared_ptr< ShaderProgram > getShaderProgram( const char *name ) { return std::shared_ptr< ShaderProgram >(); }
        virtual std::shared_ptr< ShaderProgram > getFallbackProgram( std::shared_ptr< Material > const &, std::shared_ptr< Geometry > const &, std::shared_ptr< Primitive > const & ) { return std::shared_ptr< ShaderProgram >(); }

	public:
		std::shared_ptr< Material > _defaultMaterial;

	public:
        std::shared_ptr< Catalog< ShaderProgram > > &getShaderProgramCatalog( void ) { return _shaderProgramCatalog; }
		void setShaderProgramCatalog( std::shared_ptr< Catalog< ShaderProgram > > const &catalog ) { _shaderProgramCatalog = catalog; }

		std::shared_ptr< Catalog< Texture > > &getTextureCatalog( void ) { return _textureCatalog; }
		void setTextureCatalog( std::shared_ptr< Catalog< Texture > > const &catalog ) { _textureCatalog = catalog; }

		std::shared_ptr< Catalog< VertexBufferObject > > &getVertexBufferObjectCatalog( void ) { return _vertexBufferObjectCatalog; }
		void setVertexBufferObjectCatalog( std::shared_ptr< Catalog< VertexBufferObject > > const &catalog ) { _vertexBufferObjectCatalog = catalog; }

		std::shared_ptr< Catalog< IndexBufferObject > > &getIndexBufferObjectCatalog( void ) { return _indexBufferObjectCatalog; }
		void setIndexBufferObjectCatalog( std::shared_ptr< Catalog< IndexBufferObject > > const &catalog ) { _indexBufferObjectCatalog = catalog; }

		std::shared_ptr< Catalog< FrameBufferObject > > &getFrameBufferObjectCatalog( void ) { return _frameBufferObjectCatalog; }
		void setFrameBufferObjectCatalog( std::shared_ptr< Catalog< FrameBufferObject > > const &catalog ) { _frameBufferObjectCatalog = catalog; }

	private:
		std::shared_ptr< Catalog< ShaderProgram > > _shaderProgramCatalog;
		std::shared_ptr< Catalog< Texture > > _textureCatalog;
		std::shared_ptr< Catalog< VertexBufferObject > > _vertexBufferObjectCatalog;
		std::shared_ptr< Catalog< IndexBufferObject > > _indexBufferObjectCatalog;
		std::shared_ptr< Catalog< FrameBufferObject > > _frameBufferObjectCatalog;
	};

    using RendererPtr = std::shared_ptr< Renderer >;
    
}

#endif

