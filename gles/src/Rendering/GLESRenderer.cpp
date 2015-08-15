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

#include "GLESRenderer.hpp"
#include "Utils.hpp"

#include "Catalogs/ShaderProgramCatalog.hpp"
#include "Catalogs/TextureCatalog.hpp"
#include "Catalogs/IndexBufferObjectCatalog.hpp"
#include "Catalogs/VertexBufferObjectCatalog.hpp"
#include "Catalogs/FrameBufferObjectCatalog.hpp"

#include "Programs/SimpleShaderProgram.hpp"
#include "Programs/DiffuseShaderProgram.hpp"
#include "Programs/TextureShaderProgram.hpp"
#include "Programs/SDFShaderProgram.hpp"
#include "Programs/ForwardRenderShaderProgram.hpp"

#ifdef __APPLE__
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

using namespace crimild;
using namespace crimild::gles;

GLESRenderer::GLESRenderer( void )
    : gles::GLESRenderer( nullptr )
{
    
}

GLESRenderer::GLESRenderer( FrameBufferObjectPtr const &screenBuffer )
{
    setShaderProgramCatalog( crimild::alloc< gles::ShaderProgramCatalog >() );
    setVertexBufferObjectCatalog( crimild::alloc< gles::VertexBufferObjectCatalog >() );
    setIndexBufferObjectCatalog( crimild::alloc< gles::IndexBufferObjectCatalog >() );
    setFrameBufferObjectCatalog( crimild::alloc< gles::FrameBufferObjectCatalog >( this ) );
    setTextureCatalog( crimild::alloc< gles::TextureCatalog >() );
    
    addShaderProgram( "fallback", crimild::alloc< SimpleShaderProgram >() );
//    addShaderProgram( "flat", crimild::alloc< FlatShaderProgram >() );
//    addShaderProgram( "gouraud", crimild::alloc< GouraudShaderProgram >() );
    addShaderProgram( "phong", crimild::alloc< DiffuseShaderProgram >() );
//    addShaderProgram( "phong", crimild::alloc< PhongShaderProgram >() );
    addShaderProgram( "color", crimild::alloc< DiffuseShaderProgram >() );
//    addShaderProgram( "screen", crimild::alloc< ScreenShaderProgram >() );
//    addShaderProgram( "screen_alpha", crimild::alloc< ScreenAlphaShaderProgram >() );
//    addShaderProgram( "screen_rgb", crimild::alloc< ScreenRGBShaderProgram >() );
    addShaderProgram( "texture", crimild::alloc< TextureShaderProgram >() );
//    addShaderProgram( "depth", crimild::alloc< DepthShaderProgram >() );
//    addShaderProgram( "basic", crimild::alloc< BasicRenderShaderProgram >() );
    addShaderProgram( "forward", crimild::alloc< ForwardRenderShaderProgram >() );
//    addShaderProgram( "deferred", crimild::alloc< DeferredRenderShaderProgram >() );
//    addShaderProgram( "deferred_compose", crimild::alloc< DeferredComposeRenderShaderProgram >() );
//    addShaderProgram( "blend", crimild::alloc< BlendShaderProgram >() );
//    addShaderProgram( "blur", crimild::alloc< BlurShaderProgram >() );
//    addShaderProgram( "gaussianBlur", crimild::alloc< GaussianBlurShaderProgram >() );
    addShaderProgram( "sdf", crimild::alloc< SignedDistanceFieldShaderProgram >() );
    addShaderProgram( "text", crimild::alloc< TextureShaderProgram >() );
//    addShaderProgram( "sdf", crimild::alloc< SignedDistanceFieldShaderProgram >() );
//    addShaderProgram( "text", crimild::alloc< SignedDistanceFieldShaderProgram >() );
    
    // overrides
    addShaderProgram( "basic", crimild::alloc< DiffuseShaderProgram >() );
//    addShaderProgram( "forward", crimild::alloc< TextureShaderProgram >() );

    AssetManager::getInstance()->add( AssetManager::SHADER_PROGRAM_UNLIT_TEXTURE, crimild::alloc< TextureShaderProgram >(), true );
    AssetManager::getInstance()->add( AssetManager::SHADER_PROGRAM_UNLIT_DIFFUSE, crimild::alloc< DiffuseShaderProgram >(), true );
    
    setScreenBuffer( screenBuffer );
}

GLESRenderer::~GLESRenderer( void )
{
    
}

