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
#include "Mathematics/Rect.hpp"

#include <map>

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

        void setScreenBuffer( SharedPointer< FrameBufferObject > const &screenBuffer ) { _screenBuffer = screenBuffer; }
		SharedPointer< FrameBufferObject > &getScreenBuffer( void ) { return _screenBuffer; }

		void addFrameBuffer( std::string name, SharedPointer< FrameBufferObject > const &fbo );
		SharedPointer< FrameBufferObject > getFrameBuffer( std::string name );

	private:
		SharedPointer< FrameBufferObject > _screenBuffer;
		std::map< std::string, SharedPointer< FrameBufferObject >> _framebuffers;

	public:
        virtual void setViewport( const Rectf &viewport ) { }
        
        virtual void beginRender( void );
		
		virtual void clearBuffers( void ) = 0;
        
        virtual void render( SharedPointer< RenderQueue > const &renderQueue, SharedPointer< RenderPass > const &renderPass );

        virtual void endRender( void );

	public:
		virtual void bindFrameBuffer( SharedPointer< FrameBufferObject > const &fbo );
		virtual void unbindFrameBuffer( SharedPointer< FrameBufferObject > const &fbo );

	public:
        virtual void bindProgram( SharedPointer< ShaderProgram > const &program );
		virtual void unbindProgram( SharedPointer< ShaderProgram > const &program );

		virtual void bindUniform( SharedPointer< ShaderLocation > const &location, bool value ) { bindUniform( location, value ? 1 : 0 ); }
		virtual void bindUniform( SharedPointer< ShaderLocation > const &location, int value ) = 0;
		virtual void bindUniform( SharedPointer< ShaderLocation > const &location, float value ) = 0;
		virtual void bindUniform( SharedPointer< ShaderLocation > const &location, const Vector3f &vector ) = 0;
		virtual void bindUniform( SharedPointer< ShaderLocation > const &location, const Vector2f &vector ) = 0;
		virtual void bindUniform( SharedPointer< ShaderLocation > const &location, const RGBAColorf &color ) = 0;
		virtual void bindUniform( SharedPointer< ShaderLocation > const &location, const Matrix4f &matrix ) = 0;

	public:
        virtual void bindMaterial( SharedPointer< ShaderProgram > const &program, SharedPointer< Material > const &material );
        virtual void unbindMaterial( SharedPointer< ShaderProgram > const &program, SharedPointer< Material > const &material );

	public:
        virtual void setDepthState( SharedPointer< DepthState > const &state ) = 0;
        virtual void setAlphaState( SharedPointer< AlphaState > const &state ) = 0;

	public:
        virtual void bindTexture( SharedPointer< ShaderLocation > const &location, SharedPointer< Texture > const &texture );
		virtual void unbindTexture( SharedPointer< ShaderLocation > const &location, SharedPointer< Texture > const &texture );

	public:
		virtual void bindLight( SharedPointer< ShaderProgram > const &program, SharedPointer< Light > const &light );
		virtual void unbindLight( SharedPointer< ShaderProgram > const &program, SharedPointer< Light > const &light );

	private:
		int _lightCount;

	public:
		virtual void bindVertexBuffer( SharedPointer< ShaderProgram > const &program, SharedPointer< VertexBufferObject > const &vbo );
		virtual void unbindVertexBuffer( SharedPointer< ShaderProgram > const &program, SharedPointer< VertexBufferObject > const &vbo );

        virtual void bindIndexBuffer( SharedPointer< ShaderProgram > const &program, SharedPointer< IndexBufferObject > const &ibo );
		virtual void unbindIndexBuffer( SharedPointer< ShaderProgram > const &program, SharedPointer< IndexBufferObject > const &ibo );

	public:
		virtual void applyTransformations( SharedPointer< ShaderProgram > const &program, SharedPointer< Geometry > const &geometry, SharedPointer< Camera > const &camera );
        virtual void applyTransformations( SharedPointer< ShaderProgram > const &program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model, const Matrix4f &normal );
        virtual void applyTransformations( SharedPointer< ShaderProgram > const &program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model );
		virtual void restoreTransformations( SharedPointer< ShaderProgram > const &program, SharedPointer< Geometry > const &geometry, SharedPointer< Camera > const &camera );

	public:
		virtual void drawPrimitive( SharedPointer< ShaderProgram > const &program, SharedPointer< Primitive > const &primitive ) = 0;

		/**
			\brief optional
		 */
		virtual void drawBuffers( SharedPointer< ShaderProgram > const &program, Primitive::Type type, SharedPointer< VertexBufferObject > const &vbo, unsigned int count ) { }

		virtual void drawScreenPrimitive( SharedPointer< ShaderProgram > const &program );

	private:
		PrimitivePtr _screenPrimitive;

	public:
		virtual void addShaderProgram( std::string key, SharedPointer< ShaderProgram > const &program ) { _programs[ key ] = program; }
        virtual SharedPointer< ShaderProgram > getShaderProgram( std::string key ) { return _programs[ key ]; }

        virtual SharedPointer< ShaderProgram > getFallbackProgram( SharedPointer< Material > const &, SharedPointer< Geometry > const &, SharedPointer< Primitive > const & ) { return SharedPointer< ShaderProgram >(); }

	public:
		SharedPointer< Material > _defaultMaterial;
		std::map< std::string, SharedPointer< ShaderProgram >> _programs;

	public:
        SharedPointer< Catalog< ShaderProgram > > &getShaderProgramCatalog( void ) { return _shaderProgramCatalog; }
		void setShaderProgramCatalog( SharedPointer< Catalog< ShaderProgram > > const &catalog ) { _shaderProgramCatalog = catalog; }

		SharedPointer< Catalog< Texture > > &getTextureCatalog( void ) { return _textureCatalog; }
		void setTextureCatalog( SharedPointer< Catalog< Texture > > const &catalog ) { _textureCatalog = catalog; }

		SharedPointer< Catalog< VertexBufferObject > > &getVertexBufferObjectCatalog( void ) { return _vertexBufferObjectCatalog; }
		void setVertexBufferObjectCatalog( SharedPointer< Catalog< VertexBufferObject > > const &catalog ) { _vertexBufferObjectCatalog = catalog; }

		SharedPointer< Catalog< IndexBufferObject > > &getIndexBufferObjectCatalog( void ) { return _indexBufferObjectCatalog; }
		void setIndexBufferObjectCatalog( SharedPointer< Catalog< IndexBufferObject > > const &catalog ) { _indexBufferObjectCatalog = catalog; }

		SharedPointer< Catalog< FrameBufferObject > > &getFrameBufferObjectCatalog( void ) { return _frameBufferObjectCatalog; }
		void setFrameBufferObjectCatalog( SharedPointer< Catalog< FrameBufferObject > > const &catalog ) { _frameBufferObjectCatalog = catalog; }

	private:
		SharedPointer< Catalog< ShaderProgram > > _shaderProgramCatalog;
		SharedPointer< Catalog< Texture > > _textureCatalog;
		SharedPointer< Catalog< VertexBufferObject > > _vertexBufferObjectCatalog;
		SharedPointer< Catalog< IndexBufferObject > > _indexBufferObjectCatalog;
		SharedPointer< Catalog< FrameBufferObject > > _frameBufferObjectCatalog;
	};

    using RendererPtr = SharedPointer< Renderer >;
    
}

#endif

