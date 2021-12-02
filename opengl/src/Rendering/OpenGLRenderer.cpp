/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#include "OpenGLRenderer.hpp"

#include "Catalogs/ShaderProgramCatalog.hpp"
#include "Catalogs/VertexBufferObjectCatalog.hpp"
#include "Catalogs/IndexBufferObjectCatalog.hpp"
#include "Catalogs/FrameBufferObjectCatalog.hpp"
#include "Catalogs/TextureCatalog.hpp"
#include "Catalogs/RenderTargetCatalog.hpp"
#include "Catalogs/PrimitiveCatalog.hpp"

#include "Programs/StandardShaderProgram.hpp"
#include "Programs/LitTextureShaderProgram.hpp"
#include "Programs/UnlitTextureShaderProgram.hpp"
#include "Programs/UnlitDiffuseShaderProgram.hpp"
#include "Programs/ScreenTextureShaderProgram.hpp"
#include "Programs/SignedDistanceFieldShaderProgram.hpp"
#include "Programs/TextShaderProgram.hpp"
#include "Programs/DepthShaderProgram.hpp"
#include "Programs/ColorTintShaderProgram.hpp"
#include "Programs/UnlitVertexColorShaderProgram.hpp"
#include "Programs/ParticleSystemShaderProgram.hpp"
#include "Programs/DebugDepthShaderProgram.hpp"
#include "Programs/DepthPassShaderProgram.hpp"

#include "Rendering/ImageEffects/ColorTintImageEffect.hpp"

#include "Rendering/ShaderGraph/OpenGLShaderGraph.hpp"

#include <Rendering/AlphaState.hpp>
#include <Rendering/DepthState.hpp>
#include <Rendering/ColorMaskState.hpp>
#include <Rendering/CullFaceState.hpp>
#include <Rendering/FrameBufferObject.hpp>

#include <Rendering/ShaderGraph/ShaderGraph.hpp>

#include <SceneGraph/Light.hpp>

using namespace crimild;
using namespace crimild::shadergraph;
using namespace crimild::opengl;

OpenGLRenderer::OpenGLRenderer( void )
    : OpenGLRenderer( nullptr )
{
    
}

OpenGLRenderer::OpenGLRenderer( SharedPointer< FrameBufferObject > const &screenBuffer )
{
    setShaderProgramCatalog( crimild::alloc< ShaderProgramCatalog >() );
	setVertexBufferObjectCatalog( crimild::alloc< VertexBufferObjectCatalog >() );
	setIndexBufferObjectCatalog( crimild::alloc< IndexBufferObjectCatalog >() );
	setFrameBufferObjectCatalog( crimild::alloc< FrameBufferObjectCatalog >( this ) );
	setTextureCatalog( crimild::alloc< TextureCatalog >() );
	setRenderTargetCatalog( crimild::alloc< RenderTargetCatalog >( this ) );
	setPrimitiveCatalog( crimild::alloc< PrimitiveCatalog >() );

	if ( screenBuffer != nullptr ) {
		setScreenBuffer( screenBuffer );
	}

	// TODO: Move these calls to 'configure()'?
    setShaderProgram( Renderer::SHADER_PROGRAM_RENDER_PASS_FORWARD_LIGHTING, crimild::alloc< StandardShaderProgram >() );
    setShaderProgram( Renderer::SHADER_PROGRAM_RENDER_PASS_DEPTH, crimild::alloc< DepthPassShaderProgram >() );
	
    setShaderProgram( Renderer::SHADER_PROGRAM_RENDER_PASS_STANDARD, crimild::alloc< StandardShaderProgram >() );
    
    setShaderProgram( Renderer::SHADER_PROGRAM_LIT_TEXTURE, crimild::alloc< StandardShaderProgram >() );
    
    setShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_TEXTURE, crimild::alloc< UnlitTextureShaderProgram >() );
	setShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_DIFFUSE, crimild::alloc< UnlitDiffuseShaderProgram >() );
	setShaderProgram( Renderer::SHADER_PROGRAM_UNLIT_VERTEX_COLOR, crimild::alloc< UnlitVertexColorShaderProgram >() );
    
	setShaderProgram( Renderer::SHADER_PROGRAM_PARTICLE_SYSTEM, crimild::alloc< ParticleSystemShaderProgram >() );
	setShaderProgram( Renderer::SHADER_PROGRAM_POINT_SPRITE, crimild::alloc< ParticleSystemShaderProgram >() );

