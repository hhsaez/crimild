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

#include "Crimild_Foundation.hpp"
#include "Crimild_Mathematics.hpp"
#include "Primitives/Primitive.hpp"
#include "SceneGraph/Light.hpp"

#include <map>

namespace crimild {

    class AlphaState;
    class Camera;
    class CullFaceState;
    class DepthState;
    class ColorMaskState;
    class FrameBufferObject;
    // class RenderTarget;
    class Geometry;
    class IndexBufferObject;
    class InstancedBufferObject;
    class Material;
    class RenderQueue;
    class ShaderLocation;
    class ShaderProgram;
    class ShaderUniform;
    class Texture;
    class VertexBufferObject;
    class VisibilitySet;

    namespace rendergraph {

        class RenderGraph;

    }

    namespace shadergraph {

        class ShaderGraph;

    }

    class [[deprecated]] Renderer
        : public SharedObject,
          public DynamicSingleton< Renderer > {
    protected:
        Renderer( void );

    public:
        virtual ~Renderer( void );

    public:
        virtual void configure( void );

        virtual crimild::Size getMaxLights( Light::Type lightType ) const { return 10; }

    public:
        /**
            \brief Set the rendering viewport in absolute values
         */
        virtual void setViewport( const Rectf &viewport ) { }

        /**
            \brief Set the rendering viewport relative to the
             screen buffer's resolution
         */
        virtual void setScreenViewport( const Rectf &viewport = Rectf { { 0.0f, 0.0f }, { 1.0f, 1.0f } } );

        virtual void beginRender( void );

        virtual void clearBuffers( void ) = 0;

        virtual void render( RenderQueue *renderQueue, rendergraph::RenderGraph *renderGraph );

        virtual void endRender( void );

        virtual void presentFrame( void );

    public:
        // virtual void bindRenderTarget( RenderTarget *target );
        // virtual void unbindRenderTarget( RenderTarget *target );

        virtual void bindFrameBuffer( FrameBufferObject *fbo );
        virtual void unbindFrameBuffer( FrameBufferObject *fbo );

    public:
        virtual void bindProgram( ShaderProgram *program );
        virtual void unbindProgram( ShaderProgram *program );

        virtual void bindUniform( ShaderLocation *location, bool value ) { bindUniform( location, value ? 1 : 0 ); }
        virtual void bindUniform( ShaderLocation *location, size_t value ) { bindUniform( location, ( int ) value ); }
        virtual void bindUniform( ShaderLocation *location, int value ) = 0;
        virtual void bindUniform( ShaderLocation *location, const Array< crimild::Int32 > &value ) = 0;
        virtual void bindUniform( ShaderLocation *location, float value ) = 0;
        virtual void bindUniform( ShaderLocation *location, const Vector3f &vector ) = 0;
        virtual void bindUniform( ShaderLocation *location, const Vector2f &vector ) = 0;
        virtual void bindUniform( ShaderLocation *location, const ColorRGBA &color ) = 0;
        virtual void bindUniform( ShaderLocation *location, const Matrix4f &matrix ) = 0;
        virtual void bindUniform( ShaderLocation *location, const Matrix3f &matrix ) = 0;

        virtual void bindUniformBlock( ShaderLocation *location, crimild::Int32 blockId ) = 0;

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
        virtual void bindLight( Light *light );
        virtual void unbindLight( Light *light );

    public:
        virtual void bindPrimitive( ShaderProgram *program, Primitive *primitive );
        virtual void unbindPrimitive( ShaderProgram *program, Primitive *primitive );

        virtual void bindVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo );
        virtual void unbindVertexBuffer( ShaderProgram *program, VertexBufferObject *vbo );

        virtual void bindIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo );
        virtual void unbindIndexBuffer( ShaderProgram *program, IndexBufferObject *ibo );

        virtual void bindInstancedBuffer( ShaderProgram *program, InstancedBufferObject *buffer );
        virtual void unbindInstancedBuffer( ShaderProgram *program, InstancedBufferObject *buffer );

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

    private:
        SharedPointer< FrameBufferObject > _screenBuffer;

    public:
        Catalog< ShaderProgram > *getShaderProgramCatalog( void ) { return crimild::get_ptr( _shaderProgramCatalog ); }
        void setShaderProgramCatalog( SharedPointer< Catalog< ShaderProgram > > const &catalog ) { _shaderProgramCatalog = catalog; }

        Catalog< Texture > *getTextureCatalog( void ) { return crimild::get_ptr( _textureCatalog ); }
        void setTextureCatalog( SharedPointer< Catalog< Texture > > const &catalog ) { _textureCatalog = catalog; }

        Catalog< VertexBufferObject > *getVertexBufferObjectCatalog( void ) { return crimild::get_ptr( _vertexBufferObjectCatalog ); }
        void setVertexBufferObjectCatalog( SharedPointer< Catalog< VertexBufferObject > > const &catalog ) { _vertexBufferObjectCatalog = catalog; }

        Catalog< IndexBufferObject > *getIndexBufferObjectCatalog( void ) { return crimild::get_ptr( _indexBufferObjectCatalog ); }
        void setIndexBufferObjectCatalog( SharedPointer< Catalog< IndexBufferObject > > const &catalog ) { _indexBufferObjectCatalog = catalog; }

        Catalog< InstancedBufferObject > *getInstancedBufferObjectCatalog( void ) { return crimild::get_ptr( _instancedBufferObjectCatalog ); }
        void setInstancedBufferObjectCatalog( SharedPointer< Catalog< InstancedBufferObject > > const &catalog ) { _instancedBufferObjectCatalog = catalog; }

        Catalog< FrameBufferObject > *getFrameBufferObjectCatalog( void ) { return crimild::get_ptr( _frameBufferObjectCatalog ); }
        void setFrameBufferObjectCatalog( SharedPointer< Catalog< FrameBufferObject > > const &catalog ) { _frameBufferObjectCatalog = catalog; }

        // Catalog< RenderTarget > *getRenderTargetCatalog( void ) { return crimild::get_ptr( _renderTargetCatalog ); }
        // void setRenderTargetCatalog( SharedPointer< Catalog< RenderTarget > > const &catalog ) { _renderTargetCatalog = catalog; }

        Catalog< Primitive > *getPrimitiveCatalog( void ) { return crimild::get_ptr( _primitiveCatalog ); }
        void setPrimitiveCatalog( SharedPointer< Catalog< Primitive > > const &catalog ) { _primitiveCatalog = catalog; }

        Catalog< Light > *getLightCatalog( void ) { return crimild::get_ptr( _lightCatalog ); }
        void setLightCatalog( SharedPointer< Catalog< Light > > const &catalog ) { _lightCatalog = catalog; }

    private:
        SharedPointer< Catalog< ShaderProgram > > _shaderProgramCatalog;
        SharedPointer< Catalog< Texture > > _textureCatalog;
        SharedPointer< Catalog< VertexBufferObject > > _vertexBufferObjectCatalog;
        SharedPointer< Catalog< IndexBufferObject > > _indexBufferObjectCatalog;
        SharedPointer< Catalog< InstancedBufferObject > > _instancedBufferObjectCatalog;
        SharedPointer< Catalog< FrameBufferObject > > _frameBufferObjectCatalog;
        // SharedPointer< Catalog< RenderTarget > > _renderTargetCatalog;
        SharedPointer< Catalog< Primitive > > _primitiveCatalog;
        SharedPointer< Catalog< Light > > _lightCatalog;

    public:
        virtual SharedPointer< shadergraph::ShaderGraph > createShaderGraph( void ) = 0;
    };

}

#endif
