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
#include "Rendering/OpenGLUtils.hpp"

#include <Rendering/IndexBufferObject.hpp>

using namespace crimild;
using namespace crimild::opengl;

IndexBufferObjectCatalog::IndexBufferObjectCatalog( void )
{

}

IndexBufferObjectCatalog::~IndexBufferObjectCatalog( void )
{

}

int IndexBufferObjectCatalog::getNextResourceId( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    GLuint id;
    glGenBuffers( 1, &id );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
    
    return id;
}

void IndexBufferObjectCatalog::bind( IndexBufferObject *ibo )
{
	if ( ibo == nullptr ) return;
	
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	Catalog< IndexBufferObject >::bind( ibo );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo->getCatalogId() );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void IndexBufferObjectCatalog::unbind( IndexBufferObject *ibo )
{
	if ( ibo == nullptr ) return;
	
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	Catalog< IndexBufferObject >::unbind( ibo );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void IndexBufferObjectCatalog::load( IndexBufferObject *ibo )
{
	if ( ibo == nullptr ) return;
	
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	Catalog< IndexBufferObject >::load( ibo );

	int id = ibo->getCatalogId();
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, id );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
		sizeof( IndexPrecision ) * ibo->getIndexCount(), 
		ibo->getData(), 
		GL_STATIC_DRAW );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void IndexBufferObjectCatalog::unload( IndexBufferObject *ibo )
{
	if ( ibo == nullptr ) return;
	
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    _unusedIBOIds.push_back( ibo->getCatalogId() );
	Catalog< IndexBufferObject >::unload( ibo );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void IndexBufferObjectCatalog::cleanup( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
    for ( auto id : _unusedIBOIds ) {
        GLuint bufferId = id;
        glDeleteBuffers( 1, &bufferId );
    }
    
    _unusedIBOIds.clear();
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

