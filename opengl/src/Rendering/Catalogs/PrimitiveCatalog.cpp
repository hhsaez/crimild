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

#include "PrimitiveCatalog.hpp"

#include "Rendering/OpenGLUtils.hpp"

#include <Foundation/Log.hpp>
#include <Primitives/Primitive.hpp>
#include <Rendering/ShaderProgram.hpp>
#include <Rendering/Renderer.hpp>

using namespace crimild;
using namespace crimild::opengl;

PrimitiveCatalog::PrimitiveCatalog( void )
{

}

PrimitiveCatalog::~PrimitiveCatalog( void )
{

}

int PrimitiveCatalog::getNextResourceId( Primitive * )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	GLuint primitiveId = 0;
#ifndef CRIMILD_FORCE_OPENGL_COMPATIBILITY_MODE
	glGenVertexArrays( 1, &primitiveId );
#endif
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
    
    return primitiveId;
}

void PrimitiveCatalog::bind( Primitive *primitive )
{
	if ( primitive == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid primitive pointer" );
		return;
	}

	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	auto vbo = primitive->getVertexBuffer();
	auto ibo = primitive->getIndexBuffer();
	if ( vbo->getCatalog() == nullptr || ibo->getCatalog() == nullptr ) {
		// Either vbo or ibo changed, so we need to reload the primitive
		unload( primitive );
	}

	Catalog< Primitive >::bind( primitive );

	if ( primitive->getCatalog() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Could not bind primitive" );
		return;
	}

#ifndef CRIMILD_FORCE_OPENGL_COMPATIBILITY_MODE
	glBindVertexArray( primitive->getCatalogId() );
#else
	// Bind buffers here in compat mode
	auto renderer = Renderer::getInstance();
	renderer->getVertexBufferObjectCatalog()->bind( primitive->getVertexBuffer() );
	renderer->getIndexBufferObjectCatalog()->bind( primitive->getIndexBuffer() );
#endif

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void PrimitiveCatalog::unbind( Primitive *primitive )
{
	if ( primitive == nullptr ) {
		return;
	}
	
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

#ifndef CRIMILD_FORCE_OPENGL_COMPATIBILITY_MODE
    glBindVertexArray( 0 );
#else
	// Unbind buffers here in compat mode
	auto renderer = Renderer::getInstance();
	renderer->getVertexBufferObjectCatalog()->unbind( primitive->getVertexBuffer() );
	renderer->getIndexBufferObjectCatalog()->unbind( primitive->getIndexBuffer() );
#endif
	
	Catalog< Primitive >::unbind( primitive );

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void PrimitiveCatalog::load( Primitive *primitive )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< Primitive >::load( primitive );
    
#ifndef CRIMILD_FORCE_OPENGL_COMPATIBILITY_MODE
	glBindVertexArray( primitive->getCatalogId() );
#endif
	
	auto renderer = Renderer::getInstance();
	renderer->getVertexBufferObjectCatalog()->bind( primitive->getVertexBuffer() );
	renderer->getIndexBufferObjectCatalog()->bind( primitive->getIndexBuffer() );

#ifndef CRIMILD_FORCE_OPENGL_COMPATIBILITY_MODE
	glBindVertexArray( 0 );
#endif

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void PrimitiveCatalog::unload( Primitive *primitive )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
	auto renderer = Renderer::getInstance();
	renderer->getVertexBufferObjectCatalog()->unload( primitive->getVertexBuffer() );
	renderer->getIndexBufferObjectCatalog()->unload( primitive->getIndexBuffer() );

    if ( primitive->getCatalogId() > 0 ) {
        _primitiveIdsToDelete.push_back( primitive->getCatalogId() );
    }
    
    Catalog< Primitive >::unload( primitive );
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void PrimitiveCatalog::cleanup( void )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;
    
#ifndef CRIMILD_FORCE_OPENGL_COMPATIBILITY_MODE
    for ( auto id : _primitiveIdsToDelete ) {
        GLuint primitiveId = id;
        glDeleteVertexArrays( 1, &primitiveId );
    }
#endif
    
    _primitiveIdsToDelete.clear();
    
    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION
}