#ifdef CRIMILD_PLATFORM_DESKTOP
    setShaderProgram( Renderer::SHADER_PROGRAM_TEXT_SDF, crimild::alloc< SignedDistanceFieldShaderProgram >() );
#endif
    setShaderProgram( Renderer::SHADER_PROGRAM_TEXT_BASIC, crimild::alloc< TextShaderProgram >() );

	setShaderProgram( Renderer::SHADER_PROGRAM_SCREEN_TEXTURE, crimild::alloc< ScreenTextureShaderProgram >() );

	setShaderProgram( Renderer::SHADER_PROGRAM_DEPTH, crimild::alloc< DepthShaderProgram >() );

	// image effects
	setShaderProgram( ColorTintImageEffect::COLOR_TINT_PROGRAM_NAME, crimild::alloc< ColorTintShaderProgram >() );

	setShaderProgram( Renderer::SHADER_PROGRAM_DEBUG_DEPTH, crimild::alloc< DebugDepthShaderProgram >() );
}

OpenGLRenderer::~OpenGLRenderer( void )
{

}

void OpenGLRenderer::configure( void )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

#if !defined( CRIMILD_PLATFORM_MOBILE ) && !defined( CRIMILD_PLATFORM_EMSCRIPTEN )
	Log::info( CRIMILD_CURRENT_CLASS_NAME,
               "Configuring renderer",
               "\n       OpenGL version: ", glGetString( GL_VERSION ),
    		   "\n       GLSL version: ", glGetString( GL_SHADING_LANGUAGE_VERSION ),
    		   "\n       Vendor: ", glGetString( GL_VENDOR ),
               "\n       Renderer: ", glGetString( GL_RENDERER ) );

	glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
	if ( glewInit() != GLEW_OK ) {
        Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Cannot initialize GLEW" );
		exit( 1 );
	}

	if ( !GLEW_VERSION_3_3 ) {
        Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "OpenGL 3.3 API is not available" );
		exit( 1 );
    }
#endif

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

#ifdef CRIMILD_PLATFORM_DESKTOP
    glEnable( GL_PROGRAM_POINT_SIZE );
