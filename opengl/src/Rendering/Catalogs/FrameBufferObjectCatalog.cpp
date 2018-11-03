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

#include "Rendering/Catalogs/FrameBufferObjectCatalog.hpp"
#include "Rendering/OpenGLUtils.hpp"

#include <Rendering/Renderer.hpp>
#include <Rendering/FrameBufferObject.hpp>
#include <Rendering/RenderTarget.hpp>

#ifndef GL_RGBA16F
#define GL_RGBA16F 0x881A
#endif
#ifndef GL_RGB16F
#define GL_RGB16F 0x881B
#endif

using namespace crimild;
using namespace crimild::opengl;

FrameBufferObjectCatalog::FrameBufferObjectCatalog( Renderer *renderer )
    : _renderer( renderer )
{

}

FrameBufferObjectCatalog::~FrameBufferObjectCatalog( void )
{

}

int FrameBufferObjectCatalog::getNextResourceId( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    GLuint framebufferId;
    glGenFramebuffers( 1, &framebufferId );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
    
    return framebufferId;
}

void FrameBufferObjectCatalog::bind( FrameBufferObject *fbo )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	Catalog< FrameBufferObject >::bind( fbo );

    glBindFramebuffer( GL_FRAMEBUFFER, fbo->getCatalogId() );
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
    glViewport( 0.0f, 0.0f, fbo->getWidth(), fbo->getHeight() );
    const RGBAColorf &clearColor = fbo->getClearColor();
    
    glClearColor( clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a() );
    crimild::Int32 clearFlags = fbo->getClearFlags();
	if ( clearFlags != FrameBufferObject::ClearFlag::NONE ) {
        GLbitfield glClearFlags = 0;
        if ( clearFlags & FrameBufferObject::ClearFlag::COLOR ) glClearFlags |= GL_COLOR_BUFFER_BIT;
        if ( clearFlags & FrameBufferObject::ClearFlag::DEPTH ) glClearFlags |= GL_DEPTH_BUFFER_BIT;
        glClear( glClearFlags );
	}
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void FrameBufferObjectCatalog::unbind( FrameBufferObject *fbo )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

    int defaultFBO = 0;
    if ( getRenderer()->getScreenBuffer() != nullptr ) {
        defaultFBO = getRenderer()->getScreenBuffer()->getCatalogId();
    }
    
    glBindFramebuffer( GL_FRAMEBUFFER, defaultFBO );

	Catalog< FrameBufferObject >::unbind( fbo );
    glViewport( 0.0f, 0.0f, getRenderer()->getScreenBuffer()->getWidth(), getRenderer()->getScreenBuffer()->getHeight() );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void FrameBufferObjectCatalog::load( FrameBufferObject *fbo )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< FrameBufferObject >::load( fbo );

    int framebufferId = fbo->getCatalogId();
    if ( framebufferId > 0 ) {
        glBindFramebuffer( GL_FRAMEBUFFER, framebufferId );

        int colorAttachmentOffset = 0;
        fbo->getRenderTargets().each( [ this, &colorAttachmentOffset ]( std::string rtName, SharedPointer< RenderTarget > &target ) {

            getRenderer()->bindRenderTarget( crimild::get_ptr( target ) );

			GLenum attachment = GL_INVALID_ENUM;
			switch ( target->getType() ) {
				case RenderTarget::Type::DEPTH_32:
				case RenderTarget::Type::DEPTH_24:
				case RenderTarget::Type::DEPTH_16:
					attachment = GL_DEPTH_ATTACHMENT;
					break;
					
				case RenderTarget::Type::COLOR_RGB:
				case RenderTarget::Type::COLOR_RGBA:
					attachment = GL_COLOR_ATTACHMENT0 + colorAttachmentOffset++;
					break;
					
				default:
					Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid target type: ", ( int ) target->getType() );
					break;
			}

			glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, target->getCatalogId() );
            if ( target->getOutput() == RenderTarget::Output::RENDER_AND_TEXTURE ) {
                glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, target->getTexture()->getCatalogId(), 0 );
            }

            getRenderer()->unbindRenderTarget( crimild::get_ptr( target ) );
        });

        GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
        switch ( status ) {
            case GL_FRAMEBUFFER_COMPLETE:
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

#ifdef CRIMILD_PLATFORM_DESKTOP
		const GLenum fboBuffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4,
			GL_COLOR_ATTACHMENT5,
			GL_COLOR_ATTACHMENT6,
			GL_COLOR_ATTACHMENT7,
		};
		
		if ( colorAttachmentOffset > 0 ) {
			glDrawBuffers( colorAttachmentOffset, fboBuffers );
		}
		else {
			glDrawBuffer( GL_NONE );
            glReadBuffer( GL_NONE );
		}
#endif

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

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void FrameBufferObjectCatalog::unload( FrameBufferObject *fbo )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    int framebufferId = fbo->getCatalogId();
    if ( framebufferId > 0 ) {
        _framebufferIdsToDelete.push_back( framebufferId );
        Catalog< FrameBufferObject >::unload( fbo );
    }

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void FrameBufferObjectCatalog::cleanup( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

    for ( auto id : _framebufferIdsToDelete ) {
        GLuint framebufferId = id;
        glDeleteFramebuffers( 1, &framebufferId );
    }
    _framebufferIdsToDelete.clear();
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