void GLESRenderer::configure( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    Log::Debug << "Configuring renderer"
    << "\n       OpenGL version: " << glGetString( GL_VERSION )
    << "\n       GLSL version: " << glGetString( GL_SHADING_LANGUAGE_VERSION )
    << "\n       Vendor: " << glGetString( GL_VENDOR )
    << "\n       Renderer: " << glGetString( GL_RENDERER )
    << Log::End;
    
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
//    glEnable( GL_PROGRAM_POINT_SIZE );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GLESRenderer::setViewport( const Rectf &viewport )
{
    auto screen = getScreenBuffer();
    glViewport(
               screen->getWidth() * viewport.getX(),
               screen->getHeight() * viewport.getY(),
               screen->getWidth() * viewport.getWidth(),
               screen->getHeight() * viewport.getHeight() );
}

void GLESRenderer::beginRender( void )
{
    crimild::Renderer::beginRender();
}

void GLESRenderer::endRender( void )
{
    crimild::Renderer::endRender();
}

void GLESRenderer::clearBuffers( void )
{
    const RGBAColorf &clearColor = getScreenBuffer()->getClearColor();
    glClearColor( clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a() );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void GLESRenderer::bindUniform( ShaderLocationPtr const &location, int value )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    if ( location != nullptr && location->isValid() ) {
        glUniform1i( location->getLocation(), value );
    }
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GLESRenderer::bindUniform( ShaderLocationPtr const &location, float value )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    if ( location != nullptr && location->isValid() ) {
        glUniform1f( location->getLocation(), value );
    }
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GLESRenderer::bindUniform( ShaderLocationPtr const &location, const Vector3f &vector )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    if ( location != nullptr && location->isValid() ) {
        glUniform3fv( location->getLocation(), 1, static_cast< const GLfloat * >( vector.getData() ) );
    }
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GLESRenderer::bindUniform( ShaderLocationPtr const &location, const Vector2f &vector )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    if ( location != nullptr && location->isValid() ) {
        glUniform2fv( location->getLocation(), 1, static_cast< const GLfloat * >( vector.getData() ) );
    }
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GLESRenderer::bindUniform( ShaderLocationPtr const &location, const RGBAColorf &color )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    if ( location != nullptr && location->isValid() ) {
        glUniform4fv( location->getLocation(), 1, static_cast< const GLfloat * >( color.getData() ) );
    }
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GLESRenderer::bindUniform( ShaderLocationPtr const &location, const Matrix4f &matrix )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    if ( location != nullptr && location->isValid() ) {
        glUniformMatrix4fv( location->getLocation(), 1, GL_FALSE, static_cast< const GLfloat * >( matrix.getData() ) );
    }
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GLESRenderer::drawPrimitive( ShaderProgramPtr const &program, PrimitivePtr const &primitive )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    GLenum type;
    switch ( primitive->getType() ) {
        case Primitive::Type::POINTS:
            type = GL_POINTS;
            break;
            
        case Primitive::Type::LINES:
            type = GL_LINES;
            break;
            
        case Primitive::Type::LINE_LOOP:
            type = GL_LINE_LOOP;
            break;
            
        case Primitive::Type::LINE_STRIP:
            type = GL_LINE_STRIP;
            break;
            
        case Primitive::Type::TRIANGLE_FAN:
            type = GL_TRIANGLE_FAN;
            break;
            
        case Primitive::Type::TRIANGLE_STRIP:
            type = GL_TRIANGLE_STRIP;
            break;
            
        case Primitive::Type::TRIANGLES:
        default:
            type = GL_TRIANGLES;
            break;
    }
    
    unsigned short *base = 0;
    glDrawElements( type,
                   primitive->getIndexBuffer()->getIndexCount(),
                   GL_UNSIGNED_SHORT,
                   ( const GLvoid * ) base );
    
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GLESRenderer::drawBuffers( ShaderProgramPtr const &program, Primitive::Type bufferType, VertexBufferObjectPtr const &vbo, unsigned int count )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    GLenum type;
    switch ( bufferType ) {
        case Primitive::Type::POINTS:
            type = GL_POINTS;
            break;
            
        case Primitive::Type::LINES:
            type = GL_LINES;
            break;
            
        case Primitive::Type::LINE_LOOP:
            type = GL_LINE_LOOP;
            break;
            
        case Primitive::Type::LINE_STRIP:
            type = GL_LINE_STRIP;
            break;
            
        case Primitive::Type::TRIANGLE_FAN:
            type = GL_TRIANGLE_FAN;
            break;
            
        case Primitive::Type::TRIANGLE_STRIP:
            type = GL_TRIANGLE_STRIP;
            break;
            
        case Primitive::Type::TRIANGLES:
        default:
            type = GL_TRIANGLES;
            break;
    }
    
    bindVertexBuffer( program, vbo );
    
    glDrawArrays( type, 0, count );
    
    unbindVertexBuffer( program, vbo );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

ShaderProgramPtr GLESRenderer::getFallbackProgram( MaterialPtr const &material, GeometryPtr const &geometry, PrimitivePtr const &primitive )
{
    if ( material == nullptr || geometry == nullptr || primitive == nullptr ) {
        return getShaderProgram( "fallback" );
//        return getShaderProgram( "screen" );
    }
    
    if ( geometry->getComponent< RenderStateComponent >()->hasLights() && primitive->getVertexBuffer()->getVertexFormat().hasNormals() ) {
        return getShaderProgram( "color" );
//        return getShaderProgram( "phong" );
    }
    
    if ( material->getColorMap() && primitive->getVertexBuffer()->getVertexFormat().hasTextureCoords() ) {
        return getShaderProgram( "texture" );
    }
    
    if ( primitive->getVertexBuffer()->getVertexFormat().hasColors() ) {
        return getShaderProgram( "color" );
    }
    
//    return getShaderProgram( "flat" );
    return getShaderProgram( "color" );
}

void GLESRenderer::setAlphaState( AlphaStatePtr const &state )
{
    if ( state->isEnabled() ) {
        glEnable( GL_BLEND );
        
        GLenum srcBlendFunc = GL_SRC_ALPHA;
        switch ( state->getSrcBlendFunc() ) {
            case AlphaState::SrcBlendFunc::ZERO:
                srcBlendFunc = GL_ZERO;
                break;
            case AlphaState::SrcBlendFunc::ONE:
                srcBlendFunc = GL_ONE;
                break;
            case AlphaState::SrcBlendFunc::SRC_COLOR:
                srcBlendFunc = GL_SRC_COLOR;
                break;
            case AlphaState::SrcBlendFunc::ONE_MINUS_SRC_COLOR:
                srcBlendFunc = GL_ONE_MINUS_SRC_COLOR;
                break;
            case AlphaState::SrcBlendFunc::DST_COLOR:
                srcBlendFunc = GL_DST_COLOR;
                break;
            case AlphaState::SrcBlendFunc::ONE_MINUS_DST_COLOR:
                srcBlendFunc = GL_ONE_MINUS_DST_COLOR;
                break;
            case AlphaState::SrcBlendFunc::SRC_ALPHA:
                srcBlendFunc = GL_SRC_ALPHA;
                break;
            case AlphaState::SrcBlendFunc::ONE_MINUS_SRC_ALPHA:
                srcBlendFunc = GL_ONE_MINUS_SRC_ALPHA;
                break;
            case AlphaState::SrcBlendFunc::DST_ALPHA:
                srcBlendFunc = GL_DST_ALPHA;
                break;
            case AlphaState::SrcBlendFunc::ONE_MINUS_DST_ALPHA:
                srcBlendFunc = GL_ONE_MINUS_DST_ALPHA;
                break;
            default:
                break;
        }
        
        GLenum dstBlendFunc = GL_ONE_MINUS_SRC_ALPHA;
        switch ( state->getDstBlendFunc() ) {
            case AlphaState::DstBlendFunc::ZERO:
                dstBlendFunc = GL_ZERO;
                break;
            case AlphaState::DstBlendFunc::ONE:
                dstBlendFunc = GL_ONE;
                break;
            case AlphaState::DstBlendFunc::SRC_COLOR:
                dstBlendFunc = GL_SRC_COLOR;
                break;
            case AlphaState::DstBlendFunc::ONE_MINUS_SRC_COLOR:
                dstBlendFunc = GL_ONE_MINUS_SRC_COLOR;
                break;
            case AlphaState::DstBlendFunc::SRC_ALPHA:
                dstBlendFunc = GL_SRC_ALPHA;
                break;
            case AlphaState::DstBlendFunc::ONE_MINUS_SRC_ALPHA:
                dstBlendFunc = GL_ONE_MINUS_SRC_ALPHA;
                break;
            case AlphaState::DstBlendFunc::DST_ALPHA:
                dstBlendFunc = GL_DST_ALPHA;
                break;
            case AlphaState::DstBlendFunc::ONE_MINUS_DST_ALPHA:
                dstBlendFunc = GL_ONE_MINUS_DST_ALPHA;
                break;
            default:
                break;
        }
        
        glBlendFunc( srcBlendFunc, dstBlendFunc );
    }
    else {
        glDisable( GL_BLEND );
    }
}

void GLESRenderer::setDepthState( DepthStatePtr const &state )
{
    if ( state->isEnabled() ) {
        glEnable( GL_DEPTH_TEST );
    }
    else {
        glDisable( GL_DEPTH_TEST );
    }
}

