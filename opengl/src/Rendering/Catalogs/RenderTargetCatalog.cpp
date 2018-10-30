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

#ifndef GL_RGBA16F
#define GL_RGBA16F 0x881A
#endif
#ifndef GL_RGB16F
#define GL_RGB16F 0x881B
#endif

using namespace crimild;
using namespace crimild::opengl;

RenderTargetCatalog::RenderTargetCatalog( Renderer *renderer )
    : _renderer( renderer )
{

}

RenderTargetCatalog::~RenderTargetCatalog( void )
{

}

int RenderTargetCatalog::getNextResourceId( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	GLuint renderBufferId = 0;
	//glGenRenderbuffers( 1, &renderBufferId );
	
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

void RenderTargetCatalog::load( RenderTarget *rt )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< RenderTarget >::load( rt );

	/*
    int framebufferId = fbo->getCatalogId();
    if ( framebufferId > 0 ) {
        glBindFramebuffer( GL_FRAMEBUFFER, framebufferId );
        
        int colorAttachmentOffset = 0;
        fbo->getRenderTargets().each( [ & ]( std::string rtName, SharedPointer< RenderTarget > &target ) {
            CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
            
            int targetWidth = target->getWidth();
            int targetHeight = target->getHeight();
            
            GLenum internalFormat = GL_INVALID_ENUM;
            GLenum attachment = GL_INVALID_ENUM;
            GLenum textureType = target->useFloatTexture() ? GL_FLOAT : GL_UNSIGNED_BYTE;
            GLenum textureFormat = GL_RGBA;
            switch ( target->getType() ) {
                case RenderTarget::Type::DEPTH_32:
#ifdef CRIMILD_PLATFORM_DESKTOP
                    internalFormat = target->useFloatTexture() ? GL_DEPTH_COMPONENT32F : GL_DEPTH_COMPONENT32;
                    attachment = GL_DEPTH_ATTACHMENT;
                    textureFormat = GL_DEPTH_COMPONENT;
                    break;
#endif
                    
                case RenderTarget::Type::DEPTH_24:
#ifdef CRIMILD_PLATFORM_DESKTOP
                    internalFormat = GL_DEPTH_COMPONENT24;
                    attachment = GL_DEPTH_ATTACHMENT;
                    textureFormat = GL_DEPTH_COMPONENT;
                    break;
#endif
                    
                case RenderTarget::Type::DEPTH_16:
                    internalFormat = GL_DEPTH_COMPONENT16;
                    attachment = GL_DEPTH_ATTACHMENT;
                    textureFormat = GL_DEPTH_COMPONENT16;
                    break;

                case RenderTarget::Type::COLOR_RGB:
                    internalFormat = target->useFloatTexture() ? GL_RGB16F : GL_RGB;
                    textureFormat = GL_RGB;
                    attachment = GL_COLOR_ATTACHMENT0 + colorAttachmentOffset++;
                    break;

                case RenderTarget::Type::COLOR_RGBA:
                    internalFormat = target->useFloatTexture() ? GL_RGBA16F : GL_RGBA;
                    textureFormat = GL_RGBA;
                    attachment = GL_COLOR_ATTACHMENT0 + colorAttachmentOffset++;
                    break;

                default:
                    Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid target type: ", ( int ) target->getType() );
                    break;
            }
            
            if ( internalFormat != GL_INVALID_ENUM && attachment != GL_INVALID_ENUM ) {
				//GLuint renderBufferId;
				//glGenRenderbuffers( 1, &renderBufferId );
				//target->setId( renderBufferId );
                    
				glBindRenderbuffer( GL_RENDERBUFFER, rt->getId() );
				glRenderbufferStorage( GL_RENDERBUFFER, internalFormat, targetWidth, targetHeight );
				//glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, target->getId() );

                if ( target->getOutput() == RenderTarget::Output::RENDER_AND_TEXTURE ) {
                    GLuint textureId;
                    glGenTextures( 1, &textureId );
                    rt->getTexture()->setName( rtName );
                    rt->getTexture()->setCatalogInfo( getRenderer()->getTextureCatalog(), textureId );
                    
                    glBindTexture( GL_TEXTURE_2D, rt->getTexture()->getCatalogId() );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                    glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, targetWidth, targetHeight, 0, textureFormat, textureType, 0 );
                    //glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, target->getTexture()->getCatalogId(), 0 );
                }
            }
            
            CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
        });

        GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
        switch ( status ) {
            case GL_FRAMEBUFFER_COMPLETE:
                Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Framebuffer setup complete" );
                break;
#ifdef GL_FRAMEBUFFER_UNDEFINED
            case GL_FRAMEBUFFER_UNDEFINED:
                Log::fatal( CRIMILD_CURRENT_CLASS_NAME,  "Cannot setup FrameBuffer due to invalid window setup" );
                exit( 1 );
                break;
#endif
            case GL_FRAMEBUFFER_UNSUPPORTED:
                Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Invalid FBO attachments format. Check configuration for each attachment" );
                exit( 1 );
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Cannot setup FrameBuffer. Error configuring attachments" );
                exit( 1 );
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Cannot setup FrameBuffer. No attachments found" );
                exit( 1 );
                break;
#ifdef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Cannot setup FrameBuffer. Multisample params don't match" );
                exit( 1 );
                break;
#endif
#ifdef CRIMILD_PLATFORM_DESKTOP
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Cannot setup FrameBuffer. Attachments are not enabled for drawing" );
                exit( 1 );
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Cannot setup FrameBuffer. Layer params don't match" );
                exit( 1 );
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Cannot setup FrameBuffer. Attachments are not enabled for reading" );
                exit( 1 );
                break;
#endif
            default:
                Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Incomplete framebuffer object. Unknown error code: ", ( int ) status );
                exit( 1 );
                break;
        }

        int defaultFBO = 0;
        if ( getRenderer()->getScreenBuffer() != nullptr ) {
            defaultFBO = getRenderer()->getScreenBuffer()->getCatalogId();
        }
        
        glBindFramebuffer( GL_FRAMEBUFFER, defaultFBO );
    }
    else {
        Log::fatal( CRIMILD_CURRENT_CLASS_NAME, "Cannot create framebuffer object. Out of memory?" );
        exit( 1 );
    }
	*/

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void RenderTargetCatalog::unload( RenderTarget *rt )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	/*
	int targetId = rt->getId();
	if ( targetId > 0 ) {
		_renderbufferIdsToDelete.push_back( targetId );
	}
	
	if ( rt->getOutput() == RenderTarget::Output::RENDER_AND_TEXTURE ) {
		int textureId = rt->getTexture()->getCatalogId();
		_textureIdsToDelete.push_back( textureId );
		rt->getTexture()->setCatalogInfo( nullptr, 0 );
	}
	*/

	Catalog< RenderTarget >::unload( rt );
	
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void RenderTargetCatalog::cleanup( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	/*
    for ( auto id : _textureIdsToDelete ) {
        GLuint textureId = id;
        glDeleteTextures( 1, &textureId );
    }
    _textureIdsToDelete.clear();
    
    for ( auto id : _renderbufferIdsToDelete ) {
        GLuint renderbufferId = id;
        glDeleteRenderbuffers( 1, &renderbufferId );
    }
    _renderbufferIdsToDelete.clear();
	*/

	Catalog< RenderTarget >::cleanup();
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

