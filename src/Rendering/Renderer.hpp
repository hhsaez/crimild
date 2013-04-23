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

namespace Crimild {

	class VisibilitySet;
	class GeometryNode;
	class Primitive;
	class VertexBufferObject;
	class IndexBufferObject;

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
		
		virtual void endRender( void ) = 0;

		virtual void clearBuffers( void ) = 0;

		virtual void render( VisibilitySet *vs );

		virtual void render( GeometryNode *geometry );

		virtual void applyMaterial( GeometryNode *geometry, Primitive *primitive, Material *material );

		virtual void enableShaderProgram( ShaderProgram *program );

		virtual void enableTextures( ShaderProgram *program, Material *material );

		virtual void enableVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo );

		virtual void enableIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo );

		virtual void applyTransformations( ShaderProgram *program, GeometryNode *geometry );

		virtual void drawPrimitive( ShaderProgram *program, Primitive *primitive );

		virtual void restoreTransformations( ShaderProgram *program, GeometryNode *geometry );

		virtual void disableIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo );

		virtual void disableVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo );

		virtual void disableTextures( ShaderProgram *program, Material *material );

		virtual void disableShaderProgram( ShaderProgram *program );

	public:
		Material *getDefaultMaterial( void ) { return _defaultMaterial.get(); }
		void setDefaultMaterial( MaterialPtr material ) { _defaultMaterial = material; }

	public:
		MaterialPtr _defaultMaterial;

	private:
		Renderer( const Renderer &renderer ) { }
		Renderer &operator=( const Renderer & ) { return *this; }
	};

	typedef std::shared_ptr< Renderer > RendererPtr;

}

#endif

