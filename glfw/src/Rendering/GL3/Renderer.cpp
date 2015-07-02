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

#include "Renderer.hpp"
#include "ShaderProgramCatalog.hpp"
#include "VertexBufferObjectCatalog.hpp"
#include "IndexBufferObjectCatalog.hpp"
#include "FrameBufferObjectCatalog.hpp"
#include "TextureCatalog.hpp"

#include "Library/FlatShaderProgram.hpp"
#include "Library/GouraudShaderProgram.hpp"
#include "Library/ColorShaderProgram.hpp"
#include "Library/PhongShaderProgram.hpp"
#include "Library/ScreenShaderProgram.hpp"
#include "Library/TextureShaderProgram.hpp"
#include "Library/SepiaToneShaderProgram.hpp"

#include "Programs/DepthShaderProgram.hpp"
#include "Programs/BasicRenderShaderProgram.hpp"
#include "Programs/ForwardRenderShaderProgram.hpp"
#include "Programs/SSAOShaderProgram.hpp"
#include "Programs/SSAOBlendShaderProgram.hpp"
#include "Programs/BlendShaderProgram.hpp"
#include "Programs/BlurShaderProgram.hpp"
#include "Programs/GaussianBlurShaderProgram.hpp"
#include "Programs/SignedDistanceFieldShaderProgram.hpp"
#include "Programs/ScreenAlphaShaderProgram.hpp"
#include "Programs/ScreenRGBShaderProgram.hpp"
#include "Programs/DeferredComposeRenderShaderProgram.hpp"
#include "Programs/DeferredRenderShaderProgram.hpp"
#include "Utils.hpp"

using namespace crimild;

gl3::Renderer::Renderer( void )
    : gl3::Renderer( nullptr )
{
    
}

gl3::Renderer::Renderer( FrameBufferObjectPtr const &screenBuffer )
{
    setShaderProgramCatalog( crimild::alloc< gl3::ShaderProgramCatalog >() );
	setVertexBufferObjectCatalog( crimild::alloc< gl3::VertexBufferObjectCatalog >() );
	setIndexBufferObjectCatalog( crimild::alloc< gl3::IndexBufferObjectCatalog >() );
	setFrameBufferObjectCatalog( crimild::alloc< gl3::FrameBufferObjectCatalog >( this ) );
	setTextureCatalog( crimild::alloc< gl3::TextureCatalog >() );

	addShaderProgram( "flat", crimild::alloc< FlatShaderProgram >() );
	addShaderProgram( "gouraud", crimild::alloc< GouraudShaderProgram >() );
	addShaderProgram( "phong", crimild::alloc< PhongShaderProgram >() );
	addShaderProgram( "color", crimild::alloc< ColorShaderProgram >() );
	addShaderProgram( "screen", crimild::alloc< ScreenShaderProgram >() );
	addShaderProgram( "screen_alpha", crimild::alloc< ScreenAlphaShaderProgram >() );
	addShaderProgram( "screen_rgb", crimild::alloc< ScreenRGBShaderProgram >() );
	addShaderProgram( "texture", crimild::alloc< TextureShaderProgram >() );
    addShaderProgram( "depth", crimild::alloc< DepthShaderProgram >() );
    addShaderProgram( "basic", crimild::alloc< BasicRenderShaderProgram >() );
    addShaderProgram( "forward", crimild::alloc< ForwardRenderShaderProgram >() );
    addShaderProgram( "deferred", crimild::alloc< DeferredRenderShaderProgram >() );
    addShaderProgram( "deferred_compose", crimild::alloc< DeferredComposeRenderShaderProgram >() );
    addShaderProgram( "blend", crimild::alloc< BlendShaderProgram >() );
    addShaderProgram( "blur", crimild::alloc< BlurShaderProgram >() );
    addShaderProgram( "gaussianBlur", crimild::alloc< GaussianBlurShaderProgram >() );
    addShaderProgram( "sdf", crimild::alloc< SignedDistanceFieldShaderProgram >() );
    addShaderProgram( "text", crimild::alloc< SignedDistanceFieldShaderProgram >() );
    
    AssetManager::getInstance()->add( AssetManager::SHADER_PROGRAM_UNLIT_TEXTURE, crimild::alloc< TextureShaderProgram >(), true );

	setScreenBuffer( screenBuffer );
}