#endif
    
    if ( getScreenBuffer() != nullptr ) {
        // default FBO is not always 0
        int defaultFBO;
        glGetIntegerv( GL_FRAMEBUFFER_BINDING, &defaultFBO );
        getScreenBuffer()->setCatalogInfo( getFrameBufferObjectCatalog(), defaultFBO );
    }
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::setViewport( const Rectf &viewport )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    auto screen = getScreenBuffer();
    glViewport(
        screen->getWidth() * viewport.getX(),
        screen->getHeight() * viewport.getY(),
        screen->getWidth() * viewport.getWidth(),
        screen->getHeight() * viewport.getHeight() );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::beginRender( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    Renderer::beginRender();
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::endRender( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    Renderer::endRender();
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::clearBuffers( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	const RGBAColorf &clearColor = getScreenBuffer()->getClearColor();
	glClearColor( clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a() );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::bindUniform( ShaderLocation *location, int value )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniform1i( location->getLocation(), value );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::bindUniform( ShaderLocation *location, float value )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniform1f( location->getLocation(), value );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::bindUniform( ShaderLocation *location, const Vector3f &vector )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniform3fv( location->getLocation(), 1, static_cast< const GLfloat * >( vector.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::bindUniform( ShaderLocation *location, const Vector2f &vector )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniform2fv( location->getLocation(), 1, static_cast< const GLfloat * >( vector.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::bindUniform( ShaderLocation *location, const RGBAColorf &color )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniform4fv( location->getLocation(), 1, static_cast< const GLfloat * >( color.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::bindUniform( ShaderLocation *location, const Matrix4f &matrix )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniformMatrix4fv( location->getLocation(), 1, GL_FALSE, static_cast< const GLfloat * >( matrix.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::bindUniform( ShaderLocation *location, const Matrix3f &matrix )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location != nullptr && location->isValid() ) {
		glUniformMatrix3fv( location->getLocation(), 1, GL_FALSE, static_cast< const GLfloat * >( matrix.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::bindLight( ShaderLocation *location, crimild::Size index, Light *light )
{
	if ( location == nullptr || light == nullptr ) {
		return;
	}

	auto program = location->getProgram();
	if ( program == nullptr ) {
		return;
	}

	auto lightType = light->getType();
	
	auto ambient = RGBColorf::ZERO;
	auto diffuse = RGBColorf::ZERO;
	auto hasDirection = 0.0f;
	auto direction = -Vector3f::ZERO;
	auto hasPosition = 0.0f;
	auto position = Vector3f::ZERO;
	auto hasAttenuation = 0.0f;
	auto attenuation = Vector3f::ZERO;
	auto innerCutOff = 0.0f;
	auto outerCutOff = 0.0f;

	switch ( lightType ) {
		case Light::Type::AMBIENT:
			ambient = light->getAmbient().rgb();
			break;

		case Light::Type::DIRECTIONAL:
			diffuse = light->getColor().rgb();
			hasDirection = 1.0f;
			direction = -light->getDirection();
			break;

		case Light::Type::POINT:
			diffuse = light->getColor().rgb();
			hasPosition = 1.0f;
			position = light->getWorld().getTranslate();
			hasAttenuation = 1.0f;
			attenuation = light->getAttenuation();
			break;

		case Light::Type::SPOT:
			diffuse = light->getColor().rgb();
			hasDirection = 1.0f;
			direction = light->getDirection();
			hasPosition = 1.0f;
			position = light->getWorld().getTranslate();
			hasAttenuation = 1.0f;
			attenuation = light->getAttenuation();
			innerCutOff = Numericf::cos( light->getInnerCutoff() );
			outerCutOff = Numericf::cos( light->getOuterCutoff() );
			break;

		default:
			break;
	}

	{
		auto locName = OpenGLUtils::buildArrayShaderLocationName( "uLights", index, "ambient" );
		auto loc = glGetUniformLocation( program->getCatalogId(), locName.c_str() );
		if ( loc >= 0 ) {
			glUniform3fv( loc, 1, static_cast< const GLfloat * >( ambient.getData() ) );
		}
	}

	{
		auto loc = glGetUniformLocation(
			program->getCatalogId(),
			OpenGLUtils::buildArrayShaderLocationName( "uLights", index, "diffuse" ).c_str() );
		if ( loc >= 0 ) {
			glUniform3fv( loc, 1, static_cast< const GLfloat * >( diffuse.getData() ) );
		}
	}

	{
		auto loc = glGetUniformLocation(
			program->getCatalogId(),
			OpenGLUtils::buildArrayShaderLocationName( "uLights", index, "hasDirection" ).c_str() );
		if ( loc >= 0 ) {
			glUniform1f( loc, hasDirection );
		}
	}

	{
		auto loc = glGetUniformLocation(
			program->getCatalogId(),
			OpenGLUtils::buildArrayShaderLocationName( "uLights", index, "direction" ).c_str() );
		if ( loc >= 0 ) {
			glUniform3fv( loc, 1, static_cast< const GLfloat * >( direction.getData() ) );
		}
	}

	{
		auto loc = glGetUniformLocation(
			program->getCatalogId(),
			OpenGLUtils::buildArrayShaderLocationName( "uLights", index, "hasPosition" ).c_str() );
		if ( loc >= 0 ) {
			glUniform1f( loc, hasPosition );
		}
	}

	{
		auto loc = glGetUniformLocation(
			program->getCatalogId(),
			OpenGLUtils::buildArrayShaderLocationName( "uLights", index, "position" ).c_str() );
		if ( loc >= 0 ) {
			glUniform3fv( loc, 1, static_cast< const GLfloat * >( position.getData() ) );
		}
	}

	{
		auto loc = glGetUniformLocation(
			program->getCatalogId(),
			OpenGLUtils::buildArrayShaderLocationName( "uLights", index, "hasAttenuation" ).c_str() );
		if ( loc >= 0 ) {
			glUniform1f( loc, hasAttenuation );
		}
	}

	{
		auto loc = glGetUniformLocation(
			program->getCatalogId(),
			OpenGLUtils::buildArrayShaderLocationName( "uLights", index, "attenuation" ).c_str() );
		if ( loc >= 0 ) {
			glUniform3fv( loc, 1, static_cast< const GLfloat * >( attenuation.getData() ) );
		}
	}

	{
		auto loc = glGetUniformLocation(
			program->getCatalogId(),
			OpenGLUtils::buildArrayShaderLocationName( "uLights", index, "innerCutOff" ).c_str() );
		if ( loc >= 0 ) {
			glUniform1f( loc, innerCutOff );
		}
	}

	{
		auto loc = glGetUniformLocation(
			program->getCatalogId(),
			OpenGLUtils::buildArrayShaderLocationName( "uLights", index, "outerCutOff" ).c_str() );
		if ( loc >= 0 ) {
			glUniform1f( loc, outerCutOff );
		}
	}

}

void OpenGLRenderer::unbindLight( ShaderLocation *location, crimild::Size index, Light *light )
{
	// no-op
}

void OpenGLRenderer::drawPrimitive( ShaderProgram *program, Primitive *primitive )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	GLenum type = OpenGLUtils::PRIMITIVE_TYPE[ ( uint8_t ) primitive->getType() ];

	unsigned short *base = 0;
	glDrawElements( type,
				   primitive->getIndexBuffer()->getIndexCount(),
				   GL_UNSIGNED_SHORT,
				   ( const GLvoid * ) base );
	

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::drawBuffers( ShaderProgram *program, Primitive::Type bufferType, VertexBufferObject *vbo, unsigned int count )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	GLenum type = OpenGLUtils::PRIMITIVE_TYPE[ ( uint8_t ) bufferType ];

	bindVertexBuffer( program, vbo );

	glDrawArrays( type, 0, count );

	unbindVertexBuffer( program, vbo );	

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::setAlphaState( AlphaState *state )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	if ( state->isEnabled() ) {
		glEnable( GL_BLEND );

		GLenum srcBlendFunc = OpenGLUtils::ALPHA_SRC_BLEND_FUNC[ ( uint8_t ) state->getSrcBlendFunc() ];
		GLenum dstBlendFunc = OpenGLUtils::ALPHA_DST_BLEND_FUNC[ ( uint8_t ) state->getDstBlendFunc() ];

		glBlendFunc( srcBlendFunc, dstBlendFunc );
	}
	else {
		glDisable( GL_BLEND );
	}
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::setDepthState( DepthState *state )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	if ( state->isEnabled() ) {
		glEnable( GL_DEPTH_TEST );
	}
	else {
		glDisable( GL_DEPTH_TEST );
	}
    
    GLenum compareFunc = OpenGLUtils::DEPTH_COMPARE_FUNC[ ( uint8_t ) state->getCompareFunc() ];
    glDepthFunc( compareFunc );
    
    glDepthMask( state->isWritable() ? GL_TRUE : GL_FALSE );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::setCullFaceState( CullFaceState *state )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	if ( state->isEnabled() ) {
	    glEnable( GL_CULL_FACE );

	    GLenum mode = OpenGLUtils::CULL_FACE_MODE[ ( uint8_t ) state->getCullFaceMode() ];
    	glCullFace( mode );

	}
	else {
		glDisable( GL_CULL_FACE );
	}
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void OpenGLRenderer::setColorMaskState( ColorMaskState *state )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	if ( state->isEnabled() ) {
		glColorMask( 
			state->getRMask() ? GL_TRUE : GL_FALSE,
			state->getGMask() ? GL_TRUE : GL_FALSE,
			state->getBMask() ? GL_TRUE : GL_FALSE,
			state->getAMask() ? GL_TRUE : GL_FALSE );
	}
	else {
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	}

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

SharedPointer< ShaderGraph > OpenGLRenderer::createShaderGraph( void )
{
	return crimild::alloc< OpenGLShaderGraph >();
}

