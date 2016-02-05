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
    
    // this may be wrong. what if there is no depth buffer?
    int fboColorBufferCount = 0;
    fbo->getRenderTargets().each( [&]( std::string, RenderTarget *target ) {
        if ( target->getType() == RenderTarget::Type::COLOR_RGB || target->getType() == RenderTarget::Type::COLOR_RGBA ) {
            fboColorBufferCount++;
        }
    });
    glDrawBuffers( fboColorBufferCount, fboBuffers );
#endif

    glClearColor( clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a() );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
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
        fbo->getRenderTargets().each( [&]( std::string, RenderTarget *target ) {
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
                    Log::Error << "Invalid target type: " << ( int ) target->getType() << Log::End;
                    break;
            }
            
            if ( internalFormat != GL_INVALID_ENUM && attachment != GL_INVALID_ENUM ) {
                if ( target->getOutput() == RenderTarget::Output::RENDER || target->getOutput() == RenderTarget::Output::RENDER_AND_TEXTURE ) {
                    GLuint renderBufferId;
                    glGenRenderbuffers( 1, &renderBufferId );
                    target->setId( renderBufferId );
                    
                    glBindRenderbuffer( GL_RENDERBUFFER, target->getId() );
                    glRenderbufferStorage( GL_RENDERBUFFER, internalFormat, targetWidth, targetHeight );
                    glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, target->getId() );
                }

                if ( target->getOutput() == RenderTarget::Output::TEXTURE || target->getOutput() == RenderTarget::Output::RENDER_AND_TEXTURE ) {
                    GLuint textureId;
                    glGenTextures( 1, &textureId );
                    target->getTexture()->setCatalogInfo( getRenderer()->getTextureCatalog(), textureId );
                    
                    glBindTexture( GL_TEXTURE_2D, target->getTexture()->getCatalogId() );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                    glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, targetWidth, targetHeight, 0, textureFormat, textureType, 0 );
                    glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, target->getTexture()->getCatalogId(), 0 );
                }
            }
            
            CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
        });

        GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
        switch ( status ) {
            case GL_FRAMEBUFFER_COMPLETE:
                Log::Debug << "Framebuffer setup complete" << Log::End;
                break;
            case GL_FRAMEBUFFER_UNDEFINED:
                Log::Error << "Cannot setup FrameBuffer due to invalid window setup" << Log::End;
                exit( 1 );
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                Log::Error << "Invalid FBO attachments format. Check configuration for each attachment" << Log::End;
                exit( 1 );
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                Log::Error << "Cannot setup FrameBuffer. Error configuring attachments" << Log::End;
                exit( 1 );
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                Log::Error << "Cannot setup FrameBuffer. No attachments found" << Log::End;
                exit( 1 );
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                Log::Error << "Cannot setup FrameBuffer. Multisample params don't match" << Log::End;
                exit( 1 );
                break;
#ifdef CRIMILD_PLATFORM_DESKTOP
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                Log::Error << "Cannot setup FrameBuffer. Attachments are not enabled for drawing" << Log::End;
                exit( 1 );
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                Log::Error << "Cannot setup FrameBuffer. Layer params don't match" << Log::End;
                exit( 1 );
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                Log::Error << "Cannot setup FrameBuffer. Attachments are not enabled for reading" << Log::End;
                exit( 1 );
                break;
#endif
            default:
                Log::Error << "Incomplete framebuffer object. Unknown error code: " << ( int ) status << Log::End;
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
        Log::Error << "Cannot create framebuffer object. Out of memory?" << Log::End;
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
        
        fbo->getRenderTargets().each( [&]( std::string, RenderTarget *target ) {
            int targetId = target->getId();
            if ( targetId > 0 ) {
                _renderbufferIdsToDelete.push_back( targetId );
                if ( target->getOutput() == RenderTarget::Output::TEXTURE || target->getOutput() == RenderTarget::Output::RENDER_AND_TEXTURE ) {
                    int textureId = target->getTexture()->getCatalogId();
                    _textureIdsToDelete.push_back( textureId );
                    target->getTexture()->setCatalogInfo( nullptr, 0 );
                }
            }
        });
        
        Catalog< FrameBufferObject >::unload( fbo );
    }

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void FrameBufferObjectCatalog::cleanup( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
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
    
    for ( auto id : _framebufferIdsToDelete ) {
        GLuint framebufferId = id;
        glDeleteFramebuffers( 1, &framebufferId );
    }
    _framebufferIdsToDelete.clear();
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