gl3::Renderer::~Renderer( void )
{

}

void gl3::Renderer::configure( void )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Log::Debug << "Configuring renderer"
    		   << "\n       OpenGL version: " << glGetString( GL_VERSION )
    		   << "\n       GLSL version: " << glGetString( GL_SHADING_LANGUAGE_VERSION )
    		   << "\n       Vendor: " << glGetString( GL_VENDOR )
    		   << "\n       Renderer: " << glGetString( GL_RENDERER )
    		   << Log::End;

	glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
	if ( glewInit() != GLEW_OK ) {
		Log::Fatal << "Cannot initialize GLEW" << Log::End;
		exit( 1 );
	}

	if ( !GLEW_VERSION_3_2 ) {
		Log::Fatal << "OpenGL 3.2 API is not available" << Log::End;
		exit( 1 );
    }

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
    glEnable( GL_PROGRAM_POINT_SIZE );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void gl3::Renderer::setViewport( const Rectf &viewport )
{
    auto screen = getScreenBuffer();
    glViewport(
        screen->getWidth() * viewport.getX(),
        screen->getHeight() * viewport.getY(),
        screen->getWidth() * viewport.getWidth(),
        screen->getHeight() * viewport.getHeight() );
}

void gl3::Renderer::beginRender( void )
{
    crimild::Renderer::beginRender();
}

void gl3::Renderer::endRender( void )
{
    crimild::Renderer::endRender();
}

void gl3::Renderer::clearBuffers( void )
{
	const RGBAColorf &clearColor = getScreenBuffer()->getClearColor();
	glClearColor( clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a() );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void gl3::Renderer::bindUniform( ShaderLocationPtr const &location, int value )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniform1i( location->getLocation(), value );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void gl3::Renderer::bindUniform( ShaderLocationPtr const &location, float value )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniform1f( location->getLocation(), value );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void gl3::Renderer::bindUniform( ShaderLocationPtr const &location, const Vector3f &vector )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniform3fv( location->getLocation(), 1, static_cast< const GLfloat * >( vector.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void gl3::Renderer::bindUniform( ShaderLocationPtr const &location, const Vector2f &vector )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniform2fv( location->getLocation(), 1, static_cast< const GLfloat * >( vector.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void gl3::Renderer::bindUniform( ShaderLocationPtr const &location, const RGBAColorf &color )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniform4fv( location->getLocation(), 1, static_cast< const GLfloat * >( color.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void gl3::Renderer::bindUniform( ShaderLocationPtr const &location, const Matrix4f &matrix )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniformMatrix4fv( location->getLocation(), 1, GL_FALSE, static_cast< const GLfloat * >( matrix.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void gl3::Renderer::drawPrimitive( ShaderProgramPtr const &program, PrimitivePtr const &primitive )
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

void gl3::Renderer::drawBuffers( ShaderProgramPtr const &program, Primitive::Type bufferType, VertexBufferObjectPtr const &vbo, unsigned int count )
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

ShaderProgramPtr gl3::Renderer::getFallbackProgram( MaterialPtr const &material, GeometryPtr const &geometry, PrimitivePtr const &primitive )
{
	if ( material == nullptr || geometry == nullptr || primitive == nullptr ) {
		return getShaderProgram( "screen" );
	}

	if ( geometry->getComponent< RenderStateComponent >()->hasLights() && primitive->getVertexBuffer()->getVertexFormat().hasNormals() ) {
		return getShaderProgram( "phong" );
	}

	if ( material->getColorMap() && primitive->getVertexBuffer()->getVertexFormat().hasTextureCoords() ) {
		return getShaderProgram( "texture" );
	}

	if ( primitive->getVertexBuffer()->getVertexFormat().hasColors() ) {
		return getShaderProgram( "color" );
	}

	return getShaderProgram( "flat" );
}

void gl3::Renderer::setAlphaState( AlphaStatePtr const &state )
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

void gl3::Renderer::setDepthState( DepthStatePtr const &state )
{
	if ( state->isEnabled() ) {
		glEnable( GL_DEPTH_TEST );
	}
	else {
		glDisable( GL_DEPTH_TEST );
	}
}

