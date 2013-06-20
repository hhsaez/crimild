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

#include "FrameBufferObjectCatalog.hpp"
#include "Utils.hpp"

#include <GL/glew.h>
#include <GL/glfw.h>

using namespace crimild;

gl3::FrameBufferObjectCatalog::FrameBufferObjectCatalog( Renderer *renderer )
    : _renderer( renderer )
{

}

gl3::FrameBufferObjectCatalog::~FrameBufferObjectCatalog( void )
{

}

int gl3::FrameBufferObjectCatalog::getNextResourceId( void )
{
    GLuint framebufferId;
    glGenFramebuffers( 1, &framebufferId );
    return framebufferId;
}

void gl3::FrameBufferObjectCatalog::bind( FrameBufferObject *fbo )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< FrameBufferObject >::bind( fbo );

    glBindFramebuffer( GL_FRAMEBUFFER, fbo->getCatalogId() );
    glViewport( 0.0f, 0.0f, fbo->getWidth(), fbo->getHeight() );
    const RGBAColorf &clearColor = fbo->getClearColor();
    glClearColor( clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a() );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void gl3::FrameBufferObjectCatalog::unbind( FrameBufferObject *fbo )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	Catalog< FrameBufferObject >::unbind( fbo );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void gl3::FrameBufferObjectCatalog::load( FrameBufferObject *fbo )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< FrameBufferObject >::load( fbo );

    unsigned int width = fbo->getWidth();
    unsigned int height = fbo->getHeight();

    int framebufferId = fbo->getCatalogId();
    if ( framebufferId > 0 ) {
        glBindFramebuffer( GL_FRAMEBUFFER, framebufferId );

        GLuint colorBuffer;
        glGenRenderbuffers( 1, &colorBuffer );
        glBindRenderbuffer( GL_RENDERBUFFER, colorBuffer );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8, width, height );
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer );

        if ( fbo->getDepthBits() > 0 ) {
            GLuint depthBuffer;
            glGenRenderbuffers( 1, &depthBuffer );
            glBindRenderbuffer( GL_RENDERBUFFER, depthBuffer );
            glRenderbufferStorage( GL_RENDERBUFFER, ( fbo->getDepthBits() == 16 ? GL_DEPTH_COMPONENT16 : GL_DEPTH_COMPONENT24 ), width, height );
            glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer );
        }

        // generate texture that will be used as the rendering target
        GLuint offscreenSurface;
        glGenTextures( 1, &offscreenSurface );
        glBindTexture( GL_TEXTURE_2D, offscreenSurface );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offscreenSurface, 0 );
        fbo->getTexture()->setCatalogInfo( getRenderer()->getTextureCatalog(), offscreenSurface );

        GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
        if ( status != GL_FRAMEBUFFER_COMPLETE ) {
            Log::Error << "Incomplete framebuffer object (error code = " << ( int ) status << Log::End;
            exit( 1 );
        }

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    }
    else {
        Log::Error << "Cannot create framebuffer object (out of memory?)" << Log::End;
        exit( 1 );
    }

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void gl3::FrameBufferObjectCatalog::unload( FrameBufferObject *fbo )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

    GLuint framebufferId = fbo->getCatalogId();
    if ( framebufferId > 0 ) {
        glDeleteFramebuffers( 1, &framebufferId );

        Catalog< FrameBufferObject >::unload( fbo );
    }

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

