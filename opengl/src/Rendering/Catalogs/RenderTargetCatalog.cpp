/*
 * Copyright (c) 2013-2018, H. Hernan Saez
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

#include "Rendering/Catalogs/RenderTargetCatalog.hpp"
#include "Rendering/OpenGLUtils.hpp"

#include <Rendering/Renderer.hpp>
#include <Rendering/RenderTarget.hpp>
#include <Rendering/Texture.hpp>

using namespace crimild;
using namespace crimild::opengl;

RenderTargetCatalog::RenderTargetCatalog( Renderer *renderer )
    : _renderer( renderer )
{

}

RenderTargetCatalog::~RenderTargetCatalog( void )
{

}

int RenderTargetCatalog::getNextResourceId( RenderTarget * )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	GLuint renderBufferId = 0;
	glGenRenderbuffers( 1, &renderBufferId );
	
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
    
    return renderBufferId;
}

void RenderTargetCatalog::bind( RenderTarget *rt )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	Catalog< RenderTarget >::bind( rt );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void RenderTargetCatalog::unbind( RenderTarget *rt )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< RenderTarget >::unbind( rt );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void RenderTargetCatalog::load( RenderTarget *target )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< RenderTarget >::load( target );

	auto renderBufferId = target->getCatalogId();
	if ( renderBufferId <= 0 ) {
        Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Cannot create Render Target object. Out of memory?" );
        exit( 1 );
	}

	auto targetWidth = target->getWidth();
	auto targetHeight = target->getHeight();
    
	GLenum internalFormat = GL_INVALID_ENUM;
	GLenum textureFormat = GL_RGBA;

	switch ( target->getType() ) {
		case RenderTarget::Type::DEPTH_32:
#if defined( GL_DEPTH_COMPONENT32 ) && !defined( CRIMILD_PLATFORM_EMSCRIPTEN )
			internalFormat = target->useFloatTexture() ? GL_DEPTH_COMPONENT32F : GL_DEPTH_COMPONENT32;
			textureFormat = GL_DEPTH_COMPONENT;
			break;
#endif
			
		case RenderTarget::Type::DEPTH_24:
#if defined( GL_DEPTH_COMPONENT24 ) && !defined( CRIMILD_PLATFORM_EMSCRIPTEN )
			internalFormat = GL_DEPTH_COMPONENT24;
			textureFormat = GL_DEPTH_COMPONENT;
			break;
#endif
			
		case RenderTarget::Type::DEPTH_16:
			internalFormat = GL_DEPTH_COMPONENT16;
			textureFormat = GL_DEPTH_COMPONENT16;
			break;
			
		case RenderTarget::Type::COLOR_RGB:
			internalFormat = target->useFloatTexture() ? GL_RGB16F : GL_RGB;
			textureFormat = GL_RGB;
			break;
			
		case RenderTarget::Type::COLOR_RGBA:
			internalFormat = target->useFloatTexture() ? GL_RGBA16F : GL_RGBA;
			textureFormat = GL_RGBA;
			break;
			
		default:
			Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid target type: ", ( int ) target->getType() );
			break;
	}
    
	if ( internalFormat != GL_INVALID_ENUM ) {
		glBindRenderbuffer( GL_RENDERBUFFER, target->getCatalogId() );
		glRenderbufferStorage( GL_RENDERBUFFER, internalFormat, targetWidth, targetHeight );
        glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	}

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void RenderTargetCatalog::unload( RenderTarget *rt )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	int targetId = rt->getCatalogId();
	if ( targetId > 0 ) {
		_renderbufferIdsToDelete.push_back( targetId );
	}

	Catalog< RenderTarget >::unload( rt );
	
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void RenderTargetCatalog::cleanup( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

    for ( auto id : _renderbufferIdsToDelete ) {
        GLuint renderbufferId = id;
        glDeleteRenderbuffers( 1, &renderbufferId );
    }
    _renderbufferIdsToDelete.clear();

	Catalog< RenderTarget >::cleanup();
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

