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

#include "TextureCatalog.hpp"

#include "Rendering/OpenGLUtils.hpp"

#include <Foundation/Log.hpp>
#include <Rendering/Texture.hpp>
#include <Rendering/ShaderLocation.hpp>

// TODO: this will cause visual artifacts in some platforms. use with care
#ifndef GL_BGR
    #define GL_BGR GL_RGB
#endif
#ifndef GL_BGRA
	#define GL_BGRA GL_RGBA
#endif
#ifndef GL_RED
#define GL_RED 0x1903
#endif

#ifndef GL_RGBA16F
#define GL_RGBA16F 0x881A
#endif
#ifndef GL_RGB16F
#define GL_RGB16F 0x881B
#endif

using namespace crimild;
using namespace crimild::opengl;

TextureCatalog::TextureCatalog( void )
	: _boundTextureCount( 0 )
{

}

TextureCatalog::~TextureCatalog( void )
{

}

int TextureCatalog::getNextResourceId( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	GLuint textureId = 0;
	glGenTextures( 1, &textureId );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
    
    return textureId;
}

void TextureCatalog::bind( Texture *texture )
{
	if ( texture == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid texture pointer" );
		return;
	}

	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< Texture >::bind( texture );
	
	if ( texture->getCatalog() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Could not bind texture" );
		return;
	}

	glBindTexture( GL_TEXTURE_2D, texture->getCatalogId() );

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void TextureCatalog::unbind( Texture *texture )
{
	if ( texture == nullptr ) {
		return;
	}
	
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	glBindTexture( GL_TEXTURE_2D, 0 );
	
	Catalog< Texture >::unbind( texture );

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
	
}

void TextureCatalog::bind( ShaderLocation *location, Texture *texture )
{
	if ( texture == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid texture pointer" );
		return;
	}

	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< Texture >::bind( location, texture );

	if ( texture->getCatalog() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Could not bind texture" );
		return;
	}

	if ( location && location->isValid() ) {
		glActiveTexture( GL_TEXTURE0 + _boundTextureCount );
		glBindTexture( GL_TEXTURE_2D, texture->getCatalogId() );
		glUniform1i( location->getLocation(), _boundTextureCount );

		++_boundTextureCount;
	} 

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void TextureCatalog::unbind( ShaderLocation *location, Texture *texture )
{
	if ( texture == nullptr ) {
		return;
	}
	
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( _boundTextureCount > 0 ) {
		--_boundTextureCount;
		glActiveTexture( GL_TEXTURE0 + _boundTextureCount );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	
	Catalog< Texture >::unbind( location, texture );

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void TextureCatalog::load( Texture *texture )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< Texture >::load( texture );

	int textureId = texture->getCatalogId();
    glBindTexture( GL_TEXTURE_2D, textureId );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLUtils::TEXTURE_FILTER_MAP[ ( uint8_t ) texture->getMinFilter() ] );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLUtils::TEXTURE_FILTER_MAP[ ( uint8_t ) texture->getMagFilter() ] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLUtils::TEXTURE_WRAP_MODE_CLAMP[ ( uint8_t ) texture->getWrapMode() ] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLUtils::TEXTURE_WRAP_MODE_CLAMP[ ( uint8_t ) texture->getWrapMode() ] );

	auto image = texture->getImage();
	auto width = image->getWidth();
	auto height = image->getHeight();

	GLenum internalFormat = GL_INVALID_ENUM;
	GLenum textureFormat = GL_INVALID_ENUM;
	crimild::Bool useFloatTexture = image->getPixelType() == Image::PixelType::FLOAT;
	GLenum textureType = useFloatTexture ? GL_FLOAT : GL_UNSIGNED_BYTE;

	switch ( image->getPixelFormat() ) {
		case Image::PixelFormat::DEPTH_32:
#ifdef CRIMILD_PLATFORM_DESKTOP
			internalFormat = useFloatTexture ? GL_DEPTH_COMPONENT32F : GL_DEPTH_COMPONENT32;
			textureFormat = GL_DEPTH_COMPONENT;
			break;
#endif
			
		case Image::PixelFormat::DEPTH_24:
#ifdef CRIMILD_PLATFORM_DESKTOP
			internalFormat = GL_DEPTH_COMPONENT24;
			textureFormat = GL_DEPTH_COMPONENT;
			break;
#endif
			
		case Image::PixelFormat::DEPTH_16:
			internalFormat = GL_DEPTH_COMPONENT16;
			textureFormat = GL_DEPTH_COMPONENT16;
			break;
			
		case Image::PixelFormat::RGB:
			internalFormat = useFloatTexture ? GL_RGB16F : GL_RGB;
			textureFormat = GL_RGB;
			break;
			
		case Image::PixelFormat::BGR:
			internalFormat = GL_RGB;
			textureFormat = GL_BGR;
			break;
			
		case Image::PixelFormat::RGBA:
			internalFormat = useFloatTexture ? GL_RGBA16F : GL_RGBA;
			textureFormat = GL_RGBA;
			break;
			
		case Image::PixelFormat::BGRA:
			internalFormat = GL_RGBA;
			textureFormat = GL_BGRA;
			break;

		case Image::PixelFormat::RED:
			internalFormat = GL_RED;
			textureFormat = GL_RED;
			
		default:
			Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid target type: ", ( int ) image->getPixelFormat() );
			break;
	}
    
	GLvoid *data = image->hasData() ? image->getData() : nullptr;

    glTexImage2D(
		GL_TEXTURE_2D,
		0,
		internalFormat, 
    	width,
		height,
		0, 
    	textureFormat, 
    	textureType,
        data );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void TextureCatalog::unload( Texture *texture )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    if ( texture->getCatalogId() > 0 ) {
        _textureIdsToDelete.push_back( texture->getCatalogId() );
    }
    
    Catalog< Texture >::unload( texture );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void TextureCatalog::cleanup( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    for ( auto id : _textureIdsToDelete ) {
        GLuint textureId = id;
        glDeleteTextures( 1, &textureId );
    }
    
    _textureIdsToDelete.clear();
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION
}

