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
#include "Foundation/Singleton.hpp"

#include "Primitives/Primitive.hpp"

#include "Mathematics/Vector.hpp"
#include "Mathematics/Matrix.hpp"
#include "Mathematics/Rect.hpp"

#include <map>

namespace crimild {
    
    class AlphaState;
    class Camera;
    class CullFaceState;
    class DepthState;
    class ColorMaskState;
    class FrameBufferObject;
	class RenderTarget;
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

	namespace shadergraph {

		class ShaderGraph;

	}

	class Renderer : 
		public SharedObject,
		public DynamicSingleton< Renderer > {
	protected:
		Renderer( void );

	public:
		virtual ~Renderer( void );

	public:
		virtual void configure( void ) = 0;

	public:
        virtual void setViewport( const Rectf &viewport ) { }
        
        virtual void beginRender( void );
		
		virtual void clearBuffers( void ) = 0;
        
        virtual void render( RenderQueue *renderQueue, RenderPass *renderPass );

        virtual void endRender( void );
            
        virtual void presentFrame( void );

	public:
		virtual void bindRenderTarget( RenderTarget *target );
		virtual void unbindRenderTarget( RenderTarget *target );
		
		virtual void bindFrameBuffer( FrameBufferObject *fbo );
		virtual void unbindFrameBuffer( FrameBufferObject *fbo );

	public:
        virtual void bindProgram( ShaderProgram *program );
		virtual void unbindProgram( ShaderProgram *program );

		virtual void bindUniform( ShaderLocation *location, bool value ) { bindUniform( location, value ? 1 : 0 ); }
		virtual void bindUniform( ShaderLocation *location, size_t value ) { bindUniform( location, ( int ) value ); }
		virtual void bindUniform( ShaderLocation *location, int value ) = 0;
		virtual void bindUniform( ShaderLocation *location, float value ) = 0;
		virtual void bindUniform( ShaderLocation *location, const Vector3f &vector ) = 0;
		virtual void bindUniform( ShaderLocation *location, const Vector2f &vector ) = 0;
		virtual void bindUniform( ShaderLocation *location, const RGBAColorf &color ) = 0;
		virtual void bindUniform( ShaderLocation *location, const Matrix4f &matrix ) = 0;

	public:
        virtual void bindMaterial( ShaderProgram *program, Material *material );
        virtual void unbindMaterial( ShaderProgram *program, Material *material );

	public:
        void setDepthState( SharedPointer< DepthState > const &state ) { setDepthState( crimild::get_ptr( state ) ); }
        virtual void setDepthState( DepthState *state ) = 0;

        void setAlphaState( SharedPointer< AlphaState > const &state ) { setAlphaState( crimild::get_ptr( state ) ); }
        virtual void setAlphaState( AlphaState *state ) = 0;

        void setCullFaceState( SharedPointer< CullFaceState > const &state ) { setCullFaceState( crimild::get_ptr( state ) ); }
        virtual void setCullFaceState( CullFaceState *state ) = 0;

        void setColorMaskState( SharedPointer< ColorMaskState > const &state ) { setColorMaskState( crimild::get_ptr( state ) ); }
        virtual void setColorMaskState( ColorMaskState *state ) = 0;

	public:
        virtual void bindTexture( ShaderLocation *location, Texture *texture );
		virtual void unbindTexture( ShaderLocation *location, Texture *texture );

	public:
		virtual void bindLight( ShaderProgram *program, Light *light );
		virtual void unbindLight( ShaderProgram *program, Light *light );

	private:
		int _lightCount;

	public:
		virtual void bindPrimitive( ShaderProgram *program, Primitive *primitive );
		virtual void unbindPrimitive( ShaderProgram *program, Primitive *primitive );
		
		virtual void bindVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo );
		virtual void unbindVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo );

        virtual void bindIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo );
		virtual void unbindIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo );

	public:
		virtual void applyTransformations( ShaderProgram *program, Geometry *geometry, Camera *camera );
        virtual void applyTransformations( ShaderProgram *program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model, const Matrix4f &normal );
        virtual void applyTransformations( ShaderProgram *program, const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model );
		virtual void restoreTransformations( ShaderProgram *program, Geometry *geometry, Camera *camera );

	public:
		virtual void drawPrimitive( ShaderProgram *program, Primitive *primitive ) = 0;

		/**
			\brief optional
		 */
		virtual void drawBuffers( ShaderProgram *program, Primitive::Type type, VertexBufferObject *vbo, unsigned int count ) { }

		virtual void drawGeometry( Geometry *geometry, ShaderProgram *program, const Matrix4f &modelMatrix );

		virtual void drawScreenPrimitive( ShaderProgram *program );

	private:
		SharedPointer< Primitive > _screenPrimitive;
        
    public:
        void setScreenBuffer( SharedPointer< FrameBufferObject > const &screenBuffer ) { _screenBuffer = screenBuffer; }
        FrameBufferObject *getScreenBuffer( void ) { return crimild::get_ptr( _screenBuffer ); }
        
        static constexpr const char *FBO_AUX_256 = "framebuffers/aux_256";
        static constexpr const char *FBO_AUX_512 = "framebuffers/aux_512";
        static constexpr const char *FBO_AUX_1024 = "framebuffers/aux_1024";
        static constexpr const char *FBO_AUX_COLOR_TARGET_NAME = "framebuffers/aux_color";
        static constexpr const char *FBO_AUX_DEPTH_TARGET_NAME = "framebuffers/aux_depth";

        void setFrameBuffer( std::string name, SharedPointer< FrameBufferObject > const &fbo );
        FrameBufferObject *getFrameBuffer( std::string name );

        SharedPointer< FrameBufferObject > generateAuxFBO( std::string name, int width, int height );
        
    private:
        SharedPointer< FrameBufferObject > _screenBuffer;
        
    public:
        static constexpr const char *SHADER_PROGRAM_RENDER_PASS_DEPTH = "shaders/render_pass/depth";
        static constexpr const char *SHADER_PROGRAM_RENDER_PASS_FORWARD_LIGHTING = "shaders/render_pass/forward_lighting";
        static constexpr const char *SHADER_PROGRAM_RENDER_PASS_SCREEN = "shaders/render_pass/screen";
		
        static constexpr const char *SHADER_PROGRAM_RENDER_PASS_STANDARD = "shaders/render_pass/standard";
        static constexpr const char *SHADER_PROGRAM_LIT_TEXTURE = "shaders/lighting/texture";
        static constexpr const char *SHADER_PROGRAM_LIT_DIFFUSE = "shaders/lighting/diffuse";
        static constexpr const char *SHADER_PROGRAM_UNLIT_TEXTURE = "shaders/unlit/texture";
        static constexpr const char *SHADER_PROGRAM_UNLIT_DIFFUSE = "shaders/unlit/diffuse";
        static constexpr const char *SHADER_PROGRAM_UNLIT_VERTEX_COLOR = "shaders/unlit/vertex_color";
        static constexpr const char *SHADER_PROGRAM_PARTICLE_SYSTEM = "shaders/unlit/particle_system";
        static constexpr const char *SHADER_PROGRAM_TEXT_BASIC = "shaders/text/basic";
        static constexpr const char *SHADER_PROGRAM_TEXT_SDF = "shaders/text/sdf";
        static constexpr const char *SHADER_PROGRAM_SCREEN_TEXTURE = "shaders/misc/screen";
		static constexpr const char *SHADER_PROGRAM_DEPTH = "shaders/misc/depth";
		static constexpr const char *SHADER_PROGRAM_POINT_SPRITE = "shaders/unlit/point_sprit";
		static constexpr const char *SHADER_PROGRAM_DEBUG_DEPTH = "shaders/debug/depth";
        
        void setShaderProgram( std::string name, SharedPointer< ShaderProgram > const &program );
        ShaderProgram *getShaderProgram( std::string name );

	public:
        Catalog< ShaderProgram > *getShaderProgramCatalog( void ) { return crimild::get_ptr( _shaderProgramCatalog ); }
		void setShaderProgramCatalog( SharedPointer< Catalog< ShaderProgram > > const &catalog ) { _shaderProgramCatalog = catalog; }

		Catalog< Texture > *getTextureCatalog( void ) { return crimild::get_ptr( _textureCatalog ); }
		void setTextureCatalog( SharedPointer< Catalog< Texture > > const &catalog ) { _textureCatalog = catalog; }

		Catalog< VertexBufferObject > *getVertexBufferObjectCatalog( void ) { return crimild::get_ptr( _vertexBufferObjectCatalog ); }
		void setVertexBufferObjectCatalog( SharedPointer< Catalog< VertexBufferObject > > const &catalog ) { _vertexBufferObjectCatalog = catalog; }

		Catalog< IndexBufferObject > *getIndexBufferObjectCatalog( void ) { return crimild::get_ptr( _indexBufferObjectCatalog ); }
		void setIndexBufferObjectCatalog( SharedPointer< Catalog< IndexBufferObject > > const &catalog ) { _indexBufferObjectCatalog = catalog; }

		Catalog< FrameBufferObject > *getFrameBufferObjectCatalog( void ) { return crimild::get_ptr( _frameBufferObjectCatalog ); }
		void setFrameBufferObjectCatalog( SharedPointer< Catalog< FrameBufferObject > > const &catalog ) { _frameBufferObjectCatalog = catalog; }

		Catalog< RenderTarget > *getRenderTargetCatalog( void ) { return crimild::get_ptr( _renderTargetCatalog ); }
		void setRenderTargetCatalog( SharedPointer< Catalog< RenderTarget > > const &catalog ) { _renderTargetCatalog = catalog; }

		Catalog< Primitive > *getPrimitiveCatalog( void ) { return crimild::get_ptr( _primitiveCatalog ); }
		void setPrimitiveCatalog( SharedPointer< Catalog< Primitive >> const &catalog ) { _primitiveCatalog = catalog; }

	private:
		SharedPointer< Catalog< ShaderProgram >> _shaderProgramCatalog;
		SharedPointer< Catalog< Texture >> _textureCatalog;
		SharedPointer< Catalog< VertexBufferObject >> _vertexBufferObjectCatalog;
		SharedPointer< Catalog< IndexBufferObject >> _indexBufferObjectCatalog;
		SharedPointer< Catalog< FrameBufferObject >> _frameBufferObjectCatalog;
		SharedPointer< Catalog< RenderTarget >> _renderTargetCatalog;
		SharedPointer< Catalog< Primitive >> _primitiveCatalog;

	public:
		virtual SharedPointer< shadergraph::ShaderGraph > createShaderGraph( void ) = 0;

	public:
		/**
		   A 1x1 white texture used when an invalid texture pointer is provided. This
		   is faster than querying if a texture was actually bound to shaders
		 */
		Texture *getFallbackTexture( void ) { return crimild::get_ptr( _fallbackTexture ); }

	private:
		SharedPointer< Texture > _fallbackTexture;
	};
    
}

#endif

