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

#include "IndexBufferObjectCatalog.hpp"

#ifdef __APPLE__
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

using namespace crimild;

gles::IndexBufferObjectCatalog::IndexBufferObjectCatalog( void )
{
    
}

gles::IndexBufferObjectCatalog::~IndexBufferObjectCatalog( void )
{
    
}

int gles::IndexBufferObjectCatalog::getNextResourceId( void )
{
    GLuint id;
    glGenBuffers( 1, &id );
    return id;
}

void gles::IndexBufferObjectCatalog::bind( ShaderProgramPtr const &program, IndexBufferObjectPtr const &ibo )
{
    Catalog< IndexBufferObject >::bind( program, ibo );
    
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo->getCatalogId() );
}

void gles::IndexBufferObjectCatalog::unbind( ShaderProgramPtr const &program, IndexBufferObjectPtr const &ibo )
{
    Catalog< IndexBufferObject >::unbind( program, ibo );
    
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void gles::IndexBufferObjectCatalog::load( IndexBufferObjectPtr const &ibo )
{
    Catalog< IndexBufferObject >::load( ibo );
    
    int id = ibo->getCatalogId();
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, id );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                 sizeof( unsigned short ) * ibo->getIndexCount(),
                 ibo->getData(),
                 GL_STATIC_DRAW );
}

void gles::IndexBufferObjectCatalog::unload( IndexBufferObjectPtr const &ibo )
{
    _unusedIBOIds.push_back( ibo->getCatalogId() );
    Catalog< IndexBufferObject >::unload( ibo );
}

void gles::IndexBufferObjectCatalog::unload( IndexBufferObject *ibo )
{
    _unusedIBOIds.push_back( ibo->getCatalogId() );
    Catalog< IndexBufferObject >::unload( ibo );
}

void gles::IndexBufferObjectCatalog::cleanup( void )
{
    for ( auto id : _unusedIBOIds ) {
        GLuint bufferId = id;
        glDeleteBuffers( 1, &bufferId );
    }
    
    _unusedIBOIds.clear();
}

